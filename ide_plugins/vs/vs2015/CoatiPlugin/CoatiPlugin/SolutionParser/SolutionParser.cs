using System;
using System.Collections.Generic;
using System.Linq;
using EnvDTE;

using Microsoft.VisualStudio.VCProjectEngine;
using System.Collections;
using System.IO;
using System.Diagnostics;

namespace CoatiSoftware.CoatiPlugin.SolutionParser
{
    class SolutionParser
    {
        static private List<Guid> _reloadedProjectGuids = new List<Guid>();
        static private List<string> _compatibilityFlags = new List<string>() { "-fms-extensions", "-fms-compatibility" };
        static private string _compatibilityVersionFlagBase = "-fms-compatibility-version="; // I want to get the exact version at runtime for this flag, therefore I keep it seperate from the others to make things a bit easier...
        static private string _compatibilityVersionFlag = _compatibilityVersionFlagBase + "19"; // This default version would correspond to VS2015

        static public List<string> _additionalCompileFlags = new List<string>();

        // Creates a cdb from all projects in the solution
        // For a more selective approach use 'CreateCommandObjects(...)' for per-project compile commands and assamble the cdb yourself
        static public CompilationDatabase CreateCompilationDatabase(DTE dte, string configurationName, string platformName)
        {
            if(dte == null)
            {
                return null;
            }

            ReloadAll(dte);

            CompilationDatabase compilationDatabase = new CompilationDatabase();

            EnvDTE.Solution solution = dte.Solution;

            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                List<CommandObject> cmdObjts = CreateCommandObjects(project, configurationName, platformName, "c11"); // TODO: retrieve real config/platform

                foreach(CommandObject cmdObj in cmdObjts)
                {
                    compilationDatabase.AddCommandObject(cmdObj);
                }
            }

            UnloadReloadedProjects(dte);

            return compilationDatabase;
        }

        public static List<CommandObject> CreateCommandObjects(Project project, string configurationName, string platformName, string cStandard)
        {
            List<CommandObject> result = new List<CommandObject>();

            DTE dte = project.DTE;
            Guid projectGuid = Utility.SolutionUtility.ReloadProject(project);

            string version = dte.Version;

            VCProject vcProject = project.Object as VCProject;

            if (vcProject == null)
            {
                return result;
            }

            SetCompatibilityVersionFlag(vcProject, configurationName, platformName);

            // gather include paths and preprocessor definitions of the project
            List<string> includeDirectories = new List<string>();
            List<string> preprocessorDefinitions = new List<string>();

            Tuple<List<string>, List<string>> pathsAndFlags = GetProjectIncludeDirectoriesAndPreprocessorDefs(vcProject, configurationName, platformName);

            includeDirectories = pathsAndFlags.Item1;
            preprocessorDefinitions = pathsAndFlags.Item2;

            // create command objects for all applicable project items
            List<ProjectItem> projectItems = Utility.ProjectUtility.GetProjectItems(project);

            VCConfiguration vcProjectConfig = GetProjectConfiguration(vcProject, configurationName, platformName);
            string cppStandard = GetCppStandardString(vcProjectConfig);

            foreach (EnvDTE.ProjectItem item in projectItems)
            {
                CommandObject cmdObj = CreateCommandObject(item, includeDirectories, preprocessorDefinitions, cppStandard, cStandard);
                if (cmdObj != null)
                {
                    result.Add(cmdObj);
                }
            }

            if(projectGuid != Guid.Empty)
            {
                Utility.SolutionUtility.UnloadProject(projectGuid, dte);
            }

            return result;
        }

