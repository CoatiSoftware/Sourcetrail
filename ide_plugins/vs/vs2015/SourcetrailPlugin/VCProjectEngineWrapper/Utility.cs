using Microsoft.VisualStudio.VCProjectEngine;

namespace VCProjectEngineWrapper
{
	public static class Utility
	{
		public static string GetWrappedVersion()
		{
			return
#if (VS2012)
				"11.0";
#elif (VS2013)
				"12.0";
#elif (VS2015)
				"14.0";
#elif (VS2017)
				"15.0";
#endif
		}
	}
}
