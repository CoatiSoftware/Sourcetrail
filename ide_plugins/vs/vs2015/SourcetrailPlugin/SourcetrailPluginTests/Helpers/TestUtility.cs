using EnvDTE;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using System;

namespace CoatiSoftware.SourcetrailPlugin.IntegrationTests.Helpers
{
	public static class TestUtility
	{
		public static void OpenSolution(string solutionFilePath)
		{
			IVsSolution solutionService = (IVsSolution)VsIdeTestHostContext.ServiceProvider.GetService(typeof(IVsSolution));
			int ret = solutionService.OpenSolutionFile((uint)__VSSLNOPENOPTIONS.SLNOPENOPT_DontConvertSLN, solutionFilePath);

			DTE dte = (DTE)VsIdeTestHostContext.ServiceProvider.GetService(typeof(DTE));
			Console.WriteLine("opened solution contains " + dte.Solution.Projects.Count.ToString() + " projects");

			Assert.AreEqual(VSConstants.S_OK, ret);
		}

		public static void CloseCurrentSolution()
		{
			IVsSolution solutionService = (IVsSolution)VsIdeTestHostContext.ServiceProvider.GetService(typeof(IVsSolution));
			int ret = solutionService.CloseSolutionElement((uint)__VSSLNSAVEOPTIONS.SLNSAVEOPT_NoSave, null, 0);
			Assert.AreEqual(VSConstants.S_OK, ret);
		}
	}
}
