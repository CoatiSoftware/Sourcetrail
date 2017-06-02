using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoatiSoftware.SourcetrailPlugin.Logging
{
	public interface ILogger
	{
		void LogMessage(LogMessage message);
	}
}
