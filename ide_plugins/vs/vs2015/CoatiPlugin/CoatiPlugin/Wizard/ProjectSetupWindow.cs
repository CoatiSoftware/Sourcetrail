using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Text.RegularExpressions;

namespace CoatiSoftware.CoatiPlugin.Wizard
{
    public partial class ProjectSetupWindow : Form
    {
        public class SolutionProject
        {
            public string m_name = "";
            public bool m_include = false;

            public SolutionProject(string name, bool include)
            {
                m_name = name;
                m_include = include;
            }
        }

        public delegate void OnCreateProject(List<EnvDTE.Project> projects, string configurationName, string platformName, string targetDir, string fileName, string cStandard);

        // public List<SolutionProject> m_projects = new List<SolutionProject>();
        public Utility.SolutionUtility.SolutionStructure m_projectStructure = new Utility.SolutionUtility.SolutionStructure();

        public List<string> m_configurations = new List<string>();
        public List<string> m_platforms = new List<string>();

        public OnCreateProject m_onCreateProject = null;

        public string m_solutionDirectory = "";
        public string m_solutionFileName = "foo";

        public bool m_containsCFiles = false;

        private List<string> m_cStandards = new List<string>() { "c1x", "gnu1x", "iso9899:201x", "c11", "gnu11", "iso9899:2011",
            "c9x", "gnu9x", "iso9899:199x", "c99", "gnu99", "iso9899:1999", "iso9899:199409", "c90", "gnu90", "iso9899:1990",
            "c89", "gnu89" };
        
        public ProjectSetupWindow()
        {
            InitializeComponent();

            UpdateGUI();

            buttonCreate.Enabled = false;
        }

        public void UpdateGUI()
        {
            Logging.Logging.LogInfo("Populating GUI");

            InitProjectCheckList();
            InitComboBoxConfigurations();
            InitComboBoxPlatforms();
            InitTextBoxTargetDirectory();
            InitTextBoxFileName();
            InitComboBoxCStandard();
        }

        private void InitComboBoxConfigurations()
        {
            Logging.Logging.LogInfo("Adding " + m_configurations.Count.ToString() + " build configurations.");

            foreach(string configuration in m_configurations)
            {
                comboBoxConfiguration.Items.Add(configuration);
            }

            if(comboBoxConfiguration.Items.Count > 0)
            {
                comboBoxConfiguration.SelectedIndex = 0;
            }
        }

        private void InitComboBoxPlatforms()
        {
            Logging.Logging.LogInfo("Adding " + m_platforms.Count.ToString() + " target platforms.");

            foreach (string platform in m_platforms)
            {
                comboBoxPlatform.Items.Add(platform);
            }

            if(comboBoxPlatform.Items.Count > 0)
            {
                comboBoxPlatform.SelectedIndex = 0;
            }
        }

        private void InitProjectCheckList()
        {
            //foreach (SolutionProject project in m_projects)
            //{
            //    treeViewProjects.Nodes.Add(new TreeNode(project.m_name));
            //}

            foreach(Utility.SolutionUtility.SolutionStructure.Node node in m_projectStructure.Nodes)
            {
                if(node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.PROJECT)
                {
                    TreeNode treeNode = new TreeNode(node.Name);
                    node.Foo = treeNode;
                    treeViewProjects.Nodes.Add(treeNode);
                }
                else if(node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.FOLDER)
                {
                    TreeNode[] subNodes = GetSubNodes(node as Utility.SolutionUtility.SolutionStructure.FolderNode);
                    TreeNode treeNode = new TreeNode(node.Name, subNodes);
                    node.Foo = treeNode;
                    treeViewProjects.Nodes.Add(treeNode);
                }
            }
        }

        private TreeNode[] GetSubNodes(Utility.SolutionUtility.SolutionStructure.FolderNode folderNode)
        {
            List<TreeNode> result = new List<TreeNode>();

            foreach(Utility.SolutionUtility.SolutionStructure.Node node in folderNode.SubNodes)
            {
                if(node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.PROJECT)
                {
                    TreeNode treeNode = new TreeNode(node.Name);
                    node.Foo = treeNode;
                    result.Add(treeNode);
                }
                else if(node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.FOLDER)
                {
                    TreeNode[] subNodes = GetSubNodes(node as Utility.SolutionUtility.SolutionStructure.FolderNode);
                    TreeNode treeNode = new TreeNode(node.Name, subNodes);
                    node.Foo = treeNode;
                    result.Add(treeNode);
                }
            }

            return result.ToArray();
        }

