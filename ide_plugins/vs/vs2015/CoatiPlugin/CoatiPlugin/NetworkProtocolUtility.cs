using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoatiSoftware.CoatiPlugin
{
    class NetworkProtocolUtility
    {
        private static string s_divider = ">>";
        private static string s_setActiveTokenPrefix = "setActiveToken";
        private static string s_moveCursorPrefix = "moveCursor";
        private static string s_endOfMessageToken = "<EOM>";

        private static string s_createProjectPrefix = "createProject";
        private static string s_createCDBProjectPrefix = "createCDBProject";
        private static string s_ideId = "vs";

        public class CursorPosition
        {
            private string _filePath = "";
            private int _lineNumber = 0;
            private int _columnNumber = 0;
            private bool _valid = false;

            public string FilePath
            {
                get { return _filePath; }
                set { _filePath = value; }
            }

            public int LineNumber
            {
                get { return _lineNumber; }
                set { _lineNumber = value; }
            }

            public int ColumnNumber
            {
                get { return _columnNumber; }
                set { _columnNumber = value; }
            }

            public bool Valid
            {
                get { return _valid; }
                set { _valid = value; }
            }
        }

        public static string createActivateTokenMessage(string filePath, int lineNumber, int columnNumber)
        {
            string message = s_setActiveTokenPrefix;

            message += s_divider;

            message += filePath;

            message += s_divider;

            message += lineNumber.ToString();

            message += s_divider;

            message += columnNumber.ToString();

            message += s_endOfMessageToken;

            return message;
        }

        public static string createCreateProjectMessage(string solutionPath)
        {
            string message = s_createProjectPrefix;

            message += s_divider;

            message += solutionPath;

            message += s_divider;

            message += s_ideId;

            message += s_endOfMessageToken;

            return message;
        }

        public static string createCreateProjectMessage(string cdbPath, List<string> headerPaths)
        {
            string message = s_createCDBProjectPrefix;

            message += s_divider;

            message += cdbPath;

            message += s_divider;

            foreach(string path in headerPaths)
            {
                message += path;

                message += s_divider;
            }

            message += s_endOfMessageToken;

            return message;
        }

        public static CursorPosition parseSetCursorMessage(string message)
        {
            CursorPosition result = new CursorPosition();

            // remove eof token right away
            if(message.IndexOf(s_endOfMessageToken) > -1)
            {
                message = message.Substring(0, message.IndexOf(s_endOfMessageToken));
            }

            int pos = message.IndexOf(s_moveCursorPrefix);

            if (pos < 0)
            {
                return result;
            }

            message = message.Substring(pos + s_moveCursorPrefix.Length + s_divider.Length);

            pos = message.IndexOf(s_divider);

            if (pos < 0)
            {
                return result;
            }

            string filePath = message.Substring(0, pos);
            result.FilePath = filePath;

            message = message.Substring(pos+2);

            pos = message.IndexOf(s_divider);

            if (pos < 0)
            {
                return result;
            }

            string sLineNumber = message.Substring(0, pos);
            int lineNumber = 0;
            bool valid = int.TryParse(sLineNumber, out lineNumber);
            if (valid)
            {
                result.LineNumber = lineNumber;
            }

            message = message.Substring(pos+2);

            string sColumnNumber = message;
            int columnNumber = 0;
            valid = int.TryParse(sColumnNumber, out columnNumber);
            if (valid)
            {
                result.ColumnNumber = columnNumber;
                result.Valid = true;
            }

            return result;
        }
    }
}
