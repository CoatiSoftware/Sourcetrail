using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using System.Net.Sockets;
using System.Net;

namespace CoatiSoftware.CoatiPlugin
{
    public class StateObject
    {
        public Socket _workSocket = null;
        public const int _bufferSize = 1024;
        public byte[] _buffer = new byte[_bufferSize];
        public StringBuilder _stringBuilder = new StringBuilder();
    }

    public class AsynchronousSocketListener
    {
        public static ManualResetEvent _allDone = new ManualResetEvent(false);

        public delegate void OnReadCallback(string message);

        public static OnReadCallback _onReadCallback = null;
        public static OnReadCallback _onErrorCallback = null;

        private static string _endOfMessageToken = "<EOM>";

        public static uint _port = 6666;
        
        public AsynchronousSocketListener()
        {
        }

        public void DoWork()
        {
            StartListening();
        }

        public static void StartListening()
        {
            const string ipAddressString = "127.0.0.1";

            byte[] bytes = new Byte[1024];

            IPAddress ipAddress = IPAddress.Parse(ipAddressString);
            IPEndPoint localEndPoint = new IPEndPoint(ipAddress, (int)_port);

            Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(100);

                while (true)
                {
                    _allDone.Reset();

                    listener.BeginAccept(new AsyncCallback(AcceptCallback), listener);
                    _allDone.WaitOne();
                }
            }
            catch (Exception e)
            {
                //if(_onErrorCallback != null)
                //{
                //    _onErrorCallback(e.ToString());
                //}
            }
        }

        public static void AcceptCallback(IAsyncResult ar)
        {
            _allDone.Set();

            Socket listener = (Socket)ar.AsyncState;
            Socket handler = listener.EndAccept(ar);

            StateObject state = new StateObject();
            state._workSocket = handler;
            handler.BeginReceive(state._buffer, 0, StateObject._bufferSize, 0, new AsyncCallback(ReadCallback), state);
        }

        public static void ReadCallback(IAsyncResult ar)
        {
            String content = String.Empty;

            StateObject state = (StateObject)ar.AsyncState;
            Socket handler = state._workSocket;

            int bytesRead = handler.EndReceive(ar);

            if (bytesRead > 0)
            {
                state._stringBuilder.Append(Encoding.ASCII.GetString(state._buffer, 0, bytesRead));

                content = state._stringBuilder.ToString();
                if (content.IndexOf(_endOfMessageToken) > -1)
                {
                    if (_onReadCallback != null)
                    {
                        _onReadCallback(content);
                    }
                }
                else
                {
                    handler.BeginReceive(state._buffer, 0, StateObject._bufferSize, 0, new AsyncCallback(ReadCallback), state);
                }
            }
        }

        private static void Send(Socket handler, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);
            handler.BeginSend(byteData, 0, byteData.Length, 0, new AsyncCallback(SendCallback), handler);
        }

        private static void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket handler = (Socket)ar.AsyncState;

                int bytesSent = handler.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to client.", bytesSent);
            }
            catch (Exception e)
            {
                if (_onErrorCallback != null)
                {
                    _onErrorCallback(e.ToString());
                }
            }
        }
    }

    public class AsynchronousClient
    {
        public static uint _port = 6667;

        private static ManualResetEvent connectDone = new ManualResetEvent(false);
        private static ManualResetEvent sendDone = new ManualResetEvent(false);

        private static String response = String.Empty;

        public static AsynchronousSocketListener.OnReadCallback _onErrorCallback = null;

        public static void Send(string message)
        {
            IPAddress ipAddress = IPAddress.Parse("127.0.0.1");
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, (int)_port);

            Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            IAsyncResult ar = client.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), client);
            if (!connectDone.WaitOne(1000))
            {
                client.EndConnect(ar);
            }

            Send(client, message);
            sendDone.WaitOne();

            client.Shutdown(SocketShutdown.Both);
            client.Close();
        }

        private static void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                if (client.Connected == false)
                {
                    return;
                }

                client.EndConnect(ar);

                connectDone.Set();
            }
            catch (Exception e)
            {
                if (_onErrorCallback != null)
                {
                    _onErrorCallback(e.ToString());
                }
            }
        }

        private static void Send(Socket client, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            client.BeginSend(byteData, 0, byteData.Length, 0, new AsyncCallback(SendCallback), client);
        }

        private static void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                int bytesSent = client.EndSend(ar);
				
                sendDone.Set();
            }
            catch (Exception e)
            {
                if (_onErrorCallback != null)
                {
                    _onErrorCallback(e.ToString());
                }
            }
        }
    }
}
