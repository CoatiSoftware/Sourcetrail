#include "qt/commandline/QtCommandLineParser.h"

#include "utility/ConfigManager.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"


QtCommandLineParser::QtCommandLineParser()
{
}

QtCommandLineParser::~QtCommandLineParser()
{
}

void QtCommandLineParser::setup()
{

    setApplicationDescription("Coati helper");
    addVersionOption();
    addHelpOption();

    QCommandLineOption projectOption(QStringList() << "p" << "project",
                                             QCoreApplication::translate("main", "Load Coati with the a Coatiprojectfile <CoatiProject>."),
                                             QCoreApplication::translate("main", "CoatiProject"));
    addOption(projectOption);
}

void QtCommandLineParser::parseCommandline()
{

    if(isSet("project"))
    {
        QString projectfilename = value("project")   ;
        FilePath projectfile(projectfilename.toStdString());
        bool isValidProjectfile = true;
        std::stringstream errorstring("Provided Projectfile is not valid: ");
        errorstring << "Provided Projectfile('" << projectfile.fileName() << ") ";
        if(!projectfile.exists())
        {
            errorstring << "does not exist";
            isValidProjectfile = false;
        }

        if(projectfile.extension() != ".coatiproject")
        {
            errorstring << "has a wrong fileending";
            isValidProjectfile = false;
        }

        std::shared_ptr<ConfigManager> configManager = ConfigManager::createEmpty();
        if(!configManager->load(TextAccess::createFromFile(projectfile.str())))
        {
            errorstring << "could not be loaded";
            isValidProjectfile = false;
        }

        if(isValidProjectfile)
        {
            MessageLoadProject(projectfile.str()).dispatch();
        }
        else
        {
            MessageStatus(errorstring.str(), true).dispatch();
            LOG_ERROR(errorstring.str());
        }
    }
}