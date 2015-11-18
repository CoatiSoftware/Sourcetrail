using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoatiSoftware.CoatiPlugin
{
    class SystemUtility
    {
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern bool SetForegroundWindow(IntPtr hWnd);
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern IntPtr SetActiveWindow(IntPtr hWnd);

        public static void GetWindowFocus()
        {
            System.Diagnostics.Process process = System.Diagnostics.Process.GetCurrentProcess();
            IntPtr windowHandle = process.MainWindowHandle;

            if (windowHandle != null)
            {
                SetForegroundWindow(windowHandle);
                SetActiveWindow(windowHandle);
            }
        }
    }
}
