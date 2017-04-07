using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.ComponentModel.Design;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Shell;

using System.Collections.Generic;
using System.IO;

using EnvDTE;
using System.Windows.Forms;

namespace CoatiSoftware.SourcetrailPlugin
{
    public class OptionPageGrid : DialogPage
    {
        private uint _serverPort = 6666;
        private uint _clientPort = 6667;
        private bool _logging = false;
        private bool _obfuscateLogging = false;
        private uint _threadCount = 1;

        public delegate void Callback();
        public static Callback _serverPortChangeCallback = null;
        public static Callback _clientPortChangeCallback = null;
        public static Callback _loggingToggled = null;
        public static Callback _obfuscationToggled = null;
        public static Callback _threadCountChanged = null;

        [Category("Sourcetrail")]
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

        [Category("Sourcetrail")]
        [DisplayName("Sourcetrail Port")]
        [Description("The port on which Sourcetrail will receive messages")]
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

        [Category("Sourcetrail")]
        [DisplayName("File Logging")]
        [Description("Enables or disables file logging for the plugin")]
        public bool LoggingEnabled
        {
            get { return _logging; }
            set
            {
                _logging = value;
                if(_loggingToggled != null)
                {
                    _loggingToggled();
                }
            }
        }

        [Category("Sourcetrail")]
        [DisplayName("Log Obfuscation")]
        [Description("Names will be obfuscated in log files. Note that already logged data will not be obfuscated retroactively. A dictionary file will be created that you can use to make sense of obfuscated logs. Keep this dictionary to yourself!")]
        public bool ObfuscateLogging
        {
            get { return _obfuscateLogging; }
            set
            {
                _obfuscateLogging = value;
                if (_obfuscationToggled != null)
                {
                    _obfuscationToggled();
                }
            }
        }

        [Category("Sourcetrail")]
        [DisplayName("Thread Count")]
        [Description("The maximum number of threads that will be used while building Compilation Databases")]
        public uint ThreadCount
        {
            get { return _threadCount; }
            set
            {
                _threadCount = value;
                if(_threadCountChanged != null)
                {
                    _threadCountChanged();
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
    [Guid(GuidList.guidSourcetrailPluginPkgString)]
    [ProvideAutoLoad(Microsoft.VisualStudio.Shell.Interop.UIContextGuids80.NoSolution)]
    [ProvideOptionPage(typeof(OptionPageGrid), "Sourcetrail", "Sourcetrail Settings", 0, 0, true)]

    public sealed class SourcetrailPluginPackage : Package
    {
        private MenuCommand _menuItemSetActiveToken = null;
        private MenuCommand _menuItemCreateProject = null;
        private MenuCommand _menuItemCreateCDB = null;
        private MenuCommand _menuItemOpenLogDir = null;

        private SolutionEvents _solutionEvents = null;

        private bool _validSolutionLoaded = false;

        Utility.CompilationDatabaseList _cdbList = new Utility.CompilationDatabaseList();

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

        public bool LoggingEnabled
        {
            get
            {
                OptionPageGrid page = (OptionPageGrid)GetDialogPage(typeof(OptionPageGrid));
                return page.LoggingEnabled;
            }
        }

        public bool LogObfuscationEnabled
        {
            get
            {
                OptionPageGrid page = (OptionPageGrid)GetDialogPage(typeof(OptionPageGrid));
                return page.ObfuscateLogging;
            }
        }

        public uint ThreadCount
        {
            get
            {
                OptionPageGrid page = (OptionPageGrid)GetDialogPage(typeof(OptionPageGrid));
                return page.ThreadCount;
            }
        }

        System.Threading.Thread _serverThread = null;



        public SourcetrailPluginPackage()
        {}

        protected override void Initialize()
        {
            base.Initialize();

            InitLogging();
            InitNetwork();

            Utility.FileUtility._errorCallback = new Utility.FileUtility.ErrorCallback(OnFileUtilityError);

            // register callbacks for changes of plugin settings
            OptionPageGrid._serverPortChangeCallback = new OptionPageGrid.Callback(OnServerPortChanged);
            OptionPageGrid._clientPortChangeCallback = new OptionPageGrid.Callback(OnClientPortChanged);
            OptionPageGrid._loggingToggled = new OptionPageGrid.Callback(OnLoggingToggled);
            OptionPageGrid._obfuscationToggled = new OptionPageGrid.Callback(OnObfuscationToggled);

            // register the plugin UI elements
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                CommandID setActiveTokenCommandID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailSetActiveToken);
                CommandID createProjectID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailCreateProject);
                CommandID createCDBID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailCreateCDB);
                CommandID openLogDirID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailOpenLogFolder);

                _menuItemSetActiveToken = new MenuCommand(MenuItemCallback, setActiveTokenCommandID);
                _menuItemCreateProject = new MenuCommand(MenuItemCallback, createProjectID);
                _menuItemCreateCDB = new MenuCommand(MenuItemCallback, createCDBID);
                _menuItemOpenLogDir = new MenuCommand(MenuItemCallback, openLogDirID);

                _menuItemCreateProject.Enabled = false;
                _menuItemSetActiveToken.Enabled = false;
                _menuItemCreateCDB.Enabled = false;
                _menuItemOpenLogDir.Enabled = true;

                mcs.AddCommand(_menuItemSetActiveToken);
                mcs.AddCommand(_menuItemCreateProject);
                mcs.AddCommand(_menuItemCreateCDB);
                mcs.AddCommand(_menuItemOpenLogDir);
            }

