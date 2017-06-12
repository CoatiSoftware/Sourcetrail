using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;

namespace CoatiSoftware.SourcetrailPlugin.SolutionParser
{
	public class CompilationDatabaseSettings
	{
		private string _name = "";
		private string _sourceProject = "";
		private string _directory = "";

		private System.DateTime _lastUpdated = new System.DateTime();

		private List<string> _includedProjects = new List<string>();
		private string _configurationName = "";
		private string _platformName = "";

		public string Name
		{
			get { return _name; }
			set { _name = value; }
		}

		public string SourceProject
		{
			get { return _sourceProject; }
			set { _sourceProject = value; }
		}

		public string Directory
		{
			get { return _directory; }
			set { _directory = value; }
		}

		public System.DateTime LastUpdated
		{
			get { return _lastUpdated; }
			set { _lastUpdated = value; }
		}

		public List<string> IncludedProjects
		{
			get { return _includedProjects; }
			set { _includedProjects = value; }
		}

		public string ConfigurationName
		{
			get { return _configurationName; }
			set { _configurationName = value; }
		}

		public string PlatformName
		{
			get { return _platformName; }
			set { _platformName = value; }
		}

		public bool CheckCdbExists()
		{
			if (_directory.Length > 0 && _name.Length > 0)
			{
				string path = GetFilePath();

				if(System.IO.File.Exists(path) == false)
				{
					_lastUpdated = System.DateTime.MinValue; // if file is missing, set date way back so it doesn't interfere with re-building
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				Logging.Logging.LogWarning("Can't check cdb data, directory and/or name has not been set: directory - '" + _directory + "', name - '" + _name + "'");
			}

			return false;
		}

		public XmlNode GetMetaDataXML(XmlDocument doc)
		{
			// XmlDocument doc = new XmlDocument();
			XmlNode root = doc.CreateElement("cdb");

			XmlElement name = doc.CreateElement("name");
			name.InnerText = _name;

			XmlElement sourceProject = doc.CreateElement("sourceProject");
			sourceProject.InnerText = _sourceProject;

			XmlElement directory = doc.CreateElement("directory");
			directory.InnerText = _directory;

			XmlElement lastUpdated = doc.CreateElement("lastUpdated");
			lastUpdated.InnerText = _lastUpdated.ToString();

			XmlElement includedProjects = doc.CreateElement("includedProjects");
			foreach (string project in _includedProjects)
			{
				XmlElement includedProject = doc.CreateElement("includedProject");
				includedProject.InnerText = project;
				includedProjects.AppendChild(includedProject);
			}

			XmlElement configuration = doc.CreateElement("configuration");
			configuration.InnerText = _configurationName;

			XmlElement platform = doc.CreateElement("platform");
			platform.InnerText = _platformName;

			root.AppendChild(name);
			root.AppendChild(sourceProject);
			root.AppendChild(directory);
			root.AppendChild(lastUpdated);
			root.AppendChild(includedProjects);
			root.AppendChild(configuration);
			root.AppendChild(platform);

			return root;
		}

		public string SerializeMetaDataXML()
		{
			XmlDocument doc = new XmlDocument();
			XmlNode root = GetMetaDataXML(doc);

			System.IO.StringWriter writer = new System.IO.StringWriter();

			XmlSerializer serializer = new XmlSerializer(typeof(XmlElement));
			serializer.Serialize(writer, root);

			return writer.ToString();
		}

		public static List<CompilationDatabaseSettings> ParseCdbsMetaData(string data)
		{
			List<CompilationDatabaseSettings> cdbs = new List<CompilationDatabaseSettings>();

			if(data.Length > 0)
			{
				XmlDocument doc = new XmlDocument();
				doc.LoadXml(data);
				XmlNode root = doc.SelectSingleNode("cdbs");

				XmlNodeList nodes = root.SelectNodes("cdb");

				foreach (XmlNode node in nodes)
				{
					cdbs.Add(ParseCdbMetaData(node));
				}
			}

			return cdbs;
		}

		public static CompilationDatabaseSettings ParseCdbMetaData(XmlNode node)
		{
			CompilationDatabaseSettings cdb = new CompilationDatabaseSettings();

			XmlNode nameNode = node.SelectSingleNode("name");
			string name = nameNode.InnerText;

			XmlNode sourceNode = node.SelectSingleNode("sourceProject");
			string source = sourceNode.InnerText;

			XmlNode directoryNode = node.SelectSingleNode("directory");
			string directory = directoryNode.InnerText;

			XmlNode updatedNode = node.SelectSingleNode("lastUpdated");
			string updated = updatedNode.InnerText;
			System.DateTime updatedDate;
			if(System.DateTime.TryParse(updated, out updatedDate) == false)
			{
				updatedDate = System.DateTime.MinValue;
			}

			XmlNode includedProjects = node.SelectSingleNode("includedProjects");
			XmlNodeList includedProjectNodes = includedProjects.SelectNodes("includedProject");
			List<string> includedProjectsList = new List<string>();
			foreach(XmlNode p in includedProjectNodes)
			{
				includedProjectsList.Add(p.InnerText);
			}

			XmlNode configurationNode = node.SelectSingleNode("configuration");
			string configuration = configurationNode.InnerText;

			XmlNode platformNode = node.SelectSingleNode("platform");
			string platform = platformNode.InnerText;

			// if cdb file is not there anymore, set the modified date back so that a full update will be performed
			if(System.IO.File.Exists(directory + "\\" + name + ".json") == false)
			{
				updatedDate = System.DateTime.MinValue;
			}

			cdb.Name = name;
			cdb.SourceProject = source;
			cdb.Directory = directory;
			cdb.LastUpdated = updatedDate;
			cdb.IncludedProjects = includedProjectsList;
			cdb.ConfigurationName = configuration;
			cdb.PlatformName = platform;

			return cdb;
		}

		private string GetFilePath()
		{
			return _directory + "\\" + _name + ".json";
		}
	}
}
