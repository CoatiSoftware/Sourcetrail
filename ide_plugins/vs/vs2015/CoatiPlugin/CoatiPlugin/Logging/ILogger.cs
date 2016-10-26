using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CoatiSoftware.CoatiPlugin.Logging
{
    public interface ILogger
    {
        void LogMessage(LogMessage message);
    }
}