            // register callbacks to enable/disable interaction for eligible solutions
            DTE dte = (DTE)GetService(typeof(DTE));
            _solutionEvents = dte.Events.SolutionEvents;

            _solutionEvents.Opened += OnSolutionOpened;
            _solutionEvents.AfterClosing += OnSolutionClosed;

            SendPing();

            Logging.Logging.LogInfo("Initialization done");
        }

        void OnSolutionOpened()
        {
            Logging.Logging.LogInfo("A solution was loaded, checking languages");

            try
            {
                // the sourcetrail ui should only be enabled when the solution contains C/C++ projects
                DTE dte = (DTE)GetService(typeof(DTE));
                List<String> languages = Utility.SolutionUtility.GetSolutionLanguages(dte);

                string solutionPath = Utility.SolutionUtility.GetSolutionPath(dte);

                if(_cdbList.CheckCDBForSolutionExists(solutionPath))
                {
                    Logging.Logging.LogInfo("A CDB for the loaded solution already exists.");
                }

                bool enable = false;
                foreach (String language in languages)
                {
                    if (language == CodeModelLanguageConstants.vsCMLanguageVC
                        || language == CodeModelLanguageConstants.vsCMLanguageMC)
                    {
                        Logging.Logging.LogInfo("C/C++ project was detected");
                        enable = true;
                    }
                }

                if (enable)
                {
                    Logging.Logging.LogInfo("Enabling plugin UI");
                    _menuItemSetActiveToken.Enabled = true;
                    _menuItemCreateProject.Enabled = true;
                    _menuItemCreateCDB.Enabled = true;

                    _validSolutionLoaded = true;
                }
                else
                {
                    _validSolutionLoaded = false;

                    Logging.Logging.LogInfo("No C/C++ project was detected");
                }
            }
            catch(Exception e)
            {
                DisplayMessage("Error", e.Message);
                Logging.Logging.LogError(e.Message);
            }
        }

        void OnSolutionClosed()
        {
            Logging.Logging.LogInfo("Solution closed, disabling plugin UI");

            _menuItemSetActiveToken.Enabled = false;
            _menuItemCreateProject.Enabled = false;
            _menuItemCreateCDB.Enabled = false;

            _validSolutionLoaded = false;
        }

        private void InitNetwork()
        {
            Logging.Logging.LogInfo("Initializing Network with Server Port " + ServerPort.ToString() + " and Client Port " + ClientPort.ToString());

            Utility.AsynchronousSocketListener._port = ServerPort;
            Utility.AsynchronousSocketListener server = new Utility.AsynchronousSocketListener();
            Utility.AsynchronousSocketListener._onReadCallback = new Utility.AsynchronousSocketListener.OnReadCallback(OnNetworkReadCallback);
            Utility.AsynchronousSocketListener._onErrorCallback = new Utility.AsynchronousSocketListener.OnReadCallback(OnNetworkErrorCallback);
            _serverThread = new System.Threading.Thread(server.DoWork);
            _serverThread.Start();

            Utility.AsynchronousClient._port = ClientPort;
            Utility.AsynchronousClient._onErrorCallback = new Utility.AsynchronousSocketListener.OnReadCallback(OnNetworkErrorCallback);
        }

        private void InitLogging()
        {
            DTE dte = (DTE)GetService(typeof(DTE));

            Logging.FileLogger fileLogger = new Logging.FileLogger();
            Logging.VSOutputLogger vsLogger = new Logging.VSOutputLogger(dte);

            Logging.LogManager.GetInstance().Loggers.Add(fileLogger);
            Logging.LogManager.GetInstance().Loggers.Add(vsLogger);
            Logging.LogManager.GetInstance().LoggingEnabled = LoggingEnabled;

            Logging.Obfuscation.NameObfuscator.Enabled(LogObfuscationEnabled);

            Logging.Logging.LogInfo("Logging initialized");
        }

