using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using CoatiSoftware.SourcetrailPlugin.SolutionParser;

namespace CoatiSoftware.SourcetrailPlugin.Wizard
{
	public partial class WindowCreateCDB : Form
	{
		public struct CreationResult
		{
			public SolutionParser.CompilationDatabase _cdb;
			public string _cdbDirectory;
			public string _cdbName;
			public List<string> _headerDirectories;
		}

		public delegate void OnFinishedCreatingCDB(CreationResult result);

		private OnFinishedCreatingCDB _onFinishedCreateCDB = null;

		private List<EnvDTE.Project> _projects = new List<EnvDTE.Project>();

		private string _configurationName = "";
		private string _platformName = "";
		private string _targetDir = "";
		private string _fileName = "";
		private string _cStandard = "";
		private string _solutionDir = "";
		private List<string> _headerDirectories;

		private SolutionParser.CompilationDatabase _cdb = null;

		CreationResult _result = new CreationResult();

		private int _threadCount = 1;

		private static object _lockObject = new object();
		private static ReaderWriterLockSlim _readWriteLock = new ReaderWriterLockSlim();

		public OnFinishedCreatingCDB CallbackOnFinishedCreatingCDB
		{
			get { return _onFinishedCreateCDB; }
			set { _onFinishedCreateCDB = value; }
		}

		public List<EnvDTE.Project> Projects
		{
			get { return _projects; }
			set { _projects = value; }
		}

		public string ConfigurationName
		{
			get { return _configurationName; }
			set { _configurationName = value; }
		}

		public string PlatformName
		{
			get { return _platformName; }
			set { _platformName = value; }
		}

		public string TargetDir
		{
			get { return _targetDir; }
			set { _targetDir = value; }
		}

		public string FileName
		{
			get { return _fileName; }
			set { _fileName = value; }
		}

		public string CStandard
		{
			get { return _cStandard; }
			set { _cStandard = value; }
		}

		public int ThreadCount
		{
			get { return _threadCount; }
			set { _threadCount = value; }
		}

		public string SolutionDir
		{
			get { return _solutionDir; }
			set { _solutionDir = value; }
		}

		public SolutionParser.CompilationDatabase CDB
		{
			get { return _cdb; }
			set { _cdb = value; }
		}


		public WindowCreateCDB()
		{
			InitializeComponent();

			progressBar.Maximum = 100;
			progressBar.Value = 0;

			backgroundWorker1.WorkerReportsProgress = true;
			backgroundWorker1.WorkerSupportsCancellation = true;
		}

		public void StartWorking()
		{
			// Show();

			backgroundWorker1.RunWorkerAsync();
		}

		private CreationResult CreateCDB()
		{
			CreationResult result = new CreationResult();
			result._cdb = null;
			result._cdbDirectory = "";
			result._cdbName = "";
			result._headerDirectories = new List<string>();

			Logging.Logging.LogInfo("Starting to create CDB");

			SolutionParser.CompilationDatabase cdb = null;
			_headerDirectories = new List<string>();

			try
			{
				System.Diagnostics.Stopwatch watch = new System.Diagnostics.Stopwatch();

				watch.Start();

				cdb = CreateCommandObjects();

				watch.Stop();

				Logging.Logging.LogInfo("Finished, elapsed time: " + watch.ElapsedMilliseconds.ToString() + " ms");

				cdb.Name = _fileName;
				cdb.Directory = _targetDir;
				cdb.SourceProject = _solutionDir;
				cdb.LastUpdated = DateTime.Now;
				cdb.ConfigurationName = _configurationName;
				cdb.PlatformName = _platformName;

				cdb.IncludedProjects = new List<string>();
				foreach (EnvDTE.Project p in _projects)
				{
					cdb.IncludedProjects.Add(p.Name);
				}

				cdb.Clean();
			}
			catch(Exception e)
			{
				Logging.Logging.LogError("Failed to create CDB: " + e.Message);
			}

			result._cdb = cdb;
			result._cdbDirectory = _targetDir;
			result._cdbName = _fileName;
			result._headerDirectories = _headerDirectories;

			Logging.Logging.LogInfo("Done creating CDB");

			return result;
		}