        static private Tuple<List<string>, List<string>> GetProjectIncludeDirectoriesAndPreprocessorDefs(VCProject project, string configurationName, string platformName)
        {
            List<string> includeDirectories = new List<string>();
            List<string> preprocessorDefinitions = new List<string>();

            IEnumerable configurations = project.Configurations as IEnumerable;

            VCConfiguration vcProjectConfig = GetProjectConfiguration(project, configurationName, platformName);

            if (vcProjectConfig != null)
            {
                // get additional include directories
                // source: http://www.mztools.com/articles/2014/MZ2014005.aspx
                IEnumerable projectTools = vcProjectConfig.Tools as IEnumerable;
                foreach (Object tool in projectTools)
                {
                    VCCLCompilerTool compilerTool = tool as VCCLCompilerTool;

                    if (compilerTool != null)
                    {
                        string additionalIncludeDirs = compilerTool.FullIncludePath;
                        string preprocessorDefinition = compilerTool.PreprocessorDefinitions;

                        string[] prepDefs = preprocessorDefinition.Split(';');

                        foreach (string prepDef in prepDefs)
                        {
                            preprocessorDefinitions.Add(prepDef);
                        }

                        string[] directories = additionalIncludeDirs.Split(';');

                        foreach (string directory in directories)
                        {
                            if (directory.Length <= 0)
                            {
                                continue;
                            }

                            includeDirectories.Add(directory);
                        }

                        break; // TODO: find some documentation on why the break is needed
                               // Apparently only the first 'tool' is needed, but why?
                    }
                }

                VCPlatform platform = vcProjectConfig.Platform as VCPlatform;
                string platformIncludeDirectories = platform.IncludeDirectories;
                string[] seperatedDirectories = platformIncludeDirectories.Split(';');

                foreach (string directory in seperatedDirectories)
                {
                    string resolvedDirectory = ResolveVSMacro(vcProjectConfig, directory);
                    string[] splitResolvedDirectory = resolvedDirectory.Split(';'); // resolved macros might result in concatenated paths

                    foreach (string p in splitResolvedDirectory)
                    {
                        includeDirectories.Add(p);
                    }
                }
            }

            includeDirectories = includeDirectories.Distinct().ToList();

            preprocessorDefinitions = preprocessorDefinitions.Distinct().ToList();

            for (int i = 0; i < includeDirectories.Count; i++)
            {
                string path = includeDirectories.ElementAt(i).Replace("\\", "/"); // backslashes would cause some string-escaping hassles...

                bool exists = true;
                if (System.IO.Directory.Exists(path) == false)
                {
                    exists = false;
                }

                // could be a relative path...
                if(exists == false)
                {
                    if (System.IO.Directory.Exists(project.ProjectDirectory + path) == false)
                    {
                        exists = false;
                    }
                    else
                    {
                        path = project.ProjectDirectory + path;
                        path = path.Replace("\\", "/");
                        exists = true;
                    }
                }

                if(exists == false)
                {
                    includeDirectories.RemoveAt(i);
                    i--;
                }
                else
                {
                    includeDirectories[i] = path;
                }
            }

            return new Tuple<List<string>, List<string>>(includeDirectories, preprocessorDefinitions);
        }

        static private VCConfiguration GetProjectConfiguration(VCProject project, string configurationName, string platformName)
        {
            if(project == null)
            {
                return null;
            }

            IEnumerable configurations = project.Configurations as IEnumerable;
            foreach (Object configuration in configurations)
            {
                VCConfiguration vcProjectConfig = configuration as VCConfiguration;

                if (vcProjectConfig != null &&
                    vcProjectConfig.ConfigurationName == configurationName &&
                    vcProjectConfig.Platform.Name == platformName)
                {
                    return vcProjectConfig;
                }
            }

            return null;
        }

        static private CommandObject CreateCommandObject(EnvDTE.ProjectItem item, List<string> includeDirectories, List<string> preprocessorDefinitions, string vcStandard, string cStandard)
        {
            DTE dte = item.DTE;

            if (item.FileCodeModel != null && item.FileCodeModel.Language == CodeModelLanguageConstants.vsCMLanguageVC)
            {
                CommandObject commandObject = new CommandObject();
                commandObject.File = item.Name;

                // only write source files to cdb, headers are implicit
                if (CheckIsHeader(item))
                {
                    return null;
                }

                VCFile vcFile = item.Object as VCFile;
                string subType = vcFile.SubType;

                VCProject project = vcFile.project;
                VCConfiguration vcConfig = GetProjectConfiguration(project, "Release", "Win32");

                VCFileConfiguration fc = vcFile.GetFileConfigurationForProjectConfiguration(vcConfig);
                VCCLCompilerTool t = fc.Tool as VCCLCompilerTool;

                string additionalOptions = t.AdditionalOptions;
                CompileAsOptions compileAs = t.CompileAs; // VCCLCompilerToolShim

                if(additionalOptions == "$(NOINHERIT)")
                {
                    additionalOptions = "";
                }

                // check wheter it's a .c file, we don't want that...
                // TODO: there is a property for comilation as .c or .cpp file (/TC and /TP), try to retrieve it
                string extension = item.Properties.Item("Extension").Value.ToString();
                if (compileAs == CompileAsOptions.compileAsC)
                {
                    vcStandard = "-std=" + cStandard;
                }

                // if a language standard was defined in the additional options the 'vcStandard' string is not used
                if(additionalOptions.Contains("-std="))
                {
                    vcStandard = "";
                }

                string directory = item.Properties.Item("FullPath").Value.ToString();

                int idx = directory.LastIndexOf('\\');
                if (idx != -1)
                {
                    directory = directory.Substring(0, idx + 1);
                }

                directory = directory.Replace('\\', '/');
                commandObject.File = directory + item.Name;

                commandObject.Directory = System.IO.Path.GetDirectoryName(dte.Solution.FullName); // TODO: replace with actual cdb location
                commandObject.Directory = commandObject.Directory.Replace('\\', '/');

                commandObject.Command = "clang-tool ";

                foreach (string flag in _compatibilityFlags)
                {
                    commandObject.Command += flag + " ";
                }

                commandObject.Command += _compatibilityVersionFlag + " ";

                foreach (string dir in includeDirectories)
                {
                    commandObject.Command += " -isystem '" + dir + "' "; // using '-isystem' because it allows for use of quotes and pointy brackets in source files. In other words it's more robust. It's slower than '-I' though
                }

                foreach (string prepDef in preprocessorDefinitions)
                {
                    commandObject.Command += " -D " + prepDef + " ";
                }

                foreach(string flag in _additionalCompileFlags)
                {
                    commandObject.Command += " -D " + flag + " ";
                }

                commandObject.Command += vcStandard + " ";

                commandObject.Command += additionalOptions + " ";

                commandObject.Command += "'" + commandObject.File + "'";

                return commandObject;
            }

            return null;
        }

