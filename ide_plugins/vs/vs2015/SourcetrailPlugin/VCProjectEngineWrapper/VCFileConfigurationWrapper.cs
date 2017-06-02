using Microsoft.VisualStudio.VCProjectEngine;

namespace VCProjectEngineWrapper
{
	public class
#if (VS2012)
		VCFileConfigurationWrapperVs2012
#elif (VS2013)
		VCFileConfigurationWrapperVs2013
#elif (VS2015)
		VCFileConfigurationWrapperVs2015
#elif (VS2017)
		VCFileConfigurationWrapperVs2017
#endif
		: IVCFileConfigurationWrapper
	{
		private VCFileConfiguration _wrapped = null;

		public
#if (VS2012)
			VCFileConfigurationWrapperVs2012
#elif (VS2013)
			VCFileConfigurationWrapperVs2013
#elif (VS2015)
			VCFileConfigurationWrapperVs2015
#elif (VS2017)
			VCFileConfigurationWrapperVs2017
#endif
			(object wrapped)
		{
			_wrapped = wrapped as VCFileConfiguration;
		}

		public bool isValid()
		{
			return (_wrapped != null);
		}

		public string GetWrappedVersion()
		{
			return Utility.GetWrappedVersion();
		}

		public IVCCLCompilerToolWrapper GetTool()
		{
			return new 
#if (VS2012)
			VCCLCompilerToolWrapperVs2012
#elif (VS2013)
			VCCLCompilerToolWrapperVs2013
#elif (VS2015)
			VCCLCompilerToolWrapperVs2015
#elif (VS2017)
			VCCLCompilerToolWrapperVs2017
#endif
			(_wrapped.Tool);
		}
	}
}
