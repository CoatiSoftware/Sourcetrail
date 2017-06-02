using System;
using System.Collections.Generic;

namespace CoatiSoftware.SourcetrailPlugin.Logging.Obfuscation
{
	class NameObfuscator
	{
		private static NameObfuscator _instance = null;

		private Dictionary<string, string> _dictionary = new Dictionary<string, string>();
		private bool _enabled = false;

		char _currentChar = 'a';
		int _currentInt = 0;

		private static string _directory = "";
		private static string _fileNamePrefix = "Dictionary_SourcetrailPlugin_";
		private static string _fileNameSufix = ".txt";

		private string _fileName = "";

		private Queue<string> _messageBacklog = new Queue<string>(); // stores messages if the log file was in use at original logging time

		private NameObfuscator()
		{
			DateTime time = DateTime.Now;

			string dateString = "";
			dateString += time.Year.ToString() + "-" + time.Month.ToString() + "-" + time.Day.ToString() + "_";
			dateString += time.Hour.ToString() + "-" + time.Minute.ToString() + "-" + time.Second.ToString();

			_fileName = _fileNamePrefix + dateString + _fileNameSufix;

			_directory = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
			_directory += "\\Coati Software\\Plugins\\VS\\";

			if (System.IO.Directory.Exists(_directory) == false)
			{
				System.IO.Directory.CreateDirectory(_directory);
			}
		}

		private static void CreateInstance()
		{
			if (_instance == null)
			{
				_instance = new NameObfuscator();
			}
		}

		public static string GetObfuscatedName(string originalName)
		{
			CreateInstance();

			if (_instance._enabled)
			{
				if (_instance._dictionary.ContainsKey(originalName))
				{
					return _instance._dictionary[originalName];
				}
				else
				{
					string newName = _instance.GetNewName();
					_instance._dictionary[originalName] = newName;

					_instance.WriteDictionaryEntryToFile(newName, originalName); // key and value reversed because that's the way the dictionary file is to be used...
					return newName;
				}
			}
			else
			{
				return originalName;
			}
		}

		public static void Enabled(bool enabled)
		{
			CreateInstance();

			_instance._enabled = enabled;
		}

		private string GetNewName()
		{
			string name = "";

			name = _currentChar.ToString() + _currentInt.ToString();

			++_currentChar;

			if((int)_currentChar > 122)
			{
				_currentChar = 'a';
				++_currentInt;
			}

			return name;
		}

		private void WriteDictionaryEntryToFile(string key, string value)
		{
			System.IO.StreamWriter writer = null;
			string message = key + " - " + value;

			try
			{
				writer = System.IO.File.AppendText(_directory + _fileName);

				// write backlog to file first
				while (_messageBacklog.Count > 0)
				{
					string bm = _messageBacklog.Dequeue();
					writer.WriteLine(bm);
				}

				writer.WriteLine(message.ToString());
				writer.Close();
			}
			catch (Exception e)
			{
				// well...file is still in use
				_messageBacklog.Enqueue(message.ToString());
			}
			finally
			{
				if (writer != null)
				{
					writer.Close();
				}
			}
		}
	}
}
