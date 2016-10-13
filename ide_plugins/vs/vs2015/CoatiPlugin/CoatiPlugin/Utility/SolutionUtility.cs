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

namespace CoatiSoftware.CoatiPlugin.Utility
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
                public Object Foo = null;
                
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
            EnvDTE.Solution solution = dte.Solution;

            return solution.FullName;
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
                if(project.Kind == EnvDTE.Constants.vsProjectKindUnmodeled) // not loaded
                {
                    continue;
                }

                // check it's a c/c++ project
                if(project.CodeModel != null)
                {
                    if(project.CodeModel.Language != CodeModelLanguageConstants.vsCMLanguageVC)
                    {
                        continue;
                    }
                }

                if(project.Kind == "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}")
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

                    projectStructure.Nodes.Add(folderNode);
                }
            }

            return projectStructure;
        }

        private static SolutionStructure.Node GetSubProjects(EnvDTE.Project project)
        {
            ProjectItems projectItems = project.ProjectItems;
            List<Project> items = new List<Project>();

            foreach (ProjectItem item in projectItems)
            {
                Project p = item.Object as Project;
                if(p != null)
                {
                    items.Add(p);
                }
            }

            if(items.Count > 0)
            {
                SolutionStructure.FolderNode folderNode = new SolutionStructure.FolderNode();
                folderNode.Name = project.Name;

                for (int i = 0; i < items.Count; i++)
                {
                    Project item = items[i];

                    SolutionStructure.Node subFolderNode = GetSubProjects(item);
                    folderNode.SubNodes.Add(subFolderNode);
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

        public static List<String> GetSolutionProjectsFullNames(DTE dte)
        {
            List<String> projectNames = new List<String>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            List<Guid> guids = new List<Guid>();

            foreach (EnvDTE.Project project in projects)
            {
                guids.Add(ReloadProject(project));
            }

            projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                projectNames.Add(project.FullName);
            }

            foreach (Guid guid in guids)
            {
                UnloadProject(guid, dte);
            }

            return projectNames;
        }

        public static List<List<String>> GetSolutionProjectItems(DTE dte)
        {
            List<List<String>> projectItems = new List<List<String>>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                EnvDTE.ProjectItems items = project.ProjectItems;                

                List<String> pItems = new List<String>();

                foreach(EnvDTE.ProjectItem item in items)
                {
                    for (short i = 0; i < item.FileCount; i++)
                    {
                        pItems.Add(item.get_FileNames(i));
                    }
                }

                projectItems.Add(pItems);
            }

            return projectItems;
        }

        public static List<String> GetSolutionLanguages(DTE dte)
        {
            List<String> languages = new List<String>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

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
            EnvDTE.Solution solution = dte.Solution;

            return solution.Saved;
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

            return result;
        }

        public static DTE2 GetDTE2(DTE dte)
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
            foreach(DTE2 dte2 in dte2List)
            {
                int m = StringUtility.GetMatchingCharsFromStart(dte.Solution.FullName, dte2.Solution.FullName);
                if(m > maxMatch.Value)
                {
                    maxMatch = new KeyValuePair<DTE2, int>(dte2, m);
                }
            }

            return maxMatch.Key;
        }

        // returns true if the project was reloaded, false if the project did not need to be reloaded
        static public Guid ReloadProject(Project project)
        {
            if (project != null && project.Kind == EnvDTE.Constants.vsProjectKindUnmodeled)
            {
                DTE dte = project.DTE;

                ServiceProvider sp = new ServiceProvider(dte as Microsoft.VisualStudio.OLE.Interop.IServiceProvider);
                IVsSolution vsSolution = sp.GetService(typeof(SVsSolution)) as IVsSolution;

                IVsHierarchy hierarchy;

                string solutionDirectory = "";
                string solutionFile = "";
                string userOptions = "";
                vsSolution.GetSolutionInfo(out solutionDirectory, out solutionFile, out userOptions);

                vsSolution.GetProjectOfUniqueName(solutionDirectory + project.UniqueName, out hierarchy);

                if (hierarchy != null)
                {
                    Guid projectGuid;

                    hierarchy.GetGuidProperty(
                                VSConstants.VSITEMID_ROOT,
                                (int)__VSHPROPID.VSHPROPID_ProjectIDGuid,
                                out projectGuid);

                    if (projectGuid != null)
                    {
                        (vsSolution as IVsSolution4).ReloadProject(projectGuid);
                        return projectGuid;
                    }
                }
            }

            return Guid.Empty;
        }

        static public void UnloadProject(Guid guid, DTE dte)
        {
            if (dte == null)
            {
                return;
            }

            EnvDTE.Solution solution = dte.Solution;

            EnvDTE.Projects projects = solution.Projects;

            ServiceProvider sp = new ServiceProvider(dte as Microsoft.VisualStudio.OLE.Interop.IServiceProvider);
            IVsSolution vsSolution = sp.GetService(typeof(SVsSolution)) as IVsSolution;

            (vsSolution as IVsSolution4).UnloadProject(guid, (uint)_VSProjectUnloadStatus.UNLOADSTATUS_UnloadedByUser);
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
    }
}
