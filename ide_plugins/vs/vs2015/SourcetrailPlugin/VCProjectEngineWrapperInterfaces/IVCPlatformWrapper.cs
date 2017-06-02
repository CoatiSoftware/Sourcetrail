namespace VCProjectEngineWrapper
{
    public interface IVCPlatformWrapper
    {
		string GetWrappedVersion();
		bool isValid();

		string GetExecutableDirectories();
		string[] GetIncludeDirectories();
	}
}
