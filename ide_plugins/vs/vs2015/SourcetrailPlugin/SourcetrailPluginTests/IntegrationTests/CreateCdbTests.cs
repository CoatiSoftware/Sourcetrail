using CoatiSoftware.SourcetrailPlugin.IntegrationTests.Helpers;
using CoatiSoftware.SourcetrailPlugin.SolutionParser;
using EnvDTE;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VSSDK.Tools.VsIdeTesting;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Collections.Generic;
using System.IO;

namespace CoatiSoftware.SourcetrailPlugin.IntegrationTests
{
	[TestClass]
	public class CreateCdbTests
	{
		private bool _updateExpectedOutput = false;

		[TestMethod]
		[HostType("VS IDE")]
		public void TestSourcetrailPluginPackageGetsLoaded()
		{
			UIThreadInvoker.Invoke(new Action(() =>
			{
				// Load the package into the shell.
				Assert.IsNotNull(VsIdeTestHostContext.ServiceProvider);
				IVsShell shellService = (IVsShell)VsIdeTestHostContext.ServiceProvider.GetService(typeof(SVsShell));
				Guid packageGuid = new Guid(GuidList.guidSourcetrailPluginPkgString);

				IVsPackage package;
				shellService.IsPackageLoaded(ref packageGuid, out package);
				if (package == null)
				{
					shellService.LoadPackage(ref packageGuid, out package);
				}

				Assert.IsTrue(package is SourcetrailPluginPackage);
			}));
		}

		[TestMethod]
		[HostType("VS IDE")]
		public void TestCompilationDatabaseCreationForCinderSolution()
		{
			UIThreadInvoker.Initialize();
			UIThreadInvoker.Invoke(new Action(() =>
			{
				TestCompilationDatabaseForSolution("../../../SourcetrailPluginTests/bin/data/cinder/cinder.sln");
			}));
		}

		[TestMethod]
		[HostType("VS IDE")]
		public void TestCompilationDatabaseCreationForAllFilesInSameFolder()
		{
			UIThreadInvoker.Initialize();
			UIThreadInvoker.Invoke(new Action(() =>
			{
				TestCompilationDatabaseForSolution("../../../SourcetrailPluginTests/bin/data/all_in_same_folder/test.sln");
			}));
		}

		private void TestCompilationDatabaseForSolution(string solutionPath)
		{
			Console.WriteLine("opening solution: " + solutionPath);
			Helpers.TestUtility.OpenSolution(solutionPath);

			Console.WriteLine("creating compilation database");

			CompilationDatabase output = null;
			try
			{
				output = CreateCompilationDatabaseForCurrentSolution();
			}
			catch (Exception e)
			{
				Console.WriteLine("Exception: " + e.Message);
				Console.WriteLine("Stack Trace: " + e.StackTrace);
				Assert.Fail("Caught and exception while creating compilation database.");
			}

			Assert.IsNotNull(output);

			string cdbPath = Path.ChangeExtension(solutionPath, "json");
			if (_updateExpectedOutput)
			{
				output.SortAlphabetically();
				Console.WriteLine("writing compilation database to file: " + cdbPath);
				File.WriteAllText(cdbPath, output.SerializeToJson());
				Assert.IsTrue(File.Exists(cdbPath));
			}
			else
			{
				Console.WriteLine("reading compilation database from file: " + cdbPath);
				CompilationDatabase expectedOutput = CompilationDatabase.LoadFromFile(cdbPath);
				Assert.IsNotNull(expectedOutput);

				Console.WriteLine("comparing generated compilation database to expected output");
				Assert.IsTrue(output == expectedOutput, "The created compilation database differs from the expected output");
			}

			Console.WriteLine("closing solution");
			Helpers.TestUtility.CloseCurrentSolution();
		}

		private static CompilationDatabase CreateCompilationDatabaseForCurrentSolution()
		{
			DTE dte = (DTE)VsIdeTestHostContext.ServiceProvider.GetService(typeof(DTE));
			Assert.IsNotNull(dte);

			CompilationDatabase cdb = new CompilationDatabase();
			foreach (Project project in dte.Solution.Projects)
			{
				List<string> configurationNames = Utility.SolutionUtility.GetConfigurationNames(dte);
				Assert.IsTrue(configurationNames.Count > 0, "No target configurations found in loaded solution.");

				List<string> platformNames = Utility.SolutionUtility.GetPlatformNames(dte);
				Assert.IsTrue(platformNames.Count > 0, "No target platforms found in loaded solution.");

				SolutionParser.SolutionParser solutionParser = new SolutionParser.SolutionParser(new TestPathResolver());
				foreach (SolutionParser.CompileCommand command in solutionParser.CreateCompileCommands(
					project, configurationNames[0], platformNames[0], "c11"
				))
				{
					cdb.AddCompileCommand(command);
				}
			}

			return cdb;
		}
	}
}
