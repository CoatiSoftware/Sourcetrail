namespace VCProjectEngineWrapper
{
	public interface IVCCLCompilerToolWrapper
	{
		string GetWrappedVersion();
		bool isValid();

		string GetAdditionalOptions();
		bool GetCompilesAsC();
		string GetToolPath();
		string[] GetAdditionalIncludeDirectories();
		string[] GetPreprocessorDefinitions();
	}
}
