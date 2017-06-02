using EnvDTE;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using VCProjectEngineWrapper;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
	public class ProjectUtility
	{
		public static List<string> GetPropertyNamesAndValues(Properties properties)
		{
			List<string> ret = new List<string>();
			foreach (Property propertiy in properties)
			{
				string name = propertiy.Name;
				string value = "";

				try
				{
					value = propertiy.Value.ToString();
				}
				catch (Exception e)
				{
					value = "Error occurred while converting value to string.";
				}

				ret.Add(name + ": " + value);
			}

			return ret;
		}

		public static bool HasProperty(Properties properties, string propertyName)
		{
			if (properties != null)
			{
				foreach (Property item in properties)
				{
					if (item != null && item.Name == propertyName)
					{
						return true;
					}
				}
			}
			return false;
		}

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

		static public List<string> GetProjectIncludeDirectories(IVCProjectWrapper project, string configurationName, string platformName, IPathResolver pathResolver)
		{
			// get additional include directories
			// source: http://www.mztools.com/articles/2014/MZ2014005.aspx

			Logging.Logging.LogInfo("Attempting to retreive Include Directories for project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.GetName()) + "'");

			List<string> includeDirectories = new List<string>();

			IVCConfigurationWrapper vcProjectConfig = project.getConfiguration(configurationName, platformName);

			if (vcProjectConfig != null && vcProjectConfig.isValid())
			{
				IVCCLCompilerToolWrapper compilerTool = vcProjectConfig.GetCompilerTool();

				if (compilerTool != null && compilerTool.isValid())
				{
					foreach (string directory in compilerTool.GetAdditionalIncludeDirectories())
					{
						if (directory.Length <= 0)
						{
							continue;
						}

						string resolvedDirectories = pathResolver.ResolveVsMacroInPath(directory, vcProjectConfig);

						foreach (string resolvedDirectory in resolvedDirectories.Split(';'))
						{
							string dir = pathResolver.GetAsAbsoluteCanonicalPath(resolvedDirectory, project);
							includeDirectories.Add(dir);
						}
					}
				}

				try
				{
					IVCPlatformWrapper platform = vcProjectConfig.GetPlatform();
					if (platform != null && platform.isValid())
					{
						foreach (string directory in platform.GetIncludeDirectories())
						{
							string resolvedDirectories = pathResolver.ResolveVsMacroInPath(directory, vcProjectConfig);
							foreach (string resolvedDirectory in resolvedDirectories.Split(';'))
							{
								includeDirectories.Add(resolvedDirectory);
							}
						}
					}
				}
				catch (Exception e)
				{
					Logging.Logging.LogError("Failed to retreive platform include directories: " + e.Message);
					return new List<string>();
				}
			}
			else
			{
				Logging.Logging.LogWarning("Could not retreive Project Configuration. No include directories could be retreived.");
				return new List<string>();
			}

			includeDirectories = includeDirectories.Distinct().ToList();

			Logging.Logging.LogInfo("Attempting to clean up.");

			for (int i = 0; i < includeDirectories.Count; i++)
			{
				string path = includeDirectories.ElementAt(i).Replace("\\", "/"); // backslashes would cause some string-escaping hassles...

				if (path.Length == 0)
				{
					includeDirectories.RemoveAt(i);
					i--;
				}
				else
				{
					includeDirectories[i] = path;
				}
			}

			Logging.Logging.LogInfo("Found " + includeDirectories.Count.ToString() + " distinct include directories.");

			return includeDirectories;
		}

		static public List<string> GetProjectPreprocessorDefinitions(IVCProjectWrapper project, string configurationName, string platformName)
		{
			Logging.Logging.LogInfo("Attempting to retreive Preprocessor Definitions for project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.GetName()) + "'");

			List<string> preprocessorDefinitions = new List<string>();

			IVCCLCompilerToolWrapper compilerTool = null;
			IVCConfigurationWrapper vcProjectConfig = project.getConfiguration(configurationName, platformName);
			if (vcProjectConfig != null && vcProjectConfig.isValid())
			{
				compilerTool = vcProjectConfig.GetCompilerTool();
			}

			if (compilerTool != null && compilerTool.isValid())
			{
				foreach (string preprocessorDefinition in compilerTool.GetPreprocessorDefinitions())
				{
					preprocessorDefinitions.Add(preprocessorDefinition.Replace("\\\"", "\""));
				}
			}
			else
			{
				Logging.Logging.LogWarning("Could not retreive compiler tool. No preprocessor definitions could be retreived.");
				return new List<string>();
			}

			preprocessorDefinitions = preprocessorDefinitions.Distinct().ToList();

			Logging.Logging.LogInfo("Found " + preprocessorDefinitions.Count.ToString() + " distinct preprocessor definitions.");

			return preprocessorDefinitions;
		}

		// AFAIK there is no way to programmatically get the c++ standard version supported by any given VS version
		// instead I'm refearing to the VS docu for that information
		// https://msdn.microsoft.com/en-us/library/hh567368.aspx
		static public string GetCppStandardForProject(IVCProjectWrapper project, string configurationName, string platformName)
		{
			string result = "";

			string toolset = project.GetWrappedVersion();
			string justNumbers = new String(toolset.Where(Char.IsDigit).ToArray());
			int versionNumber = int.Parse(justNumbers);

			if (versionNumber < 120) // version 11 (2012)
			{
				result = "-std=c++11";
			}
			else if (versionNumber < 130) // version 12 (2013)
			{
				result = "-std=c++14";
			}
			else if (versionNumber < 150) // version 14 (2015)
			{
				result = "-std=c++14";
			}
			else if (versionNumber < 160) // version 15 (2017)
			{
				result = "-std=c++14";
			}

			return result;
		}


		// returns a valid Guid if the project was reloaded or an empty Guid if the project did not need to be reloaded
		static public Guid ReloadProject(Project project)
		{
			Logging.Logging.LogInfo("Attempting to reload project");

			try
			{
				if (project != null && project.Kind == EnvDTE.Constants.vsProjectKindUnmodeled)
				{
					DTE dte = project.DTE;

					ServiceProvider sp = new ServiceProvider(dte as Microsoft.VisualStudio.OLE.Interop.IServiceProvider);
					IVsSolution vsSolution = sp.GetService(typeof(IVsSolution)) as IVsSolution;

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
