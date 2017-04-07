using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;

namespace CoatiSoftware.SourcetrailPlugin.Utility
{
    class CompilationDatabaseList
    {
        private List<SolutionParser.CompilationDatabase> _cdbs = new List<SolutionParser.CompilationDatabase>();

        public List<SolutionParser.CompilationDatabase> CDBs
        {
            get { return _cdbs; }
            // set { _cdbs = value; }
        }

        public CompilationDatabaseList()
        {
            Refresh();
        }

        public void AppendOrUpdate(SolutionParser.CompilationDatabase cdb)
        {
            if(_cdbs.Exists(item => item.Name == cdb.Name && item.Directory == cdb.Directory) == false)
            {
                _cdbs.Add(cdb);
            }
            else
            {
                int idx = _cdbs.FindIndex(item => item.Name == cdb.Name && item.Directory == cdb.Directory);
                _cdbs[idx] = cdb;
            }
        }

        public void Refresh()
        {
            List<SolutionParser.CompilationDatabase> cdbs = new List<SolutionParser.CompilationDatabase>();

            try
            {
                string data = Utility.DataUtility.GetInstance().GetData();
                cdbs = SolutionParser.CompilationDatabase.ParseCDBsMetaData(data);

                foreach (SolutionParser.CompilationDatabase cdb in cdbs)
                {
                    cdb.CheckCDBExists();
                }
            }
            catch (Exception e)
            {
                Logging.Logging.LogError("Failed to aquire data: " + e.Message);
            }

            _cdbs = cdbs;
        }

        public List<SolutionParser.CompilationDatabase> GetCDBsForSolution(string solutionPath)
        {
            return _cdbs.FindAll(item => item.SourceProject == solutionPath);
        }

        public SolutionParser.CompilationDatabase GetCDBForSolution(string solutionPath)
        {
            return _cdbs.Find(item => item.SourceProject == solutionPath);
        }

        public SolutionParser.CompilationDatabase GetMostCurrentCDBForSolution(string solutionPath)
        {
            SolutionParser.CompilationDatabase result = null;

            try
            {
                List<SolutionParser.CompilationDatabase> candidates = GetCDBsForSolution(solutionPath);

                System.DateTime youngest = System.DateTime.MinValue;
                foreach (SolutionParser.CompilationDatabase cdb in candidates)
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

        public SolutionParser.CompilationDatabase GetCDBForSolution(string solutionPath, string cdbPath)
        {
            return _cdbs.Find(item => item.SourceProject == solutionPath && (item.Directory + "\\" + item.Name + ".json") == cdbPath);
        }

        public bool CheckCDBForSolutionExists(string solutionPath)
        {
            try
            {
                SolutionParser.CompilationDatabase cdb = GetCDBForSolution(solutionPath);
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

                foreach (SolutionParser.CompilationDatabase cdb in _cdbs)
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

        public void UnloadCDBs()
        {
            if(_cdbs == null)
            {
                Logging.Logging.LogWarning("Member '_cdbs' is null, aborting.");
                return;
            }

            foreach(SolutionParser.CompilationDatabase cdb in _cdbs)
            {
                cdb.ClearCommandObjects();
            }
        }
    }
}
