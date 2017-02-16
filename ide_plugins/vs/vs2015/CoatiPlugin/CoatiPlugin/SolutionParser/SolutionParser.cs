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

        static public List<string> _headerDirectories = new List<string>();

        static private List<string> _extensionWhiteList = new List<string>() { "c", "cc", "cpp", "cxx", "C", "h", "hpp" };

        public static List<CommandObject> CreateCommandObjects(Project project, string configurationName, string platformName, string cStandard)
        {
            Logging.Logging.LogInfo("Creating command objects from project " + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name));

            List<CommandObject> result = new List<CommandObject>();

            DTE dte = project.DTE;
            Guid projectGuid = Utility.ProjectUtility.ReloadProject(project);

            VCProject vcProject = project.Object as VCProject;

            if (vcProject == null)
            {
                Logging.Logging.LogWarning("Project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "' could not be converted to VCProject, skipping.");
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

            Logging.Logging.LogInfo("Found C++ standard " + cppStandard + ".");

            foreach (EnvDTE.ProjectItem item in projectItems)
            {
                CommandObject cmdObj = CreateCommandObject(item, includeDirectories, preprocessorDefinitions, cppStandard, cStandard, configurationName, platformName);
                if (cmdObj != null)
                {
                    result.Add(cmdObj);
                }
            }

            if (projectGuid != Guid.Empty)
            {
                Utility.ProjectUtility.UnloadProject(projectGuid, dte);
            }

            _headerDirectories = _headerDirectories.Distinct().ToList();

            return result;
        }

        static private Tuple<List<string>, List<string>> GetProjectIncludeDirectoriesAndPreprocessorDefs(VCProject project, string configurationName, string platformName)
        {
            Logging.Logging.LogInfo("Attempting to retreive Include Directories and Preprocessor Definitions for project '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(project.Name) + "'");

            List<string> includeDirectories = new List<string>();
            List<string> preprocessorDefinitions = new List<string>();

            VCConfiguration vcProjectConfig = null;
            try
            {
                vcProjectConfig = GetProjectConfiguration(project, configurationName, platformName);
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Failed to retreive project configuration: " + e.Message);
            }

            if (vcProjectConfig != null)
            {
                try
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

                                string dir = directory;

                                // In case of a project-relative path
                                if (directory.Length > 0 && directory.Substring(0, 1) == ".")
                                {
                                    dir = project.ProjectDirectory + directory;
                                }

                                // make it canonical
                                dir = new Uri(dir).LocalPath;

                                includeDirectories.Add(dir);
                            }

                            break; // TODO: find some documentation on why the break is needed
                                   // Apparently only the first 'tool' is needed, but why?
                        }
                    }
                }
                catch(Exception e)
                {
                    Logging.Logging.LogError("Failed to retreive include directories: " + e.Message);
                    return new Tuple<List<string>, List<string>>(new List<string>(), new List<string>());
                }
                
                try
                {
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
                catch (Exception e)
                {
                    Logging.Logging.LogError("Exception: " + e.Message);
                    return new Tuple<List<string>, List<string>>(new List<string>(), new List<string>());
                }
            }
            else
            {
                Logging.Logging.LogWarning("Could not retreive Project Configuration. No include directories or preprocessor definitions could be retreived.");
                return new Tuple<List<string>, List<string>>(new List<string>(), new List<string>());
            }

            includeDirectories = includeDirectories.Distinct().ToList();

            preprocessorDefinitions = preprocessorDefinitions.Distinct().ToList();

            Logging.Logging.LogInfo("Found " + includeDirectories.Count.ToString() + " distinct include directories and " + preprocessorDefinitions.Count.ToString() + " distinct preprocessor definitions.");
            Logging.Logging.LogInfo("Attempting to resolve and clean up.");

            for (int i = 0; i < includeDirectories.Count; i++)
            {
                string path = includeDirectories.ElementAt(i).Replace("\\", "/"); // backslashes would cause some string-escaping hassles...

                bool exists = true;
                if (System.IO.Directory.Exists(path) == false)
                {
                    exists = false;
                }

                // could be a relative path...
                if (exists == false)
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

                if (exists == false)
                {
                    includeDirectories.RemoveAt(i);
                    i--;
                }
                else
                {
                    includeDirectories[i] = path;
                }
            }

            Logging.Logging.LogInfo("Found " + includeDirectories.Count.ToString() + " include directories and " + preprocessorDefinitions.Count.ToString() + " preprocessor definitions");

            return new Tuple<List<string>, List<string>>(includeDirectories, preprocessorDefinitions);
        }

        static private VCConfiguration GetProjectConfiguration(VCProject project, string configurationName, string platformName)
        {
            Logging.Logging.LogInfo("Attempting to retreive project configuration");

            if (project == null)
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

            Logging.Logging.LogError("Failed to find project config matching with \"" + configurationName + "\"");

            return null;
        }

        static private CommandObject CreateCommandObject(EnvDTE.ProjectItem item, List<string> includeDirectories, List<string> preprocessorDefinitions, string vcStandard, string cStandard, string configurationName, string platformName)
        {
            string objectName = item.Name;

            Logging.Logging.LogInfo("Starting to create Command Object from item '" + Logging.Obfuscation.NameObfuscator.GetObfuscatedName(item.Name) + "'");

            try
            {
                DTE dte = item.DTE;

                if (dte == null)
                {
                    Logging.Logging.LogError("Failed to retreive DTE object. Abort creating command object.");
                }



                VCFile vcFile = item.Object as VCFile;
                string subType = vcFile.SubType;

                VCProject project = vcFile.project;
                VCConfiguration vcConfig = GetProjectConfiguration(project, configurationName, platformName);

                if (vcConfig == null)
                {
                    Logging.Logging.LogError("Project Configuration is null.");

                    return null;
                }

                VCFileConfiguration fc = vcFile.GetFileConfigurationForProjectConfiguration(vcConfig);
                VCCLCompilerTool t = fc.Tool as VCCLCompilerTool;

                if (t == null)
                {
                    Logging.Logging.LogInfo("Unable to retrieve build tool. Using extension white list to determine file type.");
                }

                if (IsSourceFile(item, t))
                {
                    CommandObject commandObject = new CommandObject();
                    commandObject.File = item.Name;

                    // only write source files to cdb, headers are implicit
                    // however, retreive header directory
                    if (CheckIsHeader(item))
                    {
                        Properties props = item.Properties;
                        foreach (Property prop in props)
                        {
                            string propName = prop.Name;
                            string propValue = prop.Value as String;

                            if (propName == "FullPath")
                            {
                                int i = propValue.LastIndexOf('\\');

                                propValue = propValue.Substring(0, i);

                                _headerDirectories.Add(propValue);
                            }
                        }

                        return null;
                    }

                    string additionalOptions = "";

                    if (t != null)
                    {
                        additionalOptions = t.AdditionalOptions;
                        CompileAsOptions compileAs = t.CompileAs; // VCCLCompilerToolShim

                        if (additionalOptions == "$(NOINHERIT)")
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
                    }

                    // if a language standard was defined in the additional options the 'vcStandard' string is not used
                    if (additionalOptions.Contains("-std="))
                    {
                        vcStandard = "";
                    }

                    string directory = item.Properties.Item("FullPath").Value.ToString();

                    List<string> names = new List<string>();
                    foreach (Property p in item.Properties)
                    {
                        names.Add(p.Name);
                    }

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

                    foreach (string flag in _additionalCompileFlags)
                    {
                        commandObject.Command += " -D " + flag + " ";
                    }

                    commandObject.Command += vcStandard + " ";

                    commandObject.Command += additionalOptions + " ";

                    commandObject.Command += "'" + commandObject.File + "'";

                    return commandObject;
                }
                else
                {
                    Logging.Logging.LogInfo("Item discarded, wrong code model");
                }
            }
            catch (Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
            }

            Logging.Logging.LogError("Failed to create command object.");

            return null;
        }

        static private bool IsSourceFile(ProjectItem item, VCCLCompilerTool tool)
        {
            if (tool != null) // if the tool is null it's probably not a normal VC project, indicating that the file code model is unavailable
            {
                if (item.FileCodeModel != null && item.FileCodeModel.Language == CodeModelLanguageConstants.vsCMLanguageVC)
                {
                    return true;
                }
            }
            else if (_extensionWhiteList.Contains(GetFileExtension(item)))
            {
                return true;
            }

            return false;
        }

        static private bool CheckIsHeader(EnvDTE.ProjectItem item)
        {
            Properties props = item.Properties;

            string propString = "";

            try
            {
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
            }
            catch (Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
            }

            return false;
        }

        static private string ResolveVSMacro(VCConfiguration vcProjectConfig, string path)
        {
            string result = path;

            try
            {
                Tuple<int, int> potentialMacroPosition = Utility.StringUtility.FindFirstRange(path, "$(", ")");

                if (potentialMacroPosition != null)
                {
                    string potentialMacro = path.Substring(potentialMacroPosition.Item1, potentialMacroPosition.Item2 - potentialMacroPosition.Item1 + 1);

                    string resolvedMacro = vcProjectConfig.Evaluate(potentialMacro);

                    result = path.Substring(0, potentialMacroPosition.Item1) + resolvedMacro + path.Substring(potentialMacroPosition.Item2 + 1);
                }
            }
            catch (Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
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

            List<EnvDTE.Project> projects = Utility.SolutionUtility.GetSolutionProjectList(dte);

            foreach (EnvDTE.Project project in projects)
            {
                _reloadedProjectGuids.Add(Utility.ProjectUtility.ReloadProject(project));
            }
        }

        static private void UnloadReloadedProjects(DTE dte)
        {
            foreach (Guid guid in _reloadedProjectGuids)
            {
                Utility.ProjectUtility.UnloadProject(guid, dte);
            }
        }

        static private void SetCompatibilityVersionFlag(VCProject project, string configurationName, string platformName)
        {
            Logging.Logging.LogInfo("Determining CL.exe (C++ compiler) version");

            VCConfiguration vcProjectConfig = GetProjectConfiguration(project, configurationName, platformName);

            if (vcProjectConfig != null)
            {
                IEnumerable projectTools = vcProjectConfig.Tools as IEnumerable;
                foreach (Object tool in projectTools)
                {
                    try
                    {
                        VCCLCompilerTool compilerTool = tool as VCCLCompilerTool;

                        if (compilerTool != null)
                        {
                            int majorCompilerVersion = GetCLMajorVersion(compilerTool, vcProjectConfig);

                            if (majorCompilerVersion > -1)
                            {
                                Logging.Logging.LogInfo("Found compiler version " + majorCompilerVersion.ToString());

                                _compatibilityVersionFlag = _compatibilityVersionFlagBase + majorCompilerVersion.ToString();
                                return;
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        Logging.Logging.LogError("Exception: " + e.Message);
                    }
                }
            }
            else
            {
                Logging.Logging.LogWarning("Failed to retreive VC Project Configuration. Using default compiler version");
            }
        }

        static private int GetCLMajorVersion(VCCLCompilerTool compilerTool, VCConfiguration vcProjectConfig)
        {
            Logging.Logging.LogInfo("Looking up CL.exe (C++ compiler)");

            if (compilerTool == null || vcProjectConfig == null)
            {
                return -1;
            }

            try
            {
                VCPlatform platform = vcProjectConfig.Platform as VCPlatform;
                string executableDirectories = platform.ExecutableDirectories;
                string[] seperatedDirectories = executableDirectories.Split(';');

                List<string> finalDirectories = new List<string>();
                foreach (string directory in seperatedDirectories)
                {
                    string resolvedDirectory = ResolveVSMacro(vcProjectConfig, directory);
                    string[] splitResolvedDirectory = resolvedDirectory.Split(';'); // resolved macros might result in concatenated paths

                    foreach (string d in splitResolvedDirectory)
                    {
                        finalDirectories.Add(d);
                    }
                }

                string toolPath = compilerTool.ToolPath;

                Logging.Logging.LogInfo("Found " + finalDirectories.Count.ToString() + " possible compiler directories.");

                foreach (string fd in finalDirectories)
                {
                    string path = fd + "\\" + toolPath;

                    if (File.Exists(path))
                    {
                        FileVersionInfo info = FileVersionInfo.GetVersionInfo(path);
                        int version = info.FileMajorPart;

                        Logging.Logging.LogInfo("Found compiler location. Compiler tool version is " + version.ToString());

                        return version;
                    }
                }
            }
            catch (Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
            }

            Logging.Logging.LogWarning("Failed to find C++ compiler tool.");

            return -1;
        }

        // AFAIK there is no way to programmatically get the c++ standard version supported by any given VS version
        // instead I'm refearing to the VS docu for that information
        // https://msdn.microsoft.com/en-us/library/hh567368.aspx
        static private string GetCppStandardString(VCConfiguration vcProjectConfig)
        {
            if (vcProjectConfig == null)
            {
                return "";
            }

            string result = "";

            IVCRulePropertyStorage rules = null;

            try
            {
                rules = vcProjectConfig.Rules.Item("ConfigurationGeneral");
            }
            catch (Exception e)
            {
                Logging.Logging.LogError("Exception: " + e.Message);
                return "";
            }

            if (rules != null)
            {
                try
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
                catch (Exception e)
                {
                    Logging.Logging.LogError("Exception: " + e.Message);
                }
            }

            return result;
        }

        static private string GetFileExtension(ProjectItem item)
        {
            if (item == null)
            {
                return "";
            }

            string name = item.Name;

            int idx = name.LastIndexOf(".");

            if (idx > -1 && idx < name.Length - 1)
            {
                return name.Substring(idx + 1);
            }
            else
            {
                return "";
            }
        }
    }
}
