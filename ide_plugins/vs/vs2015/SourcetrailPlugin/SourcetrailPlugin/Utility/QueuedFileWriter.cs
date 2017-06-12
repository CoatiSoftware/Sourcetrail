using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
	class QueuedFileWriter
	{
		private bool _working = false;
		private Thread _workerThread = null;

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

		public QueuedFileWriter(string fileName, string targetDirectory)
		{
			_fileName = fileName;
			_targetDirectory = targetDirectory;
		}

		public void PushMessage(string message)
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

		public void StartWorking()
		{
			_statusLock.EnterReadLock();
			if (_working == true)
			{
				return;
			}
			_statusLock.ExitReadLock();

			_statusLock.EnterWriteLock();
			_working = true;
			_workerThread = new Thread(new ThreadStart(Work));
			_workerThread.Start();
			_statusLock.ExitWriteLock();
		}

		public void StopWorking()
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
				WriteQueueToFile(ref _inputQueue);
				WriteQueueToFile(ref _outputQueue);

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

		private void Work()
		{
			bool working = true;

			while(working)
			{
				Commit();

				_statusLock.EnterReadLock();
				working = _working;
				_statusLock.ExitReadLock();
			}
		}

		private void Commit()
		{
			{
				_queueLock.EnterWriteLock();
				Queue<string> tmpQueue = _inputQueue;
				_inputQueue = _outputQueue;
				_outputQueue = tmpQueue;
				_queueLock.ExitWriteLock();
			}

			WriteQueueToFile(ref _outputQueue);
		}

		private void WriteQueueToFile(ref Queue<string> messageQueue)
		{
			if (messageQueue.Count > 0)
			{
				_fileLock.EnterWriteLock();

				try
				{
					StreamWriter writer = System.IO.File.AppendText(_targetDirectory + "\\" + _fileName);

					while (messageQueue.Count > 0)
					{
						_messageWrittenCount++;
						writer.WriteLine(messageQueue.Dequeue());
					}

					writer.Close();
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
}
