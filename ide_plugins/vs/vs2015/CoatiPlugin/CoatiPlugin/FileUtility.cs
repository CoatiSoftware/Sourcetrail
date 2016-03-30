using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EnvDTE;

namespace CoatiSoftware.CoatiPlugin
{
    class FileUtility
    {
        public delegate void ErrorCallback(string message);

        public static ErrorCallback _errorCallback = null;

        /**
         * Returns true when file was found, false otherwise
         */
        public static bool OpenSourceFile(DTE dte, string fileName)
        {
            try
            {
                dte.ItemOperations.OpenFile(fileName);
                return true;
            }
            catch (Exception e)
            {
                if (_errorCallback != null)
                {
                    _errorCallback("Failed to open file at " + fileName);
                }
                return false;
            }
        }

        public static void GoToLine(DTE dte, int lineNumber, int columnNumber)
        {
            try
            {
                ((EnvDTE.TextSelection)dte.ActiveDocument.Selection).MoveToLineAndOffset(lineNumber, columnNumber);
            }
            catch (Exception e)
            {
                if (_errorCallback != null)
                {
                    _errorCallback("Failed to set cursor to position [" + lineNumber.ToString() + "," + columnNumber.ToString() + "]");
                }
            }
        }

        public static string GetActiveDocumentName(DTE dte)
        {
            return dte.ActiveDocument.Name;
        }

        public static string GetActiveDocumentPath(DTE dte)
        {
            return dte.ActiveDocument.Path;
        }

        public static int GetActiveLineNumber(DTE dte)
        {
            return ((EnvDTE.TextSelection)dte.ActiveDocument.Selection).ActivePoint.Line;
        }

        public static int GetActiveColumnNumber(DTE dte)
        {
            return ((EnvDTE.TextSelection)dte.ActiveDocument.Selection).ActivePoint.LineCharOffset;
        }
    }
}
