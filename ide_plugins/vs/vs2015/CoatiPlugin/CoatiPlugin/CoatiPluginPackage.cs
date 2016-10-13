using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.ComponentModel.Design;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Shell;

using System.Collections.Generic;
using System.IO;

using EnvDTE;

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
        private MenuCommand _menuItemSetActiveToken = null;
        private MenuCommand _menuItemCreateProject = null;
        private MenuCommand _menuItemCreateCDB = null;

        private SolutionEvents _solutionEvents = null;

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

            Utility.FileUtility._errorCallback = new Utility.FileUtility.ErrorCallback(OnFileUtilityError);

            OptionPageGrid._serverPortChangeCallback = new OptionPageGrid.Callback(OnServerPortChanged);
            OptionPageGrid._clientPortChangeCallback = new OptionPageGrid.Callback(OnClientPortChanged);

            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                CommandID setActiveTokenCommandID = new CommandID(GuidList.guidCoatiPluginCmdSet, (int)PkgCmdIDList.cmdidCoatiSetActiveToken);
                CommandID createProjectID = new CommandID(GuidList.guidCoatiPluginCmdSet, (int)PkgCmdIDList.cmdidCoatiCreateProject); // cmdidCoatiCreateProject
                CommandID createCDBID = new CommandID(GuidList.guidCoatiPluginCmdSet, (int)PkgCmdIDList.cmdidCoatiCreateCDB);

                _menuItemSetActiveToken = new MenuCommand(MenuItemCallback, setActiveTokenCommandID);
                _menuItemCreateProject = new MenuCommand(MenuItemCallback, createProjectID);
                _menuItemCreateCDB = new MenuCommand(MenuItemCallback, createCDBID);

                _menuItemCreateProject.Enabled = false;
                _menuItemSetActiveToken.Enabled = false;
                _menuItemCreateCDB.Enabled = false;

                mcs.AddCommand(_menuItemSetActiveToken);
                mcs.AddCommand(_menuItemCreateProject);
                mcs.AddCommand(_menuItemCreateCDB);
            }

            DTE dte = (DTE)GetService(typeof(DTE));

            _solutionEvents = dte.Events.SolutionEvents;

            _solutionEvents.Opened += OnSolutionOpened;
            _solutionEvents.AfterClosing += OnSolutionClosed;
        }

        void OnSolutionOpened()
        {
            try
            {
                DTE dte = (DTE)GetService(typeof(DTE));
                List<String> languages = Utility.SolutionUtility.GetSolutionLanguages(dte);

                bool enable = false;
                foreach (String language in languages)
                {
                    if (language == CodeModelLanguageConstants.vsCMLanguageVC
                        || language == CodeModelLanguageConstants.vsCMLanguageMC)
                    {
                        enable = true;
                    }
                }

                if (enable)
                {
                    _menuItemSetActiveToken.Enabled = true;
                    _menuItemCreateProject.Enabled = true;
                    _menuItemCreateCDB.Enabled = true;
                }
            }
            catch(Exception e)
            {
                DisplayMessage("Error", e.Message);
            }
        }

        void OnSolutionClosed()
        {
            _menuItemSetActiveToken.Enabled = false;
            _menuItemCreateProject.Enabled = false;
            _menuItemCreateCDB.Enabled = false;
        }

        private void InitNetwork()
        {
            Utility.AsynchronousSocketListener._port = ServerPort;
            Utility.AsynchronousSocketListener server = new Utility.AsynchronousSocketListener();
            Utility.AsynchronousSocketListener._onReadCallback = new Utility.AsynchronousSocketListener.OnReadCallback(OnNetworkReadCallback);
            Utility.AsynchronousSocketListener._onErrorCallback = new Utility.AsynchronousSocketListener.OnReadCallback(OnNetworkErrorCallback);
            _serverThread = new System.Threading.Thread(server.DoWork);
            _serverThread.Start();

            Utility.AsynchronousClient._port = ClientPort;
            Utility.AsynchronousClient._onErrorCallback = new Utility.AsynchronousSocketListener.OnReadCallback(OnNetworkErrorCallback);
        }

        private void OnCreateProject(List<EnvDTE.Project> projects, string configurationName, string platformName, string targetDir, string fileName, string cStandard)
        {
            Wizard.WindowCreateCDB createCDB = new Wizard.WindowCreateCDB();
            createCDB.Projects = projects;
            createCDB.ConfigurationName = configurationName;
            createCDB.PlatformName = platformName;
            createCDB.TargetDir = targetDir;
            createCDB.FileName = fileName;
            createCDB.CStandard = cStandard;

            createCDB.CallbackOnFinishedCreatingCDB = WriteCDBToFile;

            createCDB.StartWorking();
            createCDB.ShowDialog();
        }

        private void WriteCDBToFile(SolutionParser.CompilationDatabase cdb, string directory, string fileName)
        {
            string content = cdb.SerializeJSON();
            File.WriteAllText(directory + "\\" + fileName + ".json", content);

            string title = "CDB finished";
            string message = "The CDB " + fileName + " was created at directory \"" + directory + "\"\n";
            message += "You can now use it in Coati.";

            Wizard.WindowMessage windowMessage = new Wizard.WindowMessage();
            windowMessage.Title = title;
            windowMessage.Message = message;
            windowMessage.RefreshWindow();
            windowMessage.ShowDialog();
        }

        private void MenuItemCallback(object sender, EventArgs e)
        {
            MenuCommand menuCommand = sender as MenuCommand;

            DTE dte = (DTE)GetService(typeof(DTE));

            if (menuCommand != null)
            {
                if (menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidCoatiSetActiveToken)
                {
                    string fileName = Utility.FileUtility.GetActiveDocumentName(dte);
                    string filePath = Utility.FileUtility.GetActiveDocumentPath(dte);

                    int lineNumber = Utility.FileUtility.GetActiveLineNumber(dte);
                    int columnNumber = Utility.FileUtility.GetActiveColumnNumber(dte);

                    string message = NetworkProtocolUtility.createActivateTokenMessage(filePath + fileName, lineNumber, columnNumber);

                    Utility.AsynchronousClient.Send(message);
                }
                else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidCoatiCreateCDB)
                {
                    CreateCompilationDatabase(dte);
                }
                else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidCoatiCreateProject)
                {
                    // CreateCoatiProjectOld();

                    Wizard.WindowMessage windowMessage = new Wizard.WindowMessage();
                    windowMessage.Title = "Hint";
                    windowMessage.Message = "Consider using 'Create CDB' if errors arise during Coati's indexing. This will become standard in the future.";
                    windowMessage.OnOK = CreateCoatiProjectOld;
                    windowMessage.RefreshWindow();
                    windowMessage.ShowDialog();
                }
            }
        }

        private void CreateCoatiProjectOld()
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
                string message = NetworkProtocolUtility.createCreateProjectMessage(solutionName);

                Utility.AsynchronousClient.Send(message);
            }
            else
            {
                DisplayMessage("Coati", "Can not create a Coati Project. Please check whether your VS solution is a valid C or C++ solution and is saved.");
            }
        }

        private void OnNetworkReadCallback(string message)
        {
            NetworkProtocolUtility.CursorPosition cursorPosition = NetworkProtocolUtility.parseSetCursorMessage(message);
            if (cursorPosition.Valid)
            {
                cursorPosition.ColumnNumber += 1; // VS counts columns starting at 1, coati starts at 0
                DTE dte = (DTE)GetService(typeof(DTE));
                if (Utility.FileUtility.OpenSourceFile(dte, cursorPosition.FilePath))
                {
                    Utility.FileUtility.GoToLine(dte, cursorPosition.LineNumber, cursorPosition.ColumnNumber);

                    Utility.SystemUtility.GetWindowFocus();
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
            Utility.AsynchronousSocketListener._port = ServerPort;

            _serverThread.Abort();
            Utility.AsynchronousSocketListener server = new Utility.AsynchronousSocketListener();
            _serverThread = new System.Threading.Thread(server.DoWork);
            _serverThread.Start();
        }

        private void OnClientPortChanged()
        {
            Utility.AsynchronousClient._port = ClientPort;
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
            Wizard.ProjectSetupWindow window = new Wizard.ProjectSetupWindow();

            Utility.SolutionUtility.SolutionStructure projectStructure = Utility.SolutionUtility.GetSolutionVCProjects(dte);
            List<List<string>> configsAndPlatforms = Utility.SolutionUtility.GetConfigurationAndPlatformNames(dte);

            window.m_projectStructure = projectStructure;

            window.m_configurations = configsAndPlatforms[0];
            window.m_platforms = configsAndPlatforms[1];

            string directory = System.IO.Path.GetDirectoryName(dte.Solution.FullName);
            window.m_solutionDirectory = directory;

            string solutionName = System.IO.Path.GetFileNameWithoutExtension(dte.Solution.FullName);
            window.m_solutionFileName = solutionName;

            bool containsCFiles = true; // Utility.SolutionUtility.ContainsCFiles(dte); // takes ridiculously long
            window.m_containsCFiles = containsCFiles;

            window.UpdateGUI();
            window.m_onCreateProject = OnCreateProject;
            
            window.ShowDialog();
        }
    }
}