        private void SendPing()
        {
            string message = Utility.NetworkProtocolUtility.CreatePingMessage();

            Utility.AsynchronousClient.Send(message);
        }

        private void OnCreateProject(List<EnvDTE.Project> projects, string configurationName, string platformName, string targetDir, string fileName, string cStandard)
        {
            DTE dte = (DTE)GetService(typeof(DTE));

            Wizard.WindowCreateCDB createCDB = new Wizard.WindowCreateCDB();
            createCDB.Projects = projects;
            createCDB.ConfigurationName = configurationName;
            createCDB.PlatformName = platformName;
            createCDB.TargetDir = targetDir;
            createCDB.FileName = fileName;
            createCDB.CStandard = cStandard;
            createCDB.ThreadCount = (int)ThreadCount;
            createCDB.SolutionDir = Utility.SolutionUtility.GetSolutionPath(dte);

            createCDB.CDB = _cdbList.GetCDBForSolution(createCDB.SolutionDir, targetDir + "\\" + fileName + ".json");

            createCDB.CallbackOnFinishedCreatingCDB = HandleFinishedCDB;

            createCDB.StartWorking();
            createCDB.ShowDialog();
        }

        private void HandleFinishedCDB(Wizard.WindowCreateCDB.CreationResult creationResult)
        {
            if(creationResult._cdb != null && creationResult._cdbDirectory.Length > 0 && creationResult._cdbName.Length > 0)
            {
                _cdbList.AppendOrUpdate(creationResult._cdb);
                _cdbList.SaveMetaData();

                Wizard.WindowCDBReady dialog = new Wizard.WindowCDBReady();
                dialog.setData(creationResult);
                dialog.ShowDialog();

                _cdbList.Refresh();
            }
            else
            {
                Logging.Logging.LogError("Invalid data received");
            }
        }

