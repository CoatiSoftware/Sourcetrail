namespace VCProjectEngineWrapper
{
	public interface IVCFileConfigurationWrapper
	{
		string GetWrappedVersion();
		bool isValid();

		IVCCLCompilerToolWrapper GetTool();
	}
}
