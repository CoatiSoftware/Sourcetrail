using EnvDTE;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Shell;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.ComponentModel.Design;

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
		private MenuCommand _menuItemCreateCdb = null;
		private MenuCommand _menuItemOpenLogDir = null;

		private SolutionEvents _solutionEvents = null;

		private bool _validSolutionLoaded = false;

		Utility.CompilationDatabaseSettingsList _recentSettingsList = new Utility.CompilationDatabaseSettingsList();

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
			if (null != mcs)
			{
				CommandID setActiveTokenCommandID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailSetActiveToken);
				CommandID createCdbID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailCreateCdb);
				CommandID openLogDirID = new CommandID(GuidList.guidSourcetrailPluginCmdSet, (int)PkgCmdIDList.cmdidSourcetrailOpenLogFolder);

				_menuItemSetActiveToken = new MenuCommand(MenuItemCallback, setActiveTokenCommandID);
				_menuItemCreateCdb = new MenuCommand(MenuItemCallback, createCdbID);
				_menuItemOpenLogDir = new MenuCommand(MenuItemCallback, openLogDirID);
				
				_menuItemSetActiveToken.Enabled = false;
				_menuItemCreateCdb.Enabled = false;
				_menuItemOpenLogDir.Enabled = true;

				mcs.AddCommand(_menuItemSetActiveToken);
				mcs.AddCommand(_menuItemCreateCdb);
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

				if(_recentSettingsList.CheckCdbForSolutionExists(solutionPath))
				{
					Logging.Logging.LogInfo("A Cdb for the loaded solution already exists.");
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
					_menuItemCreateCdb.Enabled = true;

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
			_menuItemCreateCdb.Enabled = false;

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

			Wizard.WindowCreateCdb createCdbWindow = new Wizard.WindowCreateCdb();
			createCdbWindow.Projects = projects;
			createCdbWindow.ConfigurationName = configurationName;
			createCdbWindow.PlatformName = platformName;
			createCdbWindow.TargetDir = targetDir;
			createCdbWindow.FileName = fileName;
			createCdbWindow.CStandard = cStandard;
			createCdbWindow.ThreadCount = (int)ThreadCount;
			createCdbWindow.SolutionDir = Utility.SolutionUtility.GetSolutionPath(dte);

			createCdbWindow.Cdb = _recentSettingsList.GetCdbForSolution(createCdbWindow.SolutionDir, targetDir + "\\" + fileName + ".json");

			createCdbWindow.CallbackOnFinishedCreatingCdb = HandleFinishedCdb;

			createCdbWindow.StartWorking();
			createCdbWindow.ShowDialog();
		}

		private void HandleFinishedCdb(Wizard.WindowCreateCdb.CreationResult creationResult)
		{
			if(creationResult._cdbSettings != null && creationResult._cdbDirectory.Length > 0 && creationResult._cdbName.Length > 0)
			{
				_recentSettingsList.AppendOrUpdate(creationResult._cdbSettings);
				_recentSettingsList.SaveMetaData();

				Wizard.WindowCdbReady dialog = new Wizard.WindowCdbReady();
				dialog.setData(creationResult);
				dialog.ShowDialog();

				_recentSettingsList.Refresh();
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
				else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidSourcetrailCreateCdb)
				{
					CreateCompilationDatabase(dte);
				}
				else if(menuCommand.CommandID.ID == (int)PkgCmdIDList.cmdidSourcetrailOpenLogFolder)
				{
					Utility.SystemUtility.OpenWindowsExplorerAtDirectory(Utility.DataUtility.GetStandardFolderDirectory());
				}
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
				0,		// false
				out result
			));
		}

		private void CreateCompilationDatabase(DTE dte)
		{
			Logging.Logging.LogInfo("Preparing CDB dialog");

			Wizard.ProjectSetupWindow window = new Wizard.ProjectSetupWindow();

			window._projectStructure = Utility.SolutionUtility.GetSolutionVCProjects(dte);
			window._configurations = Utility.SolutionUtility.GetConfigurationNames(dte);
			window._platforms = Utility.SolutionUtility.GetPlatformNames(dte);
			window._solutionDirectory = Path.GetDirectoryName(dte.Solution.FullName);
			window._solutionFileName = Path.GetFileNameWithoutExtension(dte.Solution.FullName);

			bool containsCFiles = true; // Utility.SolutionUtility.ContainsCFiles(dte); // takes ridiculously long, I'd rather just display the option by default
			window._containsCFiles = containsCFiles;

			window._cdb = _recentSettingsList.GetMostCurrentCdbForSolution(Utility.SolutionUtility.GetSolutionPath(dte));

			window.UpdateGUI();
			window._onCreateProject = OnCreateProject;
			
			window.ShowDialog();
		}
	}
}
