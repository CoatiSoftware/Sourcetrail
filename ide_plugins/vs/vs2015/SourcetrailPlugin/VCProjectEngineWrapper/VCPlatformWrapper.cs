using Microsoft.VisualStudio.VCProjectEngine;

namespace VCProjectEngineWrapper
{
	public class
#if (VS2012)
		VCPlatformWrapperVs2012
#elif (VS2013)
		VCPlatformWrapperVs2013
#elif (VS2015)
		VCPlatformWrapperVs2015
#elif (VS2017)
		VCPlatformWrapperVs2017
#endif
		: IVCPlatformWrapper
	{
		private VCPlatform _wrapped = null;

		public
#if (VS2012)
			VCPlatformWrapperVs2012
#elif (VS2013)
			VCPlatformWrapperVs2013
#elif (VS2015)
			VCPlatformWrapperVs2015
#elif (VS2017)
			VCPlatformWrapperVs2017
#endif
			(object wrapped)
		{
			_wrapped = wrapped as VCPlatform;
		}

		public bool isValid()
		{
			return (_wrapped != null);
		}

		public string GetWrappedVersion()
		{
			return Utility.GetWrappedVersion();
		}

		public string GetExecutableDirectories()
		{
			return _wrapped.ExecutableDirectories;
		}

		public string[] GetIncludeDirectories()
		{
			return _wrapped.IncludeDirectories.Split(';');
		}

	}
}
