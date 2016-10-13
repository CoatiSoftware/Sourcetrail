using System.Collections.Generic;

namespace CoatiSoftware.CoatiPlugin.SolutionParser
{
    public class CompilationDatabase
    {
        private List<CommandObject> _commandObjects = new List<CommandObject>();

        public void AddCommandObject(CommandObject commandObject)
        {
            _commandObjects.Add(commandObject);
        }

        public string SerializeJSON()
        {
            string result = "[\n";

            foreach(CommandObject cm in _commandObjects)
            {
                result += cm.SerializeJSON() + ",";
            }

            result += "\n]";

            return result;
        }
    }
}
