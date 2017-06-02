
using Newtonsoft.Json;

namespace CoatiSoftware.SourcetrailPlugin.SolutionParser
{
	public class CompileCommand
	{
		private string _directory = "";
		private string _command = "";
		private string _file = "";

		[JsonProperty(propertyName: "directory")]
		public string Directory
		{
			get { return _directory; }
			set { _directory = value; }
		}

		[JsonProperty(propertyName: "command")]
		public string Command
		{
			get { return _command; }
			set
			{
				_command = value;
				//_command = _command.Replace('"', '\'');
				//_command = _command.Replace("\"", "'");
				//_command = _command.Replace("\\\"", "'");
			}
		}

		[JsonProperty(propertyName: "file")]
		public string File
		{
			get { return _file; }
			set
			{
				_file = value;
				//_file = _file.Replace('"', '\'');
				//_file = _file.Replace('\"', '\'');
				//_file = _file.Replace("\\\"", "'");
			}
		}

		public static bool operator ==(CompileCommand a, CompileCommand b)
		{
			if (System.Object.ReferenceEquals(a, b))
			{
				return true;
			}

			if (((object)a == null) || ((object)b == null))
			{
				return false;
			}

			if (a.File != b.File || a.Command != b.Command || a.Directory != b.Directory)
			{
				return false;
			}

			return true;
		}

		public static bool operator !=(CompileCommand a, CompileCommand b)
		{
			return !(a == b);
		}

		public string SerializeToJson()
		{
			return JsonConvert.SerializeObject(this, Newtonsoft.Json.Formatting.Indented);
		}

		public static CompileCommand DeserializeFromJson(string serialized)
		{
			CompileCommand command = null;
			if (serialized.Length > 0)
			{
				command = JsonConvert.DeserializeObject<CompileCommand>(serialized);
			}
			return command;
		}
	}
}
