using Newtonsoft.Json;
using System.Collections.Generic;

namespace CoatiSoftware.SourcetrailPlugin.SolutionParser
{
	public class CompilationDatabase
	{
		private List<CompileCommand> _compileCommands = new List<CompileCommand>();

		private bool TryLoadData(string filePath)
		{
			if (filePath.Length > 0)
			{
				if (System.IO.File.Exists(filePath))
				{
					string data = "";

					using (System.IO.StreamReader file = new System.IO.StreamReader(filePath))
					{
						string line = "";

						while ((line = file.ReadLine()) != null)
						{
							data += line;
						}
					}

					DeserializeFromJson(data);

					return true;
				}
				else
				{
					Logging.Logging.LogError("The cdb file at \"" + filePath + "\" does not exist.");
				}
			}
			else
			{
				Logging.Logging.LogWarning("Can't load cdb data. Filepath has not been set.");
			}

			return false;
		}



		public static CompilationDatabase LoadFromFile(string filePath)
		{
			CompilationDatabase cdb = new CompilationDatabase();
			bool success = cdb.TryLoadData(filePath);
			return cdb;
		}


		public string SerializeToJson()
		{
			return JsonConvert.SerializeObject(_compileCommands, Newtonsoft.Json.Formatting.Indented);
		}

		public void DeserializeFromJson(string serialized)
		{
			_compileCommands.Clear();

			if (serialized.Length > 0)
			{
				_compileCommands = JsonConvert.DeserializeObject<List<CompileCommand>>(serialized);
			}
		}


		public int CompileCommandCount
		{
			get { return _compileCommands.Count; }
		}

		public static bool operator ==(CompilationDatabase a, CompilationDatabase b)
		{
			if (System.Object.ReferenceEquals(a, b))
			{
				return true;
			}

			if (((object)a == null) || ((object)b == null))
			{
				return false;
			}

			if (a.CompileCommandCount != b.CompileCommandCount)
			{
				return false;
			}

			foreach (CompileCommand aCommand in a._compileCommands)
			{
				CompileCommand bCommand = b._compileCommands.Find(x => x == aCommand);
				if (bCommand == null || aCommand.File != bCommand.File)
				{
					return false;
				}
			}

			return true;
		}

		public static bool operator !=(CompilationDatabase a, CompilationDatabase b)
		{
			return !(a == b);
		}

		public void AddCompileCommand(CompileCommand commandObject)
		{
			_compileCommands.Add(commandObject);
		}

		public void SortAlphabetically()
		{
			_compileCommands.Sort((c1, c2) => c1.File.CompareTo(c2.File));
		}

		public void Clear()
		{
			_compileCommands.Clear();
		}
	}
}
