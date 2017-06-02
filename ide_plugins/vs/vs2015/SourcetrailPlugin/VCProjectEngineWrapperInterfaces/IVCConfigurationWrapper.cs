namespace VCProjectEngineWrapper
{
    public interface IVCConfigurationWrapper
    {
		string GetWrappedVersion();
		bool isValid();

		string EvaluateMacro(string macro);
		IVCCLCompilerToolWrapper GetCompilerTool();
		IVCPlatformWrapper GetPlatform();
    }
}
