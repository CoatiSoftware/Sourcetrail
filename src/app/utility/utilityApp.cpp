#include "utility/utilityApp.h"

#include <QProcess>

std::string utility::executeProcess(const char *cmd)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start(cmd);
	process.waitForFinished();
	std::string processoutput = process.readAll().toStdString();
	process.close();

	return processoutput;
}
