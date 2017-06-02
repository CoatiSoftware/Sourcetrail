namespace VCProjectEngineWrapper
{
    public interface IVCProjectWrapper
    {
		string GetWrappedVersion();
		bool isValid();

		IVCConfigurationWrapper getConfiguration(string configurationName, string platformName);
		string GetProjectDirectory();
		string GetName();
	}
}
