using CoatiSoftware.SourcetrailPlugin.SolutionParser;
using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
	class CompilationDatabaseSettingsList
	{
		private List<CompilationDatabaseSettings> _settings = new List<CompilationDatabaseSettings>();

		public List<CompilationDatabaseSettings> Settings
		{
			get { return _settings; }
		}

		public CompilationDatabaseSettingsList()
		{
			Refresh();
		}

		public void AppendOrUpdate(CompilationDatabaseSettings cdb)
		{
			if(_settings.Exists(item => item.Name == cdb.Name && item.Directory == cdb.Directory) == false)
			{
				_settings.Add(cdb);
			}
			else
			{
				int idx = _settings.FindIndex(item => item.Name == cdb.Name && item.Directory == cdb.Directory);
				_settings[idx] = cdb;
			}
		}

		public void Refresh()
		{
			List<CompilationDatabaseSettings> cdbs = new List<CompilationDatabaseSettings>();

			try
			{
				string data = Utility.DataUtility.GetInstance().GetData();
				cdbs = CompilationDatabaseSettings.ParseCdbsMetaData(data);

				foreach (CompilationDatabaseSettings cdb in cdbs)
				{
					cdb.CheckCdbExists();
				}
			}
			catch (Exception e)
			{
				Logging.Logging.LogError("Failed to aquire data: " + e.Message);
			}

			_settings = cdbs;
		}

		public List<CompilationDatabaseSettings> GetCdbsForSolution(string solutionPath)
		{
			return _settings.FindAll(item => item.SourceProject == solutionPath);
		}

		public CompilationDatabaseSettings GetCdbForSolution(string solutionPath)
		{
			return _settings.Find(item => item.SourceProject == solutionPath);
		}

		public CompilationDatabaseSettings GetMostCurrentCdbForSolution(string solutionPath)
		{
			CompilationDatabaseSettings result = null;

			try
			{
				List<CompilationDatabaseSettings> candidates = GetCdbsForSolution(solutionPath);

				System.DateTime youngest = System.DateTime.MinValue;
				foreach (CompilationDatabaseSettings cdb in candidates)
				{
					if (cdb.LastUpdated >= youngest)
					{
						youngest = cdb.LastUpdated;
						result = cdb;
					}
				}
			}
			catch(Exception e)
			{
				Logging.Logging.LogError("Failed to find cdb: " + e.Message);
			}

			return result;
		}

		public CompilationDatabaseSettings GetCdbForSolution(string solutionPath, string cdbPath)
		{
			return _settings.Find(item => item.SourceProject == solutionPath && (item.Directory + "\\" + item.Name + ".json") == cdbPath);
		}

		public bool CheckCdbForSolutionExists(string solutionPath)
		{
			try
			{
				CompilationDatabaseSettings cdb = GetCdbForSolution(solutionPath);
				if (cdb != null && System.IO.File.Exists(cdb.Directory + "\\" + cdb.Name + ".json"))
				{
					return true;
				}
			}
			catch(Exception e)
			{
				Logging.Logging.LogError("Failed to check cdb: " + e.Message);
			}

			return false;
		}

		public void SaveMetaData()
		{
			try
			{
				XmlDocument doc = new XmlDocument();
				XmlNode root = doc.CreateElement("cdbs");

				foreach (CompilationDatabaseSettings cdb in _settings)
				{
					XmlNode metaData = cdb.GetMetaDataXML(doc);

					root.AppendChild(metaData);
				}

				System.IO.StringWriter writer = new System.IO.StringWriter();

				XmlSerializer serializer = new XmlSerializer(typeof(XmlElement));
				serializer.Serialize(writer, root);

				DataUtility.GetInstance().ClearData();
				DataUtility.GetInstance().AppendData(writer.ToString());
			}
			catch(Exception e)
			{
				Logging.Logging.LogError("Failed to save meta data: " + e.Message);
			}
		}
	}
}