		private SolutionParser.CompilationDatabase CreateCommandObjects()
		{
			SolutionParser.CompilationDatabase cdb = new SolutionParser.CompilationDatabase();
			cdb.Directory = _targetDir;
			cdb.Name = _fileName;

			File.WriteAllText(_targetDir + "\\" + _fileName + ".json", "");
			File.AppendAllText(_targetDir + "\\" + _fileName + ".json", "[\n");

			// Mutex commandObjectMutex = new Mutex();

			Utility.QueuedFileWriter fileWriter = new Utility.QueuedFileWriter(_fileName + ".json", _targetDir);
			fileWriter.StartWorking();

			try
			{
				Multitasking.LimitedThreadsTaskScheduler scheduler = new Multitasking.LimitedThreadsTaskScheduler(_threadCount);
				TaskFactory factory = new TaskFactory(scheduler);
				List<Task> tasks = new List<Task>();

				int projectsProcessed = 0;
				foreach (EnvDTE.Project project in _projects)
				{
					Logging.Logging.LogInfo("Scheduling project \"" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "\" for parsing.");

					Task task = factory.StartNew(() =>
					{
						try
						{
							SolutionParser.SolutionParser solutionParser = new SolutionParser.SolutionParser(new VsPathResolver(_targetDir));

							List<CompileCommand> commands = solutionParser.CreateCompileCommands(project, _configurationName, _platformName, _cStandard);

							lock (_lockObject)
							{
								projectsProcessed++;
								_headerDirectories.AddRange(solutionParser.HeaderDirectories);
							}

							float relativProgress = (float)projectsProcessed / (float)_projects.Count;
							Logging.Logging.LogInfo("Processing project \"" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "\"");
							backgroundWorker1.ReportProgress((int)(relativProgress * 100), "Processing project \"" + project.Name + "\"");

							foreach (CompileCommand command in commands)
							{
								// cdb.AddOrUpdateCommandObject(obj, false); // since the data is written to file right away now, no need to store it
								// the cdb is however still needed to store some meta data later

								string serializedCommand = "";
								foreach (string line in command.SerializeToJson().Split('\n'))
								{
									serializedCommand += "  " + line + "\n";
								}
								serializedCommand = serializedCommand.TrimEnd('\n');

								fileWriter.PushMessage(serializedCommand + ",\n");
							}
						}
						catch (Exception e)
						{
							Logging.Logging.LogError("Failed to create CDB for project with exception: " + e.Message);
							Logging.Logging.LogError("Stack Trace: " + e.StackTrace);
							throw (e);
						}
					});

					tasks.Add(task);
				}

				int threadCount = System.Diagnostics.Process.GetCurrentProcess().Threads.Count;

				Task.WaitAll(tasks.ToArray());

				fileWriter.StopWorking();

				backgroundWorker1.ReportProgress(100, "Writing data to file. This might take several minutes...");
			}
			catch(Exception e)
			{
				Logging.Logging.LogError("Failed to create CDB for solution with exception: " + e.Message);
				Logging.Logging.LogError("Stack Trace: " + e.StackTrace);
			}
			finally
			{
				File.AppendAllText(_targetDir + "\\" + _fileName + ".json", "\n]");
			}

			return cdb;
		}

		private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
		{
			_result = CreateCDB();
		}

		private void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
		{
			if(backgroundWorker1.CancellationPending == false)
			{
				progressBar.Value = e.ProgressPercentage;

				labelStatus.Text = e.UserState as string;
			}
			else
			{
				progressBar.Value = 0;

				labelStatus.Text = "Cancelling...";
			}
		}

		private void backgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			if(e.Cancelled == false && e.Error == null /*&& progressBar.Value >= 100*/)
			{
				_onFinishedCreateCDB?.Invoke(_result);
			}
			else
			{
				Logging.Logging.LogWarning("CDB creation was aborted by user");
			}

			Close();
		}

		private void WindowCreateCDB_FormClosed(object sender, FormClosedEventArgs e)
		{
			backgroundWorker1.CancelAsync();
		}
	}
}
