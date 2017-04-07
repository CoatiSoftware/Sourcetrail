using EnvDTE;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Diagnostics;

namespace CoatiSoftware.SourcetrailPlugin.Logging
{
    class VSOutputLogger : ILogger
    {
        private EnvDTE.DTE _dte = null;
        private OutputWindowPane _pane = null;

        public VSOutputLogger(EnvDTE.DTE dte)
        {
            _dte = dte;
        }

        public void LogMessage(LogMessage message)
        {
            if (message.MessageType == SourcetrailPlugin.Logging.LogMessage.LogMessageType.INFO)
            {
                Debug.WriteLine(message.Message, "Info");
                WriteToOutputWindow("Info: " + message.Message);
            }
            if (message.MessageType == SourcetrailPlugin.Logging.LogMessage.LogMessageType.WARNING)
            {
                Debug.WriteLine(message.Message, "Warning");
                WriteToOutputWindow("Warning: " + message.Message);
            }
            if (message.MessageType == SourcetrailPlugin.Logging.LogMessage.LogMessageType.ERROR)
            {
                Debug.WriteLine(message.Message, "Error");
                WriteToOutputWindow("Error: " + message.Message);
            }
        }

        private void WriteToOutputWindow(string message)
        {
            string paneName = "Sourcetrail Log";

            if (_dte.Windows.Count > 0)
            {
                Window window = _dte.Windows.Item(EnvDTE.Constants.vsWindowKindOutput);
                OutputWindow outputWindow = (OutputWindow)window.Object;

                OutputWindowPanes panes = outputWindow.OutputWindowPanes;

                if(_pane == null)
                {
                    try
                    {
                        for (int i = 0; i < panes.Count; i++)
                        {
                            OutputWindowPane pane = panes.Item(i);

                            if (pane.Name.Equals(paneName, StringComparison.CurrentCultureIgnoreCase))
                            {
                                _pane = outputWindow.OutputWindowPanes.Item(i);
                                break;
                            }
                        }
                    }
                    catch (Exception e)
                    {

                    }
                }

                if (_pane == null)
                {
                    _pane = outputWindow.OutputWindowPanes.Add(paneName);
                }

                _pane.OutputString(message + '\n');
            }
        }
    }
}
