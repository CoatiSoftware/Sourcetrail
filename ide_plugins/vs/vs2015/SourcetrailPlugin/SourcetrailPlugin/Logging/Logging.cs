using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace CoatiSoftware.SourcetrailPlugin.Logging
{
	public class Logging
	{
		public static void LogInfo(string message, [CallerFilePath] string file = "", [CallerMemberName] string member = "", [CallerLineNumber] int line = 0)
		{
			int idx = file.LastIndexOf('\\');
			if(idx > -1)
			{
				file = file.Substring(idx + 1);
			}

			LogManager.GetInstance().LogInfo(message, file, member, line);
		}

		public static void LogWarning(string message, [CallerFilePath] string file = "", [CallerMemberName] string member = "", [CallerLineNumber] int line = 0)
		{
			int idx = file.LastIndexOf('\\');
			if (idx > -1)
			{
				file = file.Substring(idx + 1);
			}

			LogManager.GetInstance().LogWarning(message, file, member, line);
		}

		public static void LogError(string message, [CallerFilePath] string file = "", [CallerMemberName] string member = "", [CallerLineNumber] int line = 0)
		{
			int idx = file.LastIndexOf('\\');
			if (idx > -1)
			{
				file = file.Substring(idx + 1);
			}

			LogManager.GetInstance().LogError(message, file, member, line);
		}
	}
}
