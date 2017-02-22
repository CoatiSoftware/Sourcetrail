using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;

namespace CoatiSoftware.CoatiPlugin.Utility
{
    class QueuedFileWriter
    {
        private bool _working = false;
        Thread _workerThread = null;

        private static ReaderWriterLockSlim _statusLock = new ReaderWriterLockSlim();
        private static ReaderWriterLockSlim _queueLock = new ReaderWriterLockSlim();
        private static ReaderWriterLockSlim _fileLock = new ReaderWriterLockSlim();

        private Queue<string> _inputQueue = new Queue<string>();
        private Queue<string> _outputQueue = new Queue<string>();

        private string _targetDirectory = "";
        private string _fileName = "";

        private int _messagesReceived = 0;
        private int _messageWrittenCount = 0;

        public string TargetDirectory
        {
            get { return _targetDirectory; }
            set { _targetDirectory = value; }
        }

        public string FileName
        {
            get { return _fileName; }
            set { _fileName = value; }
        }

        public void pushMessage(string message)
        {
            _queueLock.EnterWriteLock();
            _messagesReceived++;
            try
            {
                _inputQueue.Enqueue(message);
            }
            catch(Exception e)
            {
                Logging.Logging.LogError(e.Message);
            }
            finally
            {
                _queueLock.ExitWriteLock();
            }
        }

        public void startWorking()
        {
            _statusLock.EnterReadLock();
            if (_working == true)
            {
                return;
            }
            _statusLock.ExitReadLock();


            _statusLock.EnterWriteLock();
            _working = true;
            _workerThread = new Thread(new ThreadStart(work));
            _workerThread.Start();
            _statusLock.ExitWriteLock();
        }

        public void stopWorking()
        {
            _statusLock.EnterWriteLock();
            _working = false;
            _statusLock.ExitWriteLock();

            if(_workerThread != null)
            {
                _workerThread.Join();
            }

            // write remaining messages if stop was called
            _queueLock.EnterWriteLock();
            try
            {
                writeQueueToFile(ref _inputQueue);
                writeQueueToFile(ref _outputQueue);

                Logging.Logging.LogInfo("final commit done");
            }
            catch(Exception e)
            {
                Logging.Logging.LogError(e.Message);
            }
            finally
            {
                _queueLock.ExitWriteLock();
            }

            Logging.Logging.LogInfo("Messages received: " + _messagesReceived);
            Logging.Logging.LogInfo("Messages written: " + _messageWrittenCount);
        }

        private void work()
        {
            bool working = true;

            while(working)
            {
                commit();

                _statusLock.EnterReadLock();
                working = _working;
                _statusLock.ExitReadLock();
            }
        }

        private void commit()
        {
            _queueLock.EnterWriteLock();

            Queue<string> tmpQueue = _inputQueue;
            _inputQueue = _outputQueue;
            _outputQueue = tmpQueue;

            _queueLock.ExitWriteLock();


            writeQueueToFile(ref _outputQueue);
        }

        private void writeQueueToFile(ref Queue<string> messageQueue)
        {
            _fileLock.EnterWriteLock();
            try
            {
                while(messageQueue.Count > 0)
                {
                    _messageWrittenCount++;

                    string message = messageQueue.Dequeue();

                    File.AppendAllText(_targetDirectory + "\\" + _fileName, message);
                }
            }
            catch (Exception e)
            {
                Logging.Logging.LogError(e.Message);
            }
            finally
            {
                _fileLock.ExitWriteLock();
            }
        }
    }
}
