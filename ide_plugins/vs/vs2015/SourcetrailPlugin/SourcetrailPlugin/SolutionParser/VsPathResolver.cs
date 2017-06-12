using CoatiSoftware.SourcetrailPlugin.Utility;
using System;
using VCProjectEngineWrapper;

namespace CoatiSoftware.SourcetrailPlugin.SolutionParser
{
	public class VsPathResolver : IPathResolver
	{
		private string _compilationDatabaseFilePath = "";

		public VsPathResolver(string compilationDatabaseFilePath)
		{
			_compilationDatabaseFilePath = compilationDatabaseFilePath.Replace('\\', '/');
		}

		public override string GetCompilationDatabaseFilePath()
		{
			return _compilationDatabaseFilePath;
		}

		protected override string DoGetAsAbsoluteCanonicalPath(string path, IVCProjectWrapper project)
		{
			string absolutePath = project.GetProjectDirectory() + path;
			return new Uri(absolutePath).LocalPath;
		}

		protected override string ResolveVsMacro(string potentialMacro, IVCConfigurationWrapper vcProjectConfig)
		{
			return vcProjectConfig.EvaluateMacro(potentialMacro);
		}
	}
}
