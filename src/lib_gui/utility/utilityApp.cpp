#include "utility/utilityApp.h"

#include <QProcess>
#include "utility/utilityString.h"

std::string utility::executeProcess(const char *cmd)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start(cmd);
	process.waitForFinished();
	std::string processoutput = process.readAll().toStdString();
	process.close();
	processoutput = utility::trim(processoutput);

	return processoutput;
}
