#include "utility/AppPath.h"
#include "utility/commandline/CommandLineParser.h"
#include "utility/ResourcePaths.h"
#include "utility/ScopedFunctor.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

#include "Application.h"
#include "ProjectFactoryModuleC.h"
#include "ProjectFactoryModuleCpp.h"
#include "ProjectFactoryModuleJava.h"
#include "includes.h" // defines 'void setup(int argc, char *argv[])'
#include "LicenseChecker.h"
#include "qt/network/QtNetworkFactory.h"
#include "qt/QtApplication.h"
#include "qt/QtCoreApplication.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "qt/window/QtMainWindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Coati");

	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	}

	Version version = Version::fromString(GIT_VERSION_NUMBER);
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	CommandLineParser commandLineParser(argc, argv, version.toString());
	if (commandLineParser.exitApplication())
	{
		return 0;
	}

	if (commandLineParser.runWithoutGUI())
	{
		setupPlatform(argc, argv);

		// headless Coati
		QtCoreApplication qtApp(argc, argv);

		setupApp(argc, argv);

		Application::createInstance(version, nullptr, nullptr);
		ScopedFunctor f([](){
			Application::destroyInstance();
		});

		Application::getInstance()->addProjectFactoryModule(std::make_shared<ProjectFactoryModuleC>());
		Application::getInstance()->addProjectFactoryModule(std::make_shared<ProjectFactoryModuleCpp>());
		Application::getInstance()->addProjectFactoryModule(std::make_shared<ProjectFactoryModuleJava>());

		std::shared_ptr<LicenseChecker> checker = LicenseChecker::getInstance();

		if (commandLineParser.startedWithLicense())
		{
			qtApp.saveLicense(commandLineParser.getLicense());
			return 0;
		}

		if (!checker->isCurrentLicenseValid()) // this works because the user cannot enter a license string while running the app in headless more.
		{
			LOG_WARNING("Your current Coati license seems to be invalid. Please update your license info.");
			return 0;
		}

		commandLineParser.projectLoad();
		return qtApp.exec();
	}
	else
	{
		setupPlatform(argc, argv);

		QtApplication qtApp(argc, argv);

		setupApp(argc, argv);

		qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

		QtViewFactory viewFactory;
		QtNetworkFactory networkFactory;

		Application::createInstance(version, &viewFactory, &networkFactory);
		ScopedFunctor f([](){
			Application::destroyInstance();
		});

		Application::getInstance()->addProjectFactoryModule(std::make_shared<ProjectFactoryModuleC>());
		Application::getInstance()->addProjectFactoryModule(std::make_shared<ProjectFactoryModuleCpp>());
		Application::getInstance()->addProjectFactoryModule(std::make_shared<ProjectFactoryModuleJava>());

		commandLineParser.projectLoad();

		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".otf");
		utility::loadFontsFromDirectory(ResourcePaths::getFontsPath(), ".ttf");

		return qtApp.exec();
	}
}

