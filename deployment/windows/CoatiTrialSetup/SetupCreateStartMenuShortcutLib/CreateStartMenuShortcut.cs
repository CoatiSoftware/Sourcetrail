using IWshRuntimeLibrary;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace SetupCreateStartMenuShortcutLib
{
    [RunInstaller(true)]
    public partial class CreateStartMenuShortcut : System.Configuration.Install.Installer
    {
        private static string _appShortcutName = "Coati Trial.lnk";
        private static string _uninstallShortcutName = "Uninstall Coati Trial.lnk";

        public CreateStartMenuShortcut()
        {
            InitializeComponent();
        }

        public override void Commit(IDictionary savedState)
        {
            base.Commit(savedState);

            string app = System.Reflection.Assembly.GetExecutingAssembly().Location;

            string name = "install\\" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Name;
            int nameIdx = app.LastIndexOf(name);
            string appDirectory = app.Substring(0, nameIdx);

            app = appDirectory + "Coati Trial.exe";

            WshShell shell = new WshShell();
            string shortcutAddress = Environment.GetFolderPath(Environment.SpecialFolder.CommonStartMenu) + "\\Coati Trial\\"; // "\\Coati.lnk";
            
            if(!Directory.Exists(shortcutAddress))
            {
                Directory.CreateDirectory(shortcutAddress);
            }

            IWshShortcut shortcut = (IWshShortcut)shell.CreateShortcut(shortcutAddress + _appShortcutName);
            shortcut.Description = "Fast source code navigation and visualization";
            shortcut.TargetPath = app;
            shortcut.WorkingDirectory = appDirectory;
            shortcut.IconLocation = appDirectory + "data\\gui\\icon\\coati.ico";
            shortcut.Save();

            IWshShortcut uninstallShortcut = (IWshShortcut)shell.CreateShortcut(shortcutAddress + _uninstallShortcutName);
            uninstallShortcut.Description = "Uninstall Coati Trial";
            uninstallShortcut.TargetPath = appDirectory + "\\install\\uninstall.bat";
            uninstallShortcut.WorkingDirectory = appDirectory + "\\install";
            uninstallShortcut.Save();
        }

        protected override void OnAfterRollback(IDictionary savedState)
        {
            base.OnAfterRollback(savedState);

            DeleteShortcuts();
        }

        protected override void OnAfterUninstall(IDictionary savedState)
        {
            base.OnAfterUninstall(savedState);

            DeleteShortcuts();
        }

        private void DeleteShortcuts()
        {
            WshShell shell = new WshShell();
            string shortcutAddress = Environment.GetFolderPath(Environment.SpecialFolder.CommonStartMenu) + "\\Coati Trial\\";

            if (System.IO.File.Exists(shortcutAddress + _appShortcutName))
            {
                System.IO.File.Delete(shortcutAddress + _appShortcutName);
            }

            if (System.IO.File.Exists(shortcutAddress + _uninstallShortcutName))
            {
                System.IO.File.Delete(shortcutAddress + _uninstallShortcutName);
            }

            if(Directory.Exists(shortcutAddress))
            {
                Directory.Delete(shortcutAddress);
            }
        }
    }
}
