using Microsoft.VisualStudio.VCProjectEngine;
using System;
using System.Collections.Generic;

namespace VCProjectEngineWrapper
{
	public class
#if (VS2012)
	VCFileWrapperVs2012
#elif (VS2013)
	VCFileWrapperVs2013
#elif (VS2015)
	VCFileWrapperVs2015
#elif (VS2017)
	VCFileWrapperVs2017
#endif
		: IVCFileWrapper
	{
		private VCFile _wrapped = null;

		public 
#if (VS2012)
			VCFileWrapperVs2012
#elif (VS2013)
			VCFileWrapperVs2013
#elif (VS2015)
			VCFileWrapperVs2015
#elif (VS2017)
			VCFileWrapperVs2017
#endif
			(object wrapped)
		{
			_wrapped = wrapped as VCFile;
		}

		public bool isValid()
		{
			return (_wrapped != null);
		}

		public string GetWrappedVersion()
		{
			return Utility.GetWrappedVersion();
		}

		public string GetSubType()
		{
			return _wrapped.SubType;
		}

		public IVCProjectWrapper GetProject()
		{
			return new 
#if (VS2012)
			VCProjectWrapperVs2012
#elif (VS2013)
			VCProjectWrapperVs2013
#elif (VS2015)
			VCProjectWrapperVs2015
#elif (VS2017)
			VCProjectWrapperVs2017
#endif
			(_wrapped.project);
		}

		public List<IVCFileConfigurationWrapper> GetFileConfigurations()
		{
			List<IVCFileConfigurationWrapper> fileConfigurations = new List<IVCFileConfigurationWrapper>();
			foreach (Object configuration in _wrapped.FileConfigurations)
			{
				IVCFileConfigurationWrapper vcFileConfig = new
#if (VS2012)
					VCFileConfigurationWrapperVs2012
#elif (VS2013)
					VCFileConfigurationWrapperVs2013
#elif (VS2015)
					VCFileConfigurationWrapperVs2015
#elif (VS2017)
					VCFileConfigurationWrapperVs2017
#endif
					(configuration);
				if (vcFileConfig.isValid())
				{
					fileConfigurations.Add(vcFileConfig);
				}
			}
			return fileConfigurations;
		}
	}
}
