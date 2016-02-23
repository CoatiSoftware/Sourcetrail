#include "qt/commandline/QtCommandLineParser.h"

#include "utility/ConfigManager.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"


QtCommandLineParser::QtCommandLineParser()
    : m_projectFileString("")
    , m_noGUI(false)
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
    QCommandLineOption parseProjectWithoutGUIOption(QStringList() << "d" << "database",
                                             QCoreApplication::translate("main", "Start coati to parse the Coatiprojectfile <CoatiProject>. Result *.coatidb"),
                                             QCoreApplication::translate("main", "CoatiProject"));
    addOption(parseProjectWithoutGUIOption);
}

void QtCommandLineParser::evaluateCommandline()
{
    if (isSet("database"))
    {
        m_noGUI = true;
        processProjectfile(value("database").toStdString());
    }

    if(isSet("project"))
    {
        processProjectfile(value("project").toStdString());
    }
}

void QtCommandLineParser::projectLoad()
{
    if (!m_projectFileString.empty())
    {
        MessageLoadProject(m_projectFileString, false).dispatch();
    }
}

void QtCommandLineParser::processProjectfile(const std::string& file)
{
    FilePath projectfile(file);
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
        m_projectFileString = projectfile.str();
    }
    else
    {
        MessageStatus(errorstring.str(), true).dispatch();
        LOG_ERROR(errorstring.str());
    }
}

bool QtCommandLineParser::noGUI()
{
    return m_noGUI;
}

