using Microsoft.VisualStudio.VCProjectEngine;

namespace VCProjectEngineWrapper
{
	public class
#if (VS2012)
		VCCLCompilerToolWrapperVs2012
#elif (VS2013)
		VCCLCompilerToolWrapperVs2013
#elif (VS2015)
		VCCLCompilerToolWrapperVs2015
#elif (VS2017)
		VCCLCompilerToolWrapperVs2017
#endif
		: IVCCLCompilerToolWrapper
	{
		private VCCLCompilerTool _wrapped = null;

		public
#if (VS2012)
			VCCLCompilerToolWrapperVs2012
#elif (VS2013)
			VCCLCompilerToolWrapperVs2013
#elif (VS2015)
			VCCLCompilerToolWrapperVs2015
#elif (VS2017)
			VCCLCompilerToolWrapperVs2017
#endif
			(object wrapped)
		{
			_wrapped = wrapped as VCCLCompilerTool;
		}

		public bool isValid()
		{
			return (_wrapped != null);
		}

		public string GetWrappedVersion()
		{
			return Utility.GetWrappedVersion();
		}

		public string GetAdditionalOptions()
		{
			return _wrapped.AdditionalOptions;
		}

		public bool GetCompilesAsC()
		{
			return _wrapped.CompileAs == CompileAsOptions.compileAsC;
		}

		public string GetToolPath()
		{
			return _wrapped.ToolPath;
		}

		public string[] GetAdditionalIncludeDirectories()
		{
			return _wrapped.AdditionalIncludeDirectories.Split(';');
		}

		public string[] GetPreprocessorDefinitions()
		{
			return _wrapped.PreprocessorDefinitions.Split(';');
		}

	}
}
