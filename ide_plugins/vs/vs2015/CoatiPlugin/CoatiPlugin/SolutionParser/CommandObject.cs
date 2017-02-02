
namespace CoatiSoftware.CoatiPlugin.SolutionParser
{
    public class CommandObject
    {
        private string _file = "";
        private string _directory = "";
        private string _command = "";

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

        public string Directory
        {
            get { return _directory; }
            set { _directory = value; }
        }

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

        public string SerializeJSON()
        {
            string result = "\t{\n";

            result += "\t\t\"directory\": \"" + Directory + "\",\n";
            result += "\t\t\"command\": \"" + Command + "\",\n";
            result += "\t\t\"file\": \"" + File + "\"\n";

            result += "\t}";

            return result;
        }
    }
}
