using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace SetupRemoveCacheFiles
{
    [RunInstaller(true)]
    public partial class SetupRemoveCacheFiles : System.Configuration.Install.Installer
    {
        public SetupRemoveCacheFiles()
        {
            InitializeComponent();
        }

        protected override void OnAfterRollback(IDictionary savedState)
        {
            base.OnAfterRollback(savedState);

            DeleteCacheFiles();
        }

        protected override void OnAfterUninstall(IDictionary savedState)
        {
            base.OnAfterUninstall(savedState);

            DeleteCacheFiles();
        }

        private void DeleteCacheFiles()
        {
            try
            {
                string appDataCoatiPath = Environment.GetEnvironmentVariable("APPDATA") + "\\..\\local\\Coati Software\\Coati Trial"; // \\Coati\\";

                Directory.Delete(appDataCoatiPath, true);
            }
            catch(Exception e)
            {
                // well damn
            }
        }
    }
}
