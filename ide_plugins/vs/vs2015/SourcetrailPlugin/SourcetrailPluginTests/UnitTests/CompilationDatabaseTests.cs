using CoatiSoftware.SourcetrailPlugin.SolutionParser;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CoatiSoftware.SourcetrailPlugin.IntegrationTests.UnitTests
{
	[TestClass]
	public class CompilationDatabaseTests
	{
		[TestMethod]
		public void TestComparingCompileCommandWithSelfByValueWorks()
		{
			CompileCommand command1 = new CompileCommand();
			command1.File = "test.cpp";
			command1.Directory = "./";
			command1.Command = "D test test.cpp";

			CompileCommand command2 = new CompileCommand();
			command2.File = "test.cpp";
			command2.Directory = "./";
			command2.Command = "D test test.cpp";

			Assert.IsTrue(command1 == command2);
		}

		[TestMethod]
		public void TestComparingCompilationDatabaseWithSelfByValueWorks()
		{
			CompileCommand command = new CompileCommand();
			command.File = "test.cpp";
			command.Directory = "./";
			command.Command = "D test test.cpp";

			CompilationDatabase cdb1 = new CompilationDatabase();
			cdb1.AddCompileCommand(command);

			CompilationDatabase cdb2 = new CompilationDatabase();
			cdb2.AddCompileCommand(command);

			Assert.IsTrue(cdb1 == cdb2);
		}

		[TestMethod]
		public void TestCompilationDatabaseRetainsEscapedQuotesWhenDeserializedAfterSerialization()
		{
			CompileCommand command = new CompileCommand();
			command.File = "test.cpp";
			command.Directory = "./";
			command.Command = "D DEFINE=\"value\" test.cpp";

			CompilationDatabase originalCompilationDatabase = new CompilationDatabase();
			originalCompilationDatabase.AddCompileCommand(command);
			string serialized = originalCompilationDatabase.SerializeToJson();

			CompilationDatabase deserializedCompilationDatabase = new CompilationDatabase();
			deserializedCompilationDatabase.DeserializeFromJson(serialized);

			Assert.IsTrue(deserializedCompilationDatabase == originalCompilationDatabase);
		}
	}
}
