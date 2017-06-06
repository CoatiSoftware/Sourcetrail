using CoatiSoftware.SourcetrailPlugin.Logging;
using Microsoft.VisualStudio.VCProjectEngine;
using System;
using System.Collections;

namespace VCProjectEngineWrapper
{
	public class
#if (VS2012)
		VCConfigurationWrapperVs2012
#elif (VS2013)
		VCConfigurationWrapperVs2013
#elif (VS2015)
		VCConfigurationWrapperVs2015
#elif (VS2017)
		VCConfigurationWrapperVs2017
#endif
		: IVCConfigurationWrapper
	{
		private VCConfiguration _wrapped = null;

		public
#if (VS2012)
			VCConfigurationWrapperVs2012
#elif (VS2013)
			VCConfigurationWrapperVs2013
#elif (VS2015)
			VCConfigurationWrapperVs2015
#elif (VS2017)
			VCConfigurationWrapperVs2017
#endif
			(object wrapped)
		{
			_wrapped = wrapped as VCConfiguration;
		}

		public bool isValid()
		{
			return (_wrapped != null);
		}

		public string GetWrappedVersion()
		{
			return Utility.GetWrappedVersion();
		}

		public string EvaluateMacro(string macro)
		{
			return _wrapped.Evaluate(macro);
		}

		public IVCCLCompilerToolWrapper GetCompilerTool()
		{
			try
			{
				IEnumerable tools = _wrapped.Tools as IEnumerable;
				foreach (Object tool in tools)
				{
					VCCLCompilerTool compilerTool = tool as VCCLCompilerTool;
					if (compilerTool != null)
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
							(compilerTool);
					}
				}
			}
			catch (Exception e)
			{
				Logging.LogError("Failed to retreive compiler tool: " + e.Message);
			}
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
			(null);
		}

		public IVCPlatformWrapper GetPlatform()
		{
			return new 
#if (VS2012)
			VCPlatformWrapperVs2012
#elif (VS2013)
			VCPlatformWrapperVs2013
#elif (VS2015)
			VCPlatformWrapperVs2015
#elif (VS2017)
			VCPlatformWrapperVs2017
#endif
			(_wrapped.Platform);
		}

	}
}
