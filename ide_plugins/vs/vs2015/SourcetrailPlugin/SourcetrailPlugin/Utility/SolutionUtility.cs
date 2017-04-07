using System;
using System.Collections.Generic;
using System.Linq;
using EnvDTE;
using EnvDTE80;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
    public class SolutionUtility
    {
        [DllImport("ole32.dll")]
        private static extern void CreateBindCtx(int reserved, out IBindCtx bindCtx);
        [DllImport("ole32.dll")]
        private static extern void GetRunningObjectTable(int reserved, out IRunningObjectTable runningObjectTable);

        public class SolutionStructure
        {
            public class Node
            {
                public enum NodeType
                {
                    UNKNOWN = 0,
                    PROJECT,
                    FOLDER
                };

                public string Name = "";
                public Project Project = null;
                public Object UserData = null;
                
                public virtual NodeType GetNodeType() { throw (new NotImplementedException()); }
            }

            public class FolderNode : Node
            {
                public List<Node> SubNodes = new List<Node>();

                public override NodeType GetNodeType() { return NodeType.FOLDER; }
            }

            public class ProjectNode : Node
            {
                public bool Include = false;

                public override NodeType GetNodeType() { return NodeType.PROJECT; }
            }

            public List<Node> Nodes = new List<Node>();
        }

        public static String GetSolutionPath(DTE dte)
        {
            try
            {
                EnvDTE.Solution solution = dte.Solution;

                return solution.FullName;
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);

                return "N/A";
            }
        }

        public static SolutionStructure GetSolutionVCProjects(DTE dte)
        {
            return GetProjectStructureRecursive(dte); ;
        }

        public static SolutionStructure GetProjectStructureRecursive(DTE dte)
        {
            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects solutionProjects = solution.Projects;

            List<Guid> guids = new List<Guid>();

            SolutionStructure projectStructure = new SolutionStructure();

            foreach(Project project in solutionProjects)
            {
                try
                {
                    if (project.Kind == EnvDTE.Constants.vsProjectKindUnmodeled) // not loaded
                    {
                        continue;
                    }

                    // check it's a c/c++ project
                    if (project.CodeModel != null)
                    {
                        if (project.CodeModel.Language != CodeModelLanguageConstants.vsCMLanguageVC)
                        {
                            continue;
                        }
                    }

                    if (project.Kind == "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}")
                    {
                        SolutionStructure.ProjectNode projectNode = new SolutionStructure.ProjectNode();
                        projectNode.Name = project.Name;
                        projectNode.Project = project;
                        projectNode.Include = false;

                        projectStructure.Nodes.Add(projectNode);
                    }
                    else
                    {
                        SolutionStructure.Node folderNode = GetSubProjects(project);

                        if (folderNode != null)
                        {
                            projectStructure.Nodes.Add(folderNode);
                        }
                        else
                        {
                            Logging.Logging.LogWarning("Subnode was NULL");
                        }
                    }
                }
                catch(Exception e)
                {
                    Logging.Logging.LogError("Exception: " + e.Message);
                }
            }

            return projectStructure;
        }

        private static SolutionStructure.Node GetSubProjects(EnvDTE.Project project)
        {
            ProjectItems projectItems = project.ProjectItems;
            List<Project> items = new List<Project>();

            try
            {
                foreach (ProjectItem item in projectItems)
                {
                    Project p = item.Object as Project;
                    if (p != null)
                    {
                        items.Add(p);
                    }
                }

                if (items.Count > 0)
                {
                    SolutionStructure.FolderNode folderNode = new SolutionStructure.FolderNode();
                    folderNode.Name = project.Name;

                    for (int i = 0; i < items.Count; i++)
                    {
                        Project item = items[i];

                        SolutionStructure.Node subFolderNode = GetSubProjects(item);

                        if(subFolderNode != null)
                        {
                            folderNode.SubNodes.Add(subFolderNode);
                        }
                        else
                        {
                            Logging.Logging.LogWarning("SubNode was NULL");
                        }
                    }

                    return folderNode;
                }
                else
                {
                    SolutionStructure.ProjectNode projectNode = new SolutionStructure.ProjectNode();
                    projectNode.Name = project.Name;
                    projectNode.Project = project;
                    projectNode.Include = false;

                    return projectNode;
                }
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
                return null;
            }
        }

        // Deprecated: remove when old project parsing is retired
        public static List<String> GetSolutionProjectsFullNames(DTE dte)
        {
            List<String> projectNames = new List<String>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            List<Guid> guids = new List<Guid>();

            foreach (EnvDTE.Project project in projects)
            {
                guids.Add(ProjectUtility.ReloadProject(project));
            }

            projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                projectNames.Add(project.FullName);
            }

            foreach (Guid guid in guids)
            {
                ProjectUtility.UnloadProject(guid, dte);
            }

            return projectNames;
        }

        public static List<String> GetSolutionLanguages(DTE dte)
        {
            List<String> languages = new List<String>();

            List<Project> projects = GetSolutionProjectList(dte);

            foreach (EnvDTE.Project project in projects)
            {
                if (project.CodeModel != null)
                {
                    string language = project.CodeModel.Language;
                    languages.Add(language);
                }
            }

            languages = languages.Distinct().ToList();

            return languages;
        }

        public static bool GetSolutionIsSaved(DTE dte)
        {
            try
            {
                EnvDTE.Solution solution = dte.Solution;

                return solution.Saved;
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);

                throw e;
            }
        }

        public static List<List<string>> GetConfigurationAndPlatformNames(DTE dte)
        {
            List<List<string>> result = new List<List<string>>();
            List<string> configNames = new List<string>();
            List<string> platformNames = new List<string>();

            DTE2 dte2 = SolutionUtility.GetDTE2(dte);

            if(dte2 == null)
            {
                return result;
            }

            EnvDTE80.Solution2 solution = (EnvDTE80.Solution2)dte2.Solution;
            if(solution == null)
            {
                return result;
            }

            EnvDTE80.SolutionBuild2 solutionBuild = (EnvDTE80.SolutionBuild2)solution.SolutionBuild;
            if(solutionBuild == null)
            {
                return result;
            }

            try
            {
                foreach (SolutionConfiguration2 solutionConfiguration in solutionBuild.SolutionConfigurations)
                {
                    foreach (SolutionContext context in solutionConfiguration.SolutionContexts)
                    {
                        string configurationName = context.ConfigurationName;
                        configNames.Add(configurationName);

                        string platformName = context.PlatformName;
                        platformNames.Add(platformName);
                    }
                }

                configNames = configNames.Distinct().ToList();
                platformNames = platformNames.Distinct().ToList();

                result.Add(configNames);
                result.Add(platformNames);
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
            }

            return result;
        }

        public static DTE2 GetDTE2(DTE dte)
        {
            try
            {
                List<DTE2> dte2List = new List<DTE2>();

                IRunningObjectTable runningObjectTable = null;
                GetRunningObjectTable(0, out runningObjectTable);

                IEnumMoniker enumMoniker = null;
                runningObjectTable.EnumRunning(out enumMoniker);

                enumMoniker.Reset();
                IntPtr fetched = IntPtr.Zero;
                IMoniker[] moniker = new IMoniker[1];

                while (enumMoniker.Next(1, moniker, fetched) == 0)
                {
                    IBindCtx bindCtx = null;
                    CreateBindCtx(0, out bindCtx);

                    string displayName = "";
                    moniker[0].GetDisplayName(bindCtx, null, out displayName);
                    // add all VisualStudio ROT entries to list
                    if (displayName.StartsWith("!VisualStudio"))
                    {
                        object comObject;
                        runningObjectTable.GetObject(moniker[0], out comObject);
                        dte2List.Add((DTE2)comObject);
                    }
                }

                // find the correct dte2 instance (each running VS instance has one...)
                KeyValuePair<DTE2, int> maxMatch = new KeyValuePair<DTE2, int>(null, 0);
                foreach (DTE2 dte2 in dte2List)
                {
                    int m = StringUtility.GetMatchingCharsFromStart(dte.Solution.FullName, dte2.Solution.FullName);
                    if (m > maxMatch.Value)
                    {
                        maxMatch = new KeyValuePair<DTE2, int>(dte2, m);
                    }
                }

                return maxMatch.Key;
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
                return null;
            }
        }

        static public bool ContainsCFiles(DTE dte)
        {
            List<Project> projects = GetSolutionProjectList(dte); // dte.Solution.Projects;

            foreach (Project project in projects)
            {
                if(ProjectUtility.ContainsCFiles(project) == true)
                {
                    return true;
                }
            }

            return false;
        }

        static public List<EnvDTE.Project> GetSolutionProjectList(DTE dte)
        {
            List<EnvDTE.Project> solutionProjects = new List<EnvDTE.Project>();

            try
            {
                SolutionStructure solutionStructure = GetProjectStructureRecursive(dte);

                Stack<SolutionStructure.Node> nodeStack = new Stack<Utility.SolutionUtility.SolutionStructure.Node>();
                foreach (SolutionStructure.Node node in solutionStructure.Nodes)
                {
                    nodeStack.Push(node);
                }

                while (nodeStack.Count > 0)
                {
                    Utility.SolutionUtility.SolutionStructure.Node node = nodeStack.Pop();

                    string name = node.Name;

                    if (node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.PROJECT)
                    {
                        solutionProjects.Add(node.Project);
                    }
                    else if (node.GetNodeType() == Utility.SolutionUtility.SolutionStructure.Node.NodeType.FOLDER)
                    {
                        Utility.SolutionUtility.SolutionStructure.FolderNode folderNode = node as Utility.SolutionUtility.SolutionStructure.FolderNode;
                        foreach (Utility.SolutionUtility.SolutionStructure.Node subNode in folderNode.SubNodes)
                        {
                            nodeStack.Push(subNode);
                        }
                    }
                }

                return solutionProjects;
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);

                return new List<EnvDTE.Project>();
            }
        }
    }
}
