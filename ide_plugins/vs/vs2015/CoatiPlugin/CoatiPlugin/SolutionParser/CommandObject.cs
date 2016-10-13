
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
            set { _file = value; }
        }

        public string Directory
        {
            get { return _directory; }
            set { _directory = value; }
        }

        public string Command
        {
            get { return _command; }
            set { _command = value; }
        }

        public string SerializeJSON()
        {
            string result = "\t{\n";

            result += "\t\t\"directory\": \"" + _directory + "\",\n";
            result += "\t\t\"command\": \"" + _command + "\",\n";
            result += "\t\t\"file\": \"" + _file + "\"\n";

            result += "\t}";

            return result;
        }
    }
}
