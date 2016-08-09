using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Deployment.WindowsInstaller;
using System.Xml;

namespace SetupAppSettings
{
    public class CustomActions
    {
        [CustomAction]
        public static ActionResult Main(Session session)
        {
            session.Log("Configuring AppSettings");

            try
            {
                SetupAppSettings();
            }
            catch(Exception e)
            {
                session.Log("Failed to configure AppSettings.");
                session.Log("Exception: " + e.ToString());
                return ActionResult.NotExecuted;
            }

            session.Log("Done configuring AppSettings");

            return ActionResult.Success;
        }

        private static void SetupAppSettings()
        {
            string appDataCoatiPath = Environment.GetEnvironmentVariable("APPDATA") + "\\..\\local\\Coati Software\\Coati\\";
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
