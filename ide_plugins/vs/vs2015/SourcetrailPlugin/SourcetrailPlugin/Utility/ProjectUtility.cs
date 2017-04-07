using EnvDTE;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Collections;
using System.Collections.Generic;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
    public class ProjectUtility
    {
        public static bool ContainsCFiles(Project project)
        {
            List<ProjectItem> projectItems = GetProjectItems(project);

            try
            {
                foreach (EnvDTE.ProjectItem item in projectItems)
                {
                    if (item.FileCodeModel != null && item.FileCodeModel.Language == CodeModelLanguageConstants.vsCMLanguageVC)
                    {
                        string extension = item.Properties.Item("Extension").Value.ToString();
                        if (extension == ".c")
                        {
                            return true;
                        }
                    }
                }
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
            }

            return false;
        }

        static public List<ProjectItem> GetProjectItems(Project project)
        {
            List<ProjectItem> items = new List<ProjectItem>();

            IEnumerator itemEnumerator = project.ProjectItems.GetEnumerator();

            while (itemEnumerator.MoveNext())
            {
                ProjectItem currentItem = (ProjectItem)itemEnumerator.Current;
                items.Add(GetProjectSubItemsRecursive(currentItem, ref items));
            }

            return items;
        }

        static private ProjectItem GetProjectSubItemsRecursive(ProjectItem item, ref List<ProjectItem> projectItems)
        {
            if (item.ProjectItems == null)
            {
                return item;
            }

            IEnumerator items = item.ProjectItems.GetEnumerator();

            while (items.MoveNext())
            {
                ProjectItem currentItem = (ProjectItem)items.Current;
                projectItems.Add(GetProjectSubItemsRecursive(currentItem, ref projectItems));
            }

            return item;
        }

        // returns true if the project was reloaded, false if the project did not need to be reloaded
        static public Guid ReloadProject(Project project)
        {
            Logging.Logging.LogInfo("Attempting to reload project");

            try
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
                            Logging.Logging.LogInfo("Project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "' with GUID {" + projectGuid.ToString() + "} loaded.");
                            (vsSolution as IVsSolution4).ReloadProject(projectGuid);
                            return projectGuid;
                        }
                        else
                        {
                            Logging.Logging.LogError("Failed to load project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "'");
                        }
                    }
                    else
                    {
                        Logging.Logging.LogError("Failed to retreive IVsHierarchy. Can't load project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "'");
                    }
                }
                else
                {
                    if (project == null)
                    {
                        Logging.Logging.LogWarning("Project is null");
                    }
                    else
                    {
                        Logging.Logging.LogInfo("Project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "' is already loaded");
                    }
                }
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);

                return Guid.Empty;
            }

            return Guid.Empty;
        }

        static public void UnloadProject(Guid guid, DTE dte)
        {
            Logging.Logging.LogInfo("Attempting to unload project with GUID {" + guid.ToString() + "}");

            if (dte == null)
            {
                return;
            }

            try
            {
                ServiceProvider sp = new ServiceProvider(dte as Microsoft.VisualStudio.OLE.Interop.IServiceProvider);
                IVsSolution vsSolution = sp.GetService(typeof(SVsSolution)) as IVsSolution;

                (vsSolution as IVsSolution4).UnloadProject(guid, (uint)_VSProjectUnloadStatus.UNLOADSTATUS_UnloadedByUser);

                Logging.Logging.LogInfo("Done unloading project with GUID {" + guid.ToString() + "}");
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
            }
        }
    }
}