        static private bool CheckIsHeader(EnvDTE.ProjectItem item)
        {
            Properties props = item.Properties;

            string propString = "";

            foreach (Property prop in props)
            {
                string propName = prop.Name;
                string propValue = prop.Value as String;

                propString += propName + " - " + propValue + "; ";

                if (propName == "ItemType")
                {
                    if (propValue as String == "ClInclude")
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        static private string ResolveVSMacro(VCConfiguration vcProjectConfig, string path)
        {
            string result = path;

            Tuple<int, int> potentialMacroPosition = Utility.StringUtility.FindFirstRange(path, "$(", ")");

            if (potentialMacroPosition != null)
            {
                string potentialMacro = path.Substring(potentialMacroPosition.Item1, potentialMacroPosition.Item2 - potentialMacroPosition.Item1 + 1);

                string resolvedMacro = vcProjectConfig.Evaluate(potentialMacro);

                result = path.Substring(0, potentialMacroPosition.Item1) + resolvedMacro + path.Substring(potentialMacroPosition.Item2 + 1);
            }

            return result;
        }

        static private void ReloadAll(DTE dte)
        {
            if (dte == null)
            {
                return;
            }

            EnvDTE.Solution solution = dte.Solution;

            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                _reloadedProjectGuids.Add(Utility.SolutionUtility.ReloadProject(project));
            }
        }

        static private void UnloadReloadedProjects(DTE dte)
        {
            foreach(Guid guid in _reloadedProjectGuids)
            {
                Utility.SolutionUtility.UnloadProject(guid, dte);
            }
        }

        static private void SetCompatibilityVersionFlag(VCProject project, string configurationName, string platformName)
        {
            VCConfiguration vcProjectConfig = GetProjectConfiguration(project, configurationName, platformName);

            if (vcProjectConfig != null)
            {
                IEnumerable projectTools = vcProjectConfig.Tools as IEnumerable;
                foreach (Object tool in projectTools)
                {
                    VCCLCompilerTool compilerTool = tool as VCCLCompilerTool;

                    if (compilerTool != null)
                    {
                        int majorCompilerVersion = GetCLMajorVersion(compilerTool, vcProjectConfig);

                        if (majorCompilerVersion > -1)
                        {
                            _compatibilityVersionFlag = _compatibilityVersionFlagBase + majorCompilerVersion.ToString();
                            return;
                        }
                    }
                }
            }
        }

        static private int GetCLMajorVersion(VCCLCompilerTool compilerTool, VCConfiguration vcProjectConfig)
        {
            if(compilerTool == null || vcProjectConfig == null)
            {
                return -1;
            }

            VCPlatform platform = vcProjectConfig.Platform as VCPlatform;
            string executableDirectories = platform.ExecutableDirectories;
            string[] seperatedDirectories = executableDirectories.Split(';');

            List<string> finalDirectories = new List<string>();
            foreach (string directory in seperatedDirectories)
            {
                string resolvedDirectory = ResolveVSMacro(vcProjectConfig, directory);
                string[] splitResolvedDirectory = resolvedDirectory.Split(';'); // resolved macros might result in concatenated paths

                foreach(string d in splitResolvedDirectory)
                {
                    finalDirectories.Add(d);
                }
            }

            string toolPath = compilerTool.ToolPath;

            foreach(string fd in finalDirectories)
            {
                string path = fd + "\\" + toolPath;

                if(File.Exists(path))
                {
                    FileVersionInfo info = FileVersionInfo.GetVersionInfo(path);
                    int version = info.FileMajorPart;

                    return version;
                }
            }

            return -1;
        }

        // AFAIK there is no way to programmatically get the c++ standard version supported by any given VS version
        // instead I'm refearing to the VS docu for that information
        // https://msdn.microsoft.com/en-us/library/hh567368.aspx
        static private string GetCppStandardString(VCConfiguration vcProjectConfig)
        {
            if(vcProjectConfig == null)
            {
                return "";
            }

            string result = "";

            IVCRulePropertyStorage rules = vcProjectConfig.Rules.Item("ConfigurationGeneral");

            if (rules != null)
            {
                string toolset = rules.GetUnevaluatedPropertyValue("PlatformToolset");

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
            }
            
            return result;
        }
    }
}
