using System;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.ComponentModel.Design;
using Microsoft.Win32;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;

using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading;

using EnvDTE;
using EnvDTE80;

namespace CoatiSoftware.CoatiPlugin
{
    public class OptionPageGrid : DialogPage
    {
        private uint _serverPort = 6666;
        private uint _clientPort = 6667;

        public delegate void Callback();
        public static Callback _serverPortChangeCallback = null;
        public static Callback _clientPortChangeCallback = null;

        [Category("Coati")]
        [DisplayName("VS Port")]
        [Description("The port on which Visual Studio will receive messages")]
        public uint ServerPort
        {
            get { return _serverPort; }
            set
            {
                _serverPort = value;
                if (_serverPortChangeCallback != null)
                {
                    _serverPortChangeCallback();
                }
            }
        }

        [Category("Coati")]
        [DisplayName("Coati Port")]
        [Description("The port on which Coati will receive messages")]
        public uint ClientPort
        {
            get { return _clientPort; }
            set
            {
                _clientPort = value;
                if (_clientPortChangeCallback != null)
                {
                    _clientPortChangeCallback();
                }
            }
        }

        public OptionPageGrid()
        {
        }
    }

    [PackageRegistration(UseManagedResourcesOnly = true)]
    [InstalledProductRegistration("#110", "#112", "1.0", IconResourceID = 400)]
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [Guid(GuidList.guidCoatiPluginPkgString)]
    [ProvideAutoLoad(Microsoft.VisualStudio.Shell.Interop.UIContextGuids80.NoSolution)]
    [ProvideOptionPage(typeof(OptionPageGrid), "Coati", "Coati Settings", 0, 0, true)]

    public sealed class CoatiPluginPackage : Package
    {
        public uint ServerPort
        {
            get
            {
                OptionPageGrid page = (OptionPageGrid)GetDialogPage(typeof(OptionPageGrid));
                return page.ServerPort;
            }
        }

        public uint ClientPort
        {
            get
            {
                OptionPageGrid page = (OptionPageGrid)GetDialogPage(typeof(OptionPageGrid));
                return page.ClientPort;
            }
        }

        System.Threading.Thread _serverThread = null;

        public CoatiPluginPackage()
        {}

        protected override void Initialize()
        {
            base.Initialize();

            InitNetwork();

            FileUtility._errorCallback = new FileUtility.ErrorCallback(OnFileUtilityError);

            OptionPageGrid._serverPortChangeCallback = new OptionPageGrid.Callback(OnServerPortChanged);
            OptionPageGrid._clientPortChangeCallback = new OptionPageGrid.Callback(OnClientPortChanged);

            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                CommandID setActiveTokenCommandID = new CommandID(GuidList.guidCoatiPluginCmdSet, (int)PkgCmdIDList.cmdidCoatiSetActiveToken);
                MenuCommand menuItemSetActiveToken = new MenuCommand(MenuItemCallback, setActiveTokenCommandID);
                mcs.AddCommand(menuItemSetActiveToken);
            }
        }

        private void InitNetwork()
        {
            AsynchronousSocketListener._port = ServerPort;
            AsynchronousSocketListener server = new AsynchronousSocketListener();
            AsynchronousSocketListener._onReadCallback = new AsynchronousSocketListener.OnReadCallback(OnNetworkReadCallback);
            AsynchronousSocketListener._onErrorCallback = new AsynchronousSocketListener.OnReadCallback(OnNetworkErrorCallback);
            _serverThread = new System.Threading.Thread(server.DoWork);
            _serverThread.Start();

            AsynchronousClient._port = ClientPort;
            AsynchronousClient._onErrorCallback = new AsynchronousSocketListener.OnReadCallback(OnNetworkErrorCallback);
        }

        private void MenuItemCallback(object sender, EventArgs e)
        {
            MenuCommand menuCommand = sender as MenuCommand;

            DTE dte = (DTE)GetService(typeof(DTE));

            if (menuCommand != null)
            {
                if (menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidCoatiSetActiveToken)
                {
                    DisplayMessage("Coati", "" + ServerPort.ToString());

                    string fileName = FileUtility.GetActiveDocumentName(dte);
                    string filePath = FileUtility.GetActiveDocumentPath(dte);

                    int lineNumber = FileUtility.GetActiveLineNumber(dte);
                    int columnNumber = FileUtility.GetActiveColumnNumber(dte);

                    string message = NetworkProtocolUtility.createOutMessage(filePath + fileName, lineNumber, columnNumber);
                    
                    AsynchronousClient.Send(message);
                }
            }
        }

        private void OnNetworkReadCallback(string message)
        {
            NetworkProtocolUtility.CursorPosition cursorPosition = NetworkProtocolUtility.parseInMessage(message);
            if (cursorPosition.Valid)
            {
                cursorPosition.ColumnNumber += 1; // VS counts columns starting at 1, coati starts at 0
                DTE dte = (DTE)GetService(typeof(DTE));
                if (FileUtility.OpenSourceFile(dte, cursorPosition.FilePath))
                {
                    FileUtility.GoToLine(dte, cursorPosition.LineNumber, cursorPosition.ColumnNumber);

                    SystemUtility.GetWindowFocus();
                }
            }
        }

        private void OnNetworkErrorCallback(string message)
        {
            DisplayMessage("Coati Network Error", message);
        }

        private void OnFileUtilityError(string message)
        {
            DisplayMessage("Coati File Error", message);
        }

        private void OnServerPortChanged()
        {
            AsynchronousSocketListener._port = ServerPort;

            _serverThread.Abort();
            AsynchronousSocketListener server = new AsynchronousSocketListener();
            _serverThread = new System.Threading.Thread(server.DoWork);
            _serverThread.Start();
        }

        private void OnClientPortChanged()
        {
            AsynchronousClient._port = ClientPort;
        }

        private void DisplayMessage(string title, string message)
        {
            IVsUIShell uiShell = (IVsUIShell)GetService(typeof(SVsUIShell));
            Guid clsid = Guid.Empty;
            int result;
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(uiShell.ShowMessageBox(
                       0,
                       ref clsid,
                       title,
                       message,
                       string.Empty,
                       0,
                       OLEMSGBUTTON.OLEMSGBUTTON_OK,
                       OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST,
                       OLEMSGICON.OLEMSGICON_INFO,
                       0,        // false
                       out result));
        }
    }
}
