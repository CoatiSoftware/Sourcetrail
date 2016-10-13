using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CoatiSoftware.CoatiPlugin.Wizard
{
    public partial class WindowCreateCDB : Form
    {
        public delegate void OnFinishedCreatingCDB(SolutionParser.CompilationDatabase cdb, string directory, string fileName);

        private OnFinishedCreatingCDB m_onFinishedCreateCDB = null;

        private List<EnvDTE.Project> m_projects = new List<EnvDTE.Project>();

        string m_configurationName = "";
        string m_platformName = "";
        string m_targetDir = "";
        string m_fileName = "";
        string m_cStandard = "";

        SolutionParser.CompilationDatabase m_cdb = null;

        public OnFinishedCreatingCDB CallbackOnFinishedCreatingCDB
        {
            get { return m_onFinishedCreateCDB; }
            set { m_onFinishedCreateCDB = value; }
        }

        public List<EnvDTE.Project> Projects
        {
            get { return m_projects; }
            set { m_projects = value; }
        }

        public string ConfigurationName
        {
            get { return m_configurationName; }
            set { m_configurationName = value; }
        }

        public string PlatformName
        {
            get { return m_platformName; }
            set { m_platformName = value; }
        }

        public string TargetDir
        {
            get { return m_targetDir; }
            set { m_targetDir = value; }
        }

        public string FileName
        {
            get { return m_fileName; }
            set { m_fileName = value; }
        }

        public string CStandard
        {
            get { return m_cStandard; }
            set { m_cStandard = value; }
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

        private SolutionParser.CompilationDatabase CreateCDB()
        {
            SolutionParser.CompilationDatabase cdb = new SolutionParser.CompilationDatabase();

            int projectsProcessed = 0;

            foreach (EnvDTE.Project project in m_projects)
            {
                List<SolutionParser.CommandObject> commandObjects = SolutionParser.SolutionParser.CreateCommandObjects(project, m_configurationName, m_platformName, m_cStandard);

                projectsProcessed++;
                float relativProgress = (float)projectsProcessed/(float)m_projects.Count;
                backgroundWorker1.ReportProgress((int)(relativProgress * 100), "Processing project \"" + project.Name + "\"");

                foreach (SolutionParser.CommandObject obj in commandObjects)
                {
                    cdb.AddCommandObject(obj);
                }
            }

            return cdb;
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
            m_cdb = CreateCDB();
        }

        private void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBar.Value = e.ProgressPercentage;
            
            labelStatus.Text = e.UserState as string;
        }

        private void backgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if(e.Cancelled == false && e.Error == null)
            {
                if (m_onFinishedCreateCDB != null)
                {
                    m_onFinishedCreateCDB(m_cdb, m_targetDir, m_fileName);
                }
            }

            Close();
        }

        private void WindowCreateCDB_FormClosed(object sender, FormClosedEventArgs e)
        {
            backgroundWorker1.CancelAsync();
        }
    }
}
