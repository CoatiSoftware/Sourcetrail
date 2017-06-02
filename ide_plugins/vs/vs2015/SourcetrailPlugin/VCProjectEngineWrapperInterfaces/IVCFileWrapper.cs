using System.Collections.Generic;

namespace VCProjectEngineWrapper
{
	public interface IVCFileWrapper
	{
		string GetWrappedVersion();
		bool isValid();

		string GetSubType();
		IVCProjectWrapper GetProject();
		List<IVCFileConfigurationWrapper> GetFileConfigurations();
	}
}
