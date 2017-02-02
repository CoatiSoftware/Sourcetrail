using System;

namespace CoatiSoftware.CoatiPlugin.Utility
{
    class DataUtility
    {
        static private string _standardFolder = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + "\\Coati Software\\Plugins\\VS\\";
        static private string _standardFileName = "cdbs.coatidata";

        static private DataUtility _instance = null;

        static private bool _valid = true; // stores if a file system operation failed, indicating that there is something wrong

        static public string GetStandardFolderDirectory()
        {
            return _standardFolder;
        }

        static bool Valid
        {
            get { return _valid; }
        }

        static public DataUtility GetInstance()
        {
            if(_instance == null)
            {
                _instance = new DataUtility();
            }

            return _instance;
        }

        private DataUtility()
        {
            CreateStandardFolderIfNotExists();
            CreateStandardFileIfNotExists();
        }

        public void AppendData(string data)
        {
            try
            {
                using (System.IO.StreamWriter file = System.IO.File.AppendText(_standardFolder + _standardFileName))
                {
                    file.WriteLine(data);
                }
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Failed to write data to file '" + _standardFolder + _standardFileName + "':" + e.Message);
                _valid = false;
            }
        }

        public string GetData()
        {
            string result = "";

            try
            {
                string data = "";
                if (System.IO.File.Exists(_standardFolder + _standardFileName))
                {
                    using (System.IO.StreamReader file = new System.IO.StreamReader(_standardFolder + _standardFileName))
                    {
                        string line = "";

                        while ((line = file.ReadLine()) != null)
                        {
                            data += line;
                        }
                    }
                }
                result = data;
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Failed to read data from file '" + _standardFolder + _standardFileName + "':" + e.Message);
                _valid = false;
            }

            return result;
        }

        public void ClearData()
        {
            try
            {
                System.IO.File.WriteAllText(_standardFolder + _standardFileName, "");
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Failed to clear data: " + e.Message);
            }
        }

        private void CreateStandardFolderIfNotExists()
        {
            try
            {
                if (System.IO.Directory.Exists(_standardFolder) == false)
                {
                    System.IO.Directory.CreateDirectory(_standardFolder);
                }
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Failed to create folder: " + e.Message);
                _valid = false;
            }
        }

        private void CreateStandardFileIfNotExists()
        {
            try
            {
                if (System.IO.File.Exists(_standardFolder + _standardFileName) == false)
                {
                    System.IO.File.Create(_standardFolder + _standardFileName);
                }
            }
            catch(Exception e)
            {
                Logging.Logging.LogError("Failed to create file: " + e.Message);
                _valid = false;
            }
        }
    }
}