        private void MenuItemCallback(object sender, EventArgs e)
        {
            MenuCommand menuCommand = sender as MenuCommand;

            DTE dte = (DTE)GetService(typeof(DTE));

            if (menuCommand != null)
            {
                if (menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidSourcetrailSetActiveToken)
                {
                    string fileName = Utility.FileUtility.GetActiveDocumentName(dte);
                    string filePath = Utility.FileUtility.GetActiveDocumentPath(dte);

                    int lineNumber = Utility.FileUtility.GetActiveLineNumber(dte);
                    int columnNumber = Utility.FileUtility.GetActiveColumnNumber(dte);

                    string message = Utility.NetworkProtocolUtility.CreateActivateTokenMessage(filePath + fileName, lineNumber, columnNumber);

                    Utility.AsynchronousClient.Send(message);
                }
                else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidSourcetrailCreateCDB)
                {
                    CreateCompilationDatabase(dte);
                }
                else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidSourcetrailCreateProject)
                {
                    // show hint to use CDB methad (only for CDB beta) and create project on ok
                    Wizard.WindowMessage windowMessage = new Wizard.WindowMessage();
                    windowMessage.Title = "Hint";
                    windowMessage.Message = "Consider using 'Create CDB' if errors arise during Sourcetrail's indexing. This will become standard in the future.";
                    windowMessage.OnOK = CreateSourcetrailProjectOld;
                    windowMessage.RefreshWindow();
                    windowMessage.ShowDialog();
                }
                else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidSourcetrailOpenLogFolder)
                {
                    Utility.SystemUtility.OpenWindowsExplorerAtDirectory(Utility.DataUtility.GetStandardFolderDirectory());
                }
            }
        }

        // will be removed once the CDB project creation is fully integrated
        private void CreateSourcetrailProjectOld()
        {
            DTE dte = (DTE)GetService(typeof(DTE));

            string solutionName = Utility.SolutionUtility.GetSolutionPath(dte);

            if (solutionName == "") // TODO: make a better fallback for non-existant solution
            {
                List<string> items = Utility.SolutionUtility.GetSolutionProjectsFullNames(dte);

                if (items.Count > 0)
                {
                    solutionName = items[0];
                }
            }

            if (solutionName.Length > 0)
            {
                string message = Utility.NetworkProtocolUtility.CreateCreateProjectMessage(solutionName);

                Utility.AsynchronousClient.Send(message);
            }
            else
            {
                DisplayMessage("Sourcetrail", "Can not create a Sourcetrail Project. Please check whether your VS solution is a valid C or C++ solution and is saved.");
            }
        }

        private void OnNetworkReadCallback(string message)
        {
            Utility.NetworkProtocolUtility.MESSAGE_TYPE messageType = Utility.NetworkProtocolUtility.GetMessageType(message);

            if(messageType == Utility.NetworkProtocolUtility.MESSAGE_TYPE.MOVE_CURSOR)
            {
                Utility.NetworkProtocolUtility.CursorPosition cursorPosition = Utility.NetworkProtocolUtility.ParseSetCursorMessage(message);
                if (cursorPosition.Valid)
                {
                    cursorPosition.ColumnNumber += 1; // VS counts columns starting at 1, sourcetrail starts at 0
                    DTE dte = (DTE)GetService(typeof(DTE));
                    if (Utility.FileUtility.OpenSourceFile(dte, cursorPosition.FilePath))
                    {
                        Utility.FileUtility.GoToLine(dte, cursorPosition.LineNumber, cursorPosition.ColumnNumber);

                        Utility.SystemUtility.GetWindowFocus();
                    }
                }
            }
            else if(messageType == Utility.NetworkProtocolUtility.MESSAGE_TYPE.CREATE_CDB)
            {
                if(_validSolutionLoaded)
                {
                    DTE dte = (DTE)GetService(typeof(DTE));

                    CreateCompilationDatabase(dte);
                }
            }
            else if(messageType == Utility.NetworkProtocolUtility.MESSAGE_TYPE.PING)
            {
                SendPing();
            }
        }

        private void OnNetworkErrorCallback(string message)
        {
            Logging.Logging.LogError("Network Error: " + message.ToString());
            DisplayMessage("Sourcetrail Network Error", message);
        }

        private void OnFileUtilityError(string message)
        {
            Logging.Logging.LogError("File Error: " + message.ToString());
            DisplayMessage("Sourcetrail File Error", message);
        }

        private void OnServerPortChanged()
        {
            Logging.Logging.LogInfo("Changing Server Port to " + ServerPort.ToString());

            Utility.AsynchronousSocketListener._port = ServerPort;

            _serverThread.Abort();
            Utility.AsynchronousSocketListener server = new Utility.AsynchronousSocketListener();
            _serverThread = new System.Threading.Thread(server.DoWork);
            _serverThread.Start();
        }

        private void OnClientPortChanged()
        {
            Logging.Logging.LogInfo("Changing Client Port to " + ClientPort.ToString());

            Utility.AsynchronousClient._port = ClientPort;
        }

        private void OnLoggingToggled()
        {
            Logging.LogManager.GetInstance().LoggingEnabled = LoggingEnabled;

            if(LoggingEnabled)
            {
                Logging.Logging.LogInfo("Logging enabled");
            }
        }

        private void OnObfuscationToggled()
        {
            Logging.Obfuscation.NameObfuscator.Enabled(LogObfuscationEnabled);

            if(LogObfuscationEnabled)
            {
                Logging.Logging.LogInfo("Log Obfuscation enabled");
            }
            else
            {
                Logging.Logging.LogInfo("Log Obfuscation disabled");
            }
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

        private void CreateCompilationDatabase(DTE dte)
        {
            Logging.Logging.LogInfo("Preparing CDB dialog");

            Wizard.ProjectSetupWindow window = new Wizard.ProjectSetupWindow();

            Utility.SolutionUtility.SolutionStructure projectStructure = Utility.SolutionUtility.GetSolutionVCProjects(dte);
            List<List<string>> configsAndPlatforms = Utility.SolutionUtility.GetConfigurationAndPlatformNames(dte);

            window._projectStructure = projectStructure;

            window._configurations = configsAndPlatforms[0];
            window._platforms = configsAndPlatforms[1];

            string directory = System.IO.Path.GetDirectoryName(dte.Solution.FullName);
            window._solutionDirectory = directory;

            string solutionName = System.IO.Path.GetFileNameWithoutExtension(dte.Solution.FullName);
            window._solutionFileName = solutionName;

            bool containsCFiles = true; // Utility.SolutionUtility.ContainsCFiles(dte); // takes ridiculously long, I'd rather just display the option by default
            window._containsCFiles = containsCFiles;

            window._cdb = _cdbList.GetMostCurrentCDBForSolution(Utility.SolutionUtility.GetSolutionPath(dte));

            window.UpdateGUI();
            window._onCreateProject = OnCreateProject;
            
            window.ShowDialog();
        }
    }
}
