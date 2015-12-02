using IWshRuntimeLibrary;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Linq;
using System.Threading.Tasks;

namespace SetupCreateStartMenuShortcutLib
{
    [RunInstaller(true)]
    public partial class CreateStartMenuShortcut : System.Configuration.Install.Installer
    {
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

            app = appDirectory + "Coati.exe";

            WshShell shell = new WshShell();
            string shortcutAddress = Environment.GetFolderPath(Environment.SpecialFolder.CommonStartMenu) + "Coati.lnk";
            IWshShortcut shortcut = (IWshShortcut)shell.CreateShortcut(shortcutAddress);
            shortcut.Description = "Fast source code reading and navigation";
            shortcut.TargetPath = app;
            shortcut.WorkingDirectory = appDirectory;
            shortcut.Save();
        }

        protected override void OnAfterRollback(IDictionary savedState)
        {
            base.OnAfterRollback(savedState);

            DeleteDesktopShortcut();
        }

        protected override void OnAfterUninstall(IDictionary savedState)
        {
            base.OnAfterUninstall(savedState);

            DeleteDesktopShortcut();
        }

        private void DeleteDesktopShortcut()
        {
            object shDesktop = (object)"Desktop";
            WshShell shell = new WshShell();
            string shortcutAddress = Environment.GetFolderPath(Environment.SpecialFolder.CommonStartMenu) + "Coati.lnk";

            if (System.IO.File.Exists(shortcutAddress))
            {
                System.IO.File.Delete(shortcutAddress);
            }
        }
    }
}
