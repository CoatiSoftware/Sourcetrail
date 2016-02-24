using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Linq;
using System.Threading.Tasks;
using System.Xml;

namespace SetupApplicationSettingsLib
{
    [RunInstaller(true)]
    public partial class SetupApplicationSettings : System.Configuration.Install.Installer
    {
        public SetupApplicationSettings()
        {
            InitializeComponent();
        }

        public override void Commit(IDictionary savedState)
        {
            base.Commit(savedState);

            string appDataCoatiPath = Environment.GetEnvironmentVariable("APPDATA") + "\\..\\local\\Coati Software\\Coati Trial\\";
            string appSettingsPath = appDataCoatiPath + "ApplicationSettings.xml";
            string projectsPath = appDataCoatiPath + "projects\\";

            XmlDocument appSettings = new XmlDocument();
            appSettings.Load(@appSettingsPath);

            XmlNode recentProjects = appSettings.SelectSingleNode("config/user/recent_projects");
            recentProjects.RemoveAll();

            XmlNode tutorial = appSettings.CreateElement("recent_project");
            tutorial.InnerText = projectsPath + "tutorial\\tutorial.coatiproject";
            recentProjects.AppendChild(tutorial);

            XmlNode tictactoe = appSettings.CreateElement("recent_project");
            tictactoe.InnerText = projectsPath + "tictactoe\\tictactoe.coatiproject";
            recentProjects.AppendChild(tictactoe);

            appSettings.Save(appSettingsPath);
        }
    }
}