        private void InitTextBoxTargetDirectory()
        {
            Logging.Logging.LogInfo("Setting default target directory: \"" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(m_solutionDirectory) + "\"");

            folderBrowserTargetDirectory.SelectedPath = m_solutionDirectory;
            string rootDirectory = folderBrowserTargetDirectory.SelectedPath.ToString();
            textBoxTargetDirectory.Text = rootDirectory;
        }

        private void InitTextBoxFileName()
        {
            Logging.Logging.LogInfo("Setting default file name: '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(m_solutionFileName) + "'");

            textBoxFileName.Text = m_solutionFileName;
        }

        private void InitComboBoxCStandard()
        {
            if(m_containsCFiles == false)
            {
                Logging.Logging.LogInfo("Hiding C Standard selection");

                comboBoxCStandard.Hide();
                labelCStandard.Hide();
            }
            else
            {
                Logging.Logging.LogInfo("Showing C Standard selection");

                comboBoxCStandard.Show();
                labelCStandard.Show();

                foreach(string standard in m_cStandards)
                {
                    comboBoxCStandard.Items.Add(standard);
                }

                comboBoxCStandard.SelectedIndex = 3;
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            Logging.Logging.LogInfo("Close button pressed. Aborting.");

            Close();
        }

        private void buttonCreate_Click(object sender, EventArgs e)
        {
            OnCreate();
        }

        private void OnCreate()
        {
            Logging.Logging.LogInfo("Create button pressed");

            if (m_onCreateProject != null)
            {
                string configurationName = "";
                string platformName = "";

                configurationName = comboBoxConfiguration.SelectedItem as string;
                platformName = comboBoxPlatform.SelectedItem as string;

                Logging.Logging.LogInfo("Configuration " + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(configurationName) + "|" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(platformName) + " was selected.");

                string targetDir = textBoxTargetDirectory.Text;
                
                if(Directory.Exists(targetDir) && CheckFileNameIsValid(textBoxFileName.Text))
                {
                    if (CheckFileExists())
                    {
                        Logging.Logging.LogWarning("A file \"" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(targetDir + "\\" + textBoxFileName.Text) + "\" already exists.");

                        DialogResult result = MessageBox.Show("A file of the chosen name already exists. Do you want to replace it?", "Coati Plugin", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

                        if(result == DialogResult.No)
                        {
                            Logging.Logging.LogInfo("Aborting CDB creation and attempting to make file name unique.");
                            MakeFileNameUnique();
                            return;
                        }
                    }

                    string cStandard = "c11";
                    if(m_containsCFiles)
                    {
                        cStandard = comboBoxCStandard.SelectedItem as string;
                    }

                    Logging.Logging.LogInfo("Setting C standard flag to " + cStandard);

                    m_onCreateProject(GetTreeViewProjectItems(), configurationName, platformName, targetDir, textBoxFileName.Text, cStandard);
                    Close();
                }
                else
                {
                    if(Directory.Exists(targetDir) == false)
                    {
                        Logging.Logging.LogError("The target directory \"" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(targetDir) + "\" does not exist.");
                        MessageBox.Show("The target directory does not exist.", "Coati Plugin", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }

                    if (CheckFileNameIsValid(textBoxFileName.Text) == false)
                    {
                        Logging.Logging.LogError("The chosen file name \"" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(textBoxFileName.Text) + "\" is not valid. I'd almost dare to say it's invalid!");
                        MessageBox.Show("The chosen file name is not valid.", "Coati Plugin", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }
            }
            else
            {
                Logging.Logging.LogError("CDB create callback is not set. Cannot start creating CDB.");
            }
        }

        private bool CheckFileNameIsValid(string fileName)
        {
            Regex badChars = new Regex("[" + Regex.Escape(new string(System.IO.Path.GetInvalidFileNameChars())) + "]");
            if(badChars.IsMatch(fileName))
            {
                return false;
            }

            return true;
        }

        private bool CheckFileExists()
        {
            string fileName = textBoxFileName.Text;
            string path = textBoxTargetDirectory.Text;
            string extension = labelFileNameEnding.Text;

            return File.Exists(path + "\\" + fileName + extension);
        }

        private List<EnvDTE.Project> GetTreeViewProjectItems()
        {
            List<EnvDTE.Project> solutionProjects = new List<EnvDTE.Project>();

            // Stack<TreeNode> treeNodes = new Stack<TreeNode>();
            Stack<Utility.SolutionUtility.SolutionStructure.Node> nodeStack = new Stack<Utility.SolutionUtility.SolutionStructure.Node>();
            foreach(Utility.SolutionUtility.SolutionStructure.Node node in m_projectStructure.Nodes)
            {
                nodeStack.Push(node);
            }

            while(nodeStack.Count > 0)
            {
                Utility.SolutionUtility.SolutionStructure.Node node = nodeStack.Pop();

                bool include = (node.Foo as TreeNode).Checked;

                string name = node.Name;

                if(node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.PROJECT
                    && (node.Foo as TreeNode).Checked == true)
                {
                    solutionProjects.Add(node.Project);
                }
                else if(node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.FOLDER)
                {
                    Utility.SolutionUtility.SolutionStructure.FolderNode folderNode = node as Utility.SolutionUtility.SolutionStructure.FolderNode;
                    foreach(Utility.SolutionUtility.SolutionStructure.Node subNode in folderNode.SubNodes)
                    {
                        nodeStack.Push(subNode);
                    }
                }
            }

            return solutionProjects;
        }

        private void ProjectCheckList_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void buttonSelectAll_Click(object sender, EventArgs e)
        {
            bool select = false;

            // if one or more items are unselected, select all
            // otherwise deselect all
            for (int i = 0; i < treeViewProjects.Nodes.Count; i++)
            {
                if (treeViewProjects.Nodes[i].Checked == false)
                {
                    select = true;
                }
            }

            for (int i = 0; i < treeViewProjects.Nodes.Count; i++)
            {
                treeViewProjects.Nodes[i].Checked = select;
            }

            UpdateCreateButtonEnabled();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void buttonSelect_Click(object sender, EventArgs e)
        {
            if(Directory.Exists(textBoxTargetDirectory.Text))
            {
                folderBrowserTargetDirectory.SelectedPath = textBoxTargetDirectory.Text;
            }

            folderBrowserTargetDirectory.SelectedPath = textBoxTargetDirectory.Text;

            DialogResult result = folderBrowserTargetDirectory.ShowDialog();

            string selectedPath = folderBrowserTargetDirectory.SelectedPath;

            textBoxTargetDirectory.Text = selectedPath;
        }

        private void treeViewProjects_NodeCheckChanged(object sender, TreeViewEventArgs e)
        {
            // treeViewProjects.AfterCheck -= treeViewProjects_NodeCheckChanged;

            Stack<TreeNode> nodeStack = new Stack<TreeNode>();

            foreach (TreeNode subNode in e.Node.Nodes)
            {
                nodeStack.Push(subNode);
            }

            while (nodeStack.Count > 0)
            {
                var node = nodeStack.Pop();

                node.Checked = e.Node.Checked;

                //foreach (TreeNode subNode in node.Nodes)
                //{
                //    nodeStack.Push(subNode);
                //}
            }

            // treeViewProjects.AfterCheck += treeViewProjects_NodeCheckChanged;
        }

        private void textBoxFileName_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void textBoxFileName_Leave(object sender, EventArgs e)
        {
            // MakeFileNameUnique();
        }

        private void MakeFileNameUnique()
        {
            string fileName = textBoxFileName.Text;

            int i = 0;
            while (CheckFileExists())
            {
                ++i;
                textBoxFileName.Text = fileName + i.ToString();
            }
        }

        private void ProjectCheckList_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            
        }

        private void ProjectCheckList_Click(object sender, EventArgs e)
        {
            
        }

        private void ProjectCheckList_MouseUp(object sender, MouseEventArgs e)
        {
            UpdateCreateButtonEnabled();
        }

        private void UpdateCreateButtonEnabled()
        {
            bool anythingChecked = false;

            Stack<TreeNode> treeNodes = new Stack<TreeNode>();
            foreach(TreeNode node in treeViewProjects.Nodes)
            {
                treeNodes.Push(node);
            }

            while(treeNodes.Count > 0)
            {
                TreeNode node = treeNodes.Pop();

                if(node.Nodes.Count <= 0 && node.Checked == true)
                {
                    anythingChecked = true;
                }

                foreach(TreeNode subNode in node.Nodes)
                {
                    treeNodes.Push(subNode);
                }
            }

            buttonCreate.Enabled = anythingChecked;
        }
    }
}
