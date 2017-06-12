using CoatiSoftware.SourcetrailPlugin.Utility;
using VCProjectEngineWrapper;

namespace CoatiSoftware.SourcetrailPlugin.IntegrationTests.Helpers
{
	class TestPathResolver : IPathResolver
	{
		public override string GetCompilationDatabaseFilePath()
		{
			return "<CompilationDatabaseFilePath>";
		}

		protected override string DoGetAsAbsoluteCanonicalPath(string path, IVCProjectWrapper project)
		{
			return "<ProjectBaseDirectory>/" + path;
		}

		protected override string ResolveVsMacro(string potentialMacro, IVCConfigurationWrapper vcProjectConfig)
		{
			return "<Macro " + potentialMacro + ">";
		}
	}
}
