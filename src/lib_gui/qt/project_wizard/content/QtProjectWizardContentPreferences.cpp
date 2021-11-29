#include "QtProjectWizardContentPreferences.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextCodec>
#include <QTimer>

#include "ApplicationSettings.h"
#include "FileLogger.h"
#include "FileSystem.h"
#include "MessageSwitchColorScheme.h"
#include "ResourcePaths.h"
#include "logging.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityPathDetection.h"
#include "utilityQt.h"

QtProjectWizardContentPreferences::QtProjectWizardContentPreferences(QtProjectWizardWindow* window)
	: QtProjectWizardContent(window)
	, m_oldColorSchemeIndex(-1)
	, m_newColorSchemeIndex(-1)
	, m_screenAutoScaling(nullptr)
	, m_screenScaleFactor(nullptr)
{
	m_colorSchemePaths = FileSystem::getFilePathsFromDirectory(
		ResourcePaths::getColorSchemesDirectoryPath(), {L".xml"});
}

QtProjectWizardContentPreferences::~QtProjectWizardContentPreferences()
{
	if (m_oldColorSchemeIndex != -1 && m_oldColorSchemeIndex != m_newColorSchemeIndex)
	{
		colorSchemeChanged(m_oldColorSchemeIndex);
	}
}

void QtProjectWizardContentPreferences::populate(QGridLayout* layout, int& row)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	// ui
	addTitle(QStringLiteral("USER INTERFACE"), layout, row);

	// font face
	m_fontFacePlaceHolder = new QtComboBoxPlaceHolder();
	m_fontFace = new QFontComboBox();
	m_fontFace->setEditable(false);
	addLabelAndWidget(QStringLiteral("Font Face"), m_fontFacePlaceHolder, layout, row);

	int rowNum = row;
	connect(
		m_fontFacePlaceHolder,
		&QtComboBoxPlaceHolder::opened,
		[this, rowNum, layout]()
		{
			m_fontFacePlaceHolder->hide();

			QString name = m_fontFace->currentText();
			m_fontFace->setFontFilters(QFontComboBox::MonospacedFonts);
			m_fontFace->setWritingSystem(QFontDatabase::Latin);
			m_fontFace->setCurrentText(name);

			addWidget(m_fontFace, layout, rowNum);

			QTimer::singleShot(10, [this]() { m_fontFace->showPopup(); });
		});
	row++;

	// font size
	m_fontSize = addComboBox(
		QStringLiteral("Font Size"),
		appSettings->getFontSizeMin(),
		appSettings->getFontSizeMax(),
		QLatin1String(""),
		layout,
		row);

	// tab width
	m_tabWidth = addComboBox(QStringLiteral("Tab Width"), 1, 16, QLatin1String(""), layout, row);

	// text encoding
	m_textEncoding = addComboBox(QStringLiteral("Text Encoding"), QLatin1String(""), layout, row);
	for (int mib: QTextCodec::availableMibs())
	{
		m_textEncoding->addItem(QTextCodec::codecForMib(mib)->name());
	}

	// color scheme
	m_colorSchemes = addComboBox(QStringLiteral("Color Scheme"), QLatin1String(""), layout, row);
	for (size_t i = 0; i < m_colorSchemePaths.size(); i++)
	{
		m_colorSchemes->insertItem(
			static_cast<int>(i),
			QString::fromStdWString(m_colorSchemePaths[i].withoutExtension().fileName()));
	}
	connect(
		m_colorSchemes,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this,
		&QtProjectWizardContentPreferences::colorSchemeChanged);

	// animations
	m_useAnimations = addCheckBox(
		QStringLiteral("Animations"),
		QStringLiteral("Enable animations"),
		QStringLiteral("<p>Enable animations throughout the user interface.</p>"),
		layout,
		row);

	// built-in types
	m_showBuiltinTypes = addCheckBox(
		QStringLiteral("Built-in Types"),
		QStringLiteral("Show built-in types in graph when referenced"),
		QStringLiteral("<p>Enable display of referenced built-in types in the graph view.</p>"),
		layout,
		row);

	// directory in code
	m_showDirectoryInCode = addCheckBox(
		QStringLiteral("Directory in File Title"),
		QStringLiteral("Show directory of file in code title"),
		QStringLiteral(
			"<p>Enable display of the parent directory of a code file relative to the project "
			"file.</p>"),
		layout,
		row);
	layout->setRowMinimumHeight(row - 1, 30);

	addGap(layout, row);


	// Linux UI scale
	if (utility::getOsType() == OS_LINUX)
	{
		// screen
		addTitle(QStringLiteral("SCREEN"), layout, row);

		QLabel* hint = new QLabel(QStringLiteral("<changes need restart>"));
		hint->setStyleSheet(QStringLiteral("color: grey"));
		layout->addWidget(hint, row - 1, QtProjectWizardWindow::BACK_COL, Qt::AlignRight);

		// auto scaling
		m_screenAutoScalingInfoLabel = new QLabel(QLatin1String(""));
		m_screenAutoScaling = addComboBoxWithWidgets(
			QStringLiteral("Auto Scaling to DPI"),
			QStringLiteral(
				"<p>Define if automatic scaling to screen DPI resolution is active. "
				"This setting manipulates the environment flag QT_AUTO_SCREEN_SCALE_FACTOR of the "
				"Qt "
				"framework "
				"(<a "
				"href=\"http://doc.qt.io/qt-5/highdpi.html\">http://doc.qt.io/qt-5/highdpi.html</"
				"a>). "
				"Choose 'system' to stick to the setting of your current environment.</p>"
				"<p>Changes to this setting require a restart of the application to take "
				"effect.</p>"),
			{m_screenAutoScalingInfoLabel},
			layout,
			row);
		m_screenAutoScaling->addItem(QStringLiteral("system"), -1);
		m_screenAutoScaling->addItem(QStringLiteral("off"), 0);
		m_screenAutoScaling->addItem(QStringLiteral("on"), 1);
		connect(
			m_screenAutoScaling,
			static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
			this,
			&QtProjectWizardContentPreferences::uiAutoScalingChanges);

		// scale factor
		m_screenScaleFactorInfoLabel = new QLabel(QLatin1String(""));
		m_screenScaleFactor = addComboBoxWithWidgets(
			QStringLiteral("Scale Factor"),
			QStringLiteral(
				"<p>Define a screen scale factor for the user interface of the application. "
				"This setting manipulates the environment flag QT_SCALE_FACTOR of the Qt framework "
				"(<a "
				"href=\"http://doc.qt.io/qt-5/highdpi.html\">http://doc.qt.io/qt-5/highdpi.html</"
				"a>). "
				"Choose 'system' to stick to the setting of your current environment.</p>"
				"<p>Changes to this setting require a restart of the application to take "
				"effect.</p>"),
			{m_screenScaleFactorInfoLabel},
			layout,
			row);
		m_screenScaleFactor->addItem(QStringLiteral("system"), -1.0);
		m_screenScaleFactor->addItem(QStringLiteral("25%"), 0.25);
		m_screenScaleFactor->addItem(QStringLiteral("50%"), 0.5);
		m_screenScaleFactor->addItem(QStringLiteral("75%"), 0.75);
		m_screenScaleFactor->addItem(QStringLiteral("100%"), 1.0);
		m_screenScaleFactor->addItem(QStringLiteral("125%"), 1.25);
		m_screenScaleFactor->addItem(QStringLiteral("150%"), 1.5);
		m_screenScaleFactor->addItem(QStringLiteral("175%"), 1.75);
		m_screenScaleFactor->addItem(QStringLiteral("200%"), 2.0);
		m_screenScaleFactor->addItem(QStringLiteral("250%"), 2.5);
		m_screenScaleFactor->addItem(QStringLiteral("300%"), 3.0);
		m_screenScaleFactor->addItem(QStringLiteral("400%"), 4.0);
		connect(
			m_screenScaleFactor,
			static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
			this,
			&QtProjectWizardContentPreferences::uiScaleFactorChanges);

		addGap(layout, row);
	}

	// Controls
	addTitle(QStringLiteral("CONTROLS"), layout, row);

	// scroll speed
	m_scrollSpeed = addLineEdit(
		QStringLiteral("Scroll Speed"),
		QStringLiteral(
			"<p>Set a multiplier for the in app scroll speed.</p>"
			"<p>A value between 0 and 1 results in slower scrolling while a value higher than 1 "
			"increases scroll speed.</p>"),
		layout,
		row);

	// graph zooming
	QString modifierName = utility::getOsType() == OS_MAC ? QStringLiteral("Cmd")
														  : QStringLiteral("Ctrl");
	m_graphZooming = addCheckBox(
		QStringLiteral("Graph Zoom"),
		QStringLiteral("Zoom graph on mouse wheel"),
		QStringLiteral("<p>Enable graph zoom using mouse wheel only, instead of using ") +
			modifierName + QStringLiteral(" + Mouse Wheel.</p>"),
		layout,
		row);

	addGap(layout, row);

	// output
	addTitle(QStringLiteral("OUTPUT"), layout, row);

	// logging
	m_loggingEnabled = addCheckBox(
		QStringLiteral("Logging"),
		QStringLiteral("Enable console and file logging"),
		QStringLiteral("<p>Show logs in the console and save this information in files.</p>"),
		layout,
		row);
	connect(
		m_loggingEnabled,
		&QCheckBox::clicked,
		this,
		&QtProjectWizardContentPreferences::loggingEnabledChanged);

	m_verboseIndexerLoggingEnabled = addCheckBox(
		QStringLiteral("Indexer Logging"),
		QStringLiteral("Enable verbose indexer logging"),
		QStringLiteral(
			"<p>Enable additional logs of abstract syntax tree traversal during indexing. This "
			"information can help "
			"tracking down crashes that occur during indexing.</p>"
			"<p><b>Warning</b>: This slows down indexing performance a lot.</p>"),
		layout,
		row);

	m_logPath = new QtLocationPicker(this);
	m_logPath->setPickDirectory(true);
	addLabelAndWidget(QStringLiteral("Log Directory Path"), m_logPath, layout, row);
	addHelpButton(
		QStringLiteral("Log Directory Path"),
		QStringLiteral("<p>Log file will be saved to this path.</p>"),
		layout,
		row);
	row++;

	addGap(layout, row);

	// Plugins
	addTitle(QStringLiteral("PLUGIN"), layout, row);

	// Sourcetrail port
	m_sourcetrailPort = addLineEdit(
		QStringLiteral("Sourcetrail Port"),
		QStringLiteral("<p>Port number that Sourcetrail uses to listen for incoming messages from "
					   "plugins.</p>"),
		layout,
		row);

	// Sourcetrail port
	m_pluginPort = addLineEdit(
		QStringLiteral("Plugin Port"),
		QStringLiteral(
			"<p>Port number that Sourcetrail uses to sends outgoing messages to plugins.</p>"),
		layout,
		row);

	addGap(layout, row);

	// indexing
	addTitle(QStringLiteral("INDEXING"), layout, row);

	// indexer threads
	m_threadsInfoLabel = new QLabel(QLatin1String(""));
	utility::setWidgetRetainsSpaceWhenHidden(m_threadsInfoLabel);
	m_threads = addComboBoxWithWidgets(
		QStringLiteral("Indexer Threads"),
		0,
		24,
		QStringLiteral(
			"<p>Set the number of threads used to work on indexing your project in parallel.</p>"),
		{m_threadsInfoLabel},
		layout,
		row);
	m_threads->setItemText(0, QStringLiteral("default"));
	connect(
		m_threads,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
		this,
		&QtProjectWizardContentPreferences::indexerThreadsChanges);

	// multi process indexing
	m_multiProcessIndexing = addCheckBox(
		QStringLiteral("Multi Process<br />C/C++ Indexing"),
		QStringLiteral("Run C/C++ indexer threads in different process"),
		QStringLiteral(
			"<p>Enable C/C++ indexer threads to run in different process.</p>"
			"<p>This prevents the application from crashing due to unforeseen exceptions while "
			"indexing.</p>"),
		layout,
		row);

	addGap(layout, row);


	// Java
	addTitle(QStringLiteral("JAVA"), layout, row);

	{
		// jvm library path
		m_javaPath = new QtLocationPicker(this);

		switch (utility::getOsType())
		{
		case OS_WINDOWS:
			m_javaPath->setFileFilter(QStringLiteral("JVM Library (jvm.dll)"));
			m_javaPath->setPlaceholderText(QStringLiteral("<jre_path>/bin/client/jvm.dll"));
			break;
		case OS_MAC:
			m_javaPath->setFileFilter(
				QStringLiteral("JLI or JVM Library (libjli.dylib libjvm.dylib)"));
			m_javaPath->setPlaceholderText(QStringLiteral(
				"/Library/Java/JavaVirtualMachines/<jdk_version>/Contents/MacOS/libjli.dylib"));
			break;
		case OS_LINUX:
			m_javaPath->setFileFilter(QStringLiteral("JVM Library (libjvm.so)"));
			m_javaPath->setPlaceholderText(
				QStringLiteral("<jre_path>/bin/<arch>/server/libjvm.so"));
			break;
		default:
			LOG_WARNING("No placeholders and filters set for Java path selection");
			break;
		}

		const std::string javaArchitectureString = utility::getApplicationArchitectureType() ==
				APPLICATION_ARCHITECTURE_X86_32
			? "32 Bit"
			: "64 Bit";

		addLabelAndWidget(
			("Java Path (" + javaArchitectureString + ")").c_str(), m_javaPath, layout, row);

		const std::string javaVersionString = javaArchitectureString + " Java 8";

		addHelpButton(
			QStringLiteral("Java Path"),
			("<p>Only required for indexing Java projects.</p>"
			 "<p>Provide the location of the jvm library inside the installation of your " +
			 javaVersionString +
			 " runtime environment (for information on how to set this take a look at "
			 "<a href=\"" +
			 utility::getDocumentationLink() +
			 "#finding-java-runtime-library-location\">"
			 "Finding Java Runtime Library Location</a> or use the auto detection below)</p>")
				.c_str(),
			layout,
			row);
		row++;

		m_javaPathDetector = utility::getJavaRuntimePathDetector();
		addJavaPathDetection(layout, row);
	}

	{
		// JRE System Library
		const QString title = QStringLiteral("JRE System Library");
		QLabel* label = createFormLabel(title);
		layout->addWidget(label, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

		addHelpButton(
			QStringLiteral("JRE System Library"),
			QStringLiteral("<p>Only required for indexing Java projects.</p>"
						   "<p>Add the jar files of your JRE System Library. These jars can be "
						   "found inside your "
						   "JRE install directory.</p>"),
			layout,
			row);

		m_jreSystemLibraryPaths = new QtPathListBox(
			this, title, QtPathListBox::SELECTION_POLICY_FILES_ONLY);

		layout->addWidget(m_jreSystemLibraryPaths, row, QtProjectWizardWindow::BACK_COL);
		row++;

		m_jreSystemLibraryPathsDetector = utility::getJreSystemLibraryPathsDetector();
		addJreSystemLibraryPathsDetection(layout, row);
	}
	{
		// maven path
		m_mavenPath = new QtLocationPicker(this);

#ifdef WIN32
		m_mavenPath->setFileFilter(QStringLiteral("Maven command (mvn.cmd)"));
		m_mavenPath->setPlaceholderText(QStringLiteral("<maven_path>/bin/mvn.cmd"));
#else
		m_mavenPath->setFileFilter(QStringLiteral("Maven command (mvn)"));
		m_mavenPath->setPlaceholderText(QStringLiteral("<binarypath>/mvn"));
#endif

		addLabelAndWidget(QStringLiteral("Maven Path"), m_mavenPath, layout, row);

		addHelpButton(
			QStringLiteral("Maven Path"),
			QStringLiteral("<p>Only required for indexing projects using Maven.</p>"
						   "<p>Provide the location of your installed Maven executable. You can "
						   "also use the auto "
						   "detection below.</p>"),
			layout,
			row);
		row++;

		m_mavenPathDetector = utility::getMavenExecutablePathDetector();
		addMavenPathDetection(layout, row);
	}

	addGap(layout, row);


	addTitle(QStringLiteral("Python"), layout, row);

	m_pythonPostProcessing = addCheckBox(
		QStringLiteral("Post Processing"),
		QStringLiteral("Add ambiguous edges for unsolved references (recommended)"),
		QStringLiteral("<p>Enable a post processing step to solve unsolved references after the "
					   "indexing is done. "
					   "</p>"
					   "<p>These references will be marked \"ambiguous\" to indicate that some of "
					   "these edges may "
					   "never "
					   "be encountered during runtime of the indexed code because the post "
					   "processing only relies "
					   "on "
					   "symbol names and types.</p>"),
		layout,
		row);

	addGap(layout, row);


	addTitle(QStringLiteral("C/C++"), layout, row);
}

void QtProjectWizardContentPreferences::load()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	QString fontName = QString::fromStdString(appSettings->getFontName());
	m_fontFace->setCurrentText(fontName);
	m_fontFacePlaceHolder->addItem(fontName);
	m_fontFacePlaceHolder->setCurrentText(fontName);

	m_fontSize->setCurrentIndex(appSettings->getFontSize() - appSettings->getFontSizeMin());
	m_tabWidth->setCurrentIndex(appSettings->getCodeTabWidth() - 1);

	m_textEncoding->setCurrentText(QString::fromStdString(appSettings->getTextEncoding()));

	FilePath colorSchemePath = appSettings->getColorSchemePath();
	for (int i = 0; i < static_cast<int>(m_colorSchemePaths.size()); i++)
	{
		if (colorSchemePath == m_colorSchemePaths[i])
		{
			m_colorSchemes->setCurrentIndex(i);
			m_oldColorSchemeIndex = i;
			m_newColorSchemeIndex = i;
			break;
		}
	}

	m_useAnimations->setChecked(appSettings->getUseAnimations());
	m_showBuiltinTypes->setChecked(appSettings->getShowBuiltinTypesInGraph());
	m_showDirectoryInCode->setChecked(appSettings->getShowDirectoryInCodeFileTitle());

	if (m_screenAutoScaling)
	{
		m_screenAutoScaling->setCurrentIndex(
			m_screenAutoScaling->findData(appSettings->getScreenAutoScaling()));
		uiAutoScalingChanges(m_screenAutoScaling->currentIndex());
	}

	if (m_screenScaleFactor)
	{
		m_screenScaleFactor->setCurrentIndex(
			m_screenScaleFactor->findData(appSettings->getScreenScaleFactor()));
		uiScaleFactorChanges(m_screenScaleFactor->currentIndex());
	}

	m_scrollSpeed->setText(QString::number(appSettings->getScrollSpeed(), 'f', 1));
	m_graphZooming->setChecked(appSettings->getControlsGraphZoomOnMouseWheel());

	m_loggingEnabled->setChecked(appSettings->getLoggingEnabled());
	m_verboseIndexerLoggingEnabled->setChecked(appSettings->getVerboseIndexerLoggingEnabled());
	m_verboseIndexerLoggingEnabled->setEnabled(m_loggingEnabled->isChecked());
	if (m_logPath)
	{
		m_logPath->setText(QString::fromStdWString(appSettings->getLogDirectoryPath().wstr()));
	}

	m_sourcetrailPort->setText(QString::number(appSettings->getSourcetrailPort()));
	m_pluginPort->setText(QString::number(appSettings->getPluginPort()));

	m_threads->setCurrentIndex(
		appSettings->getIndexerThreadCount());	  // index and value are the same
	indexerThreadsChanges(m_threads->currentIndex());
	m_multiProcessIndexing->setChecked(appSettings->getMultiProcessIndexingEnabled());

	if (m_javaPath)
	{
		m_javaPath->setText(QString::fromStdWString(appSettings->getJavaPath().wstr()));
	}

	m_jreSystemLibraryPaths->setPaths(appSettings->getJreSystemLibraryPaths());

	if (m_mavenPath)
	{
		m_mavenPath->setText(QString::fromStdWString(appSettings->getMavenPath().wstr()));
	}

	m_pythonPostProcessing->setChecked(appSettings->getPythonPostProcessingEnabled());
}

void QtProjectWizardContentPreferences::save()
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	appSettings->setFontName(m_fontFace->currentText().toStdString());

	appSettings->setFontSize(m_fontSize->currentIndex() + appSettings->getFontSizeMin());
	appSettings->setCodeTabWidth(m_tabWidth->currentIndex() + 1);

	appSettings->setTextEncoding(m_textEncoding->currentText().toStdString());

	appSettings->setColorSchemeName(
		m_colorSchemePaths[m_colorSchemes->currentIndex()].withoutExtension().fileName());
	m_oldColorSchemeIndex = -1;

	appSettings->setUseAnimations(m_useAnimations->isChecked());
	appSettings->setShowBuiltinTypesInGraph(m_showBuiltinTypes->isChecked());
	appSettings->setShowDirectoryInCodeFileTitle(m_showDirectoryInCode->isChecked());

	if (m_screenAutoScaling)
	{
		appSettings->setScreenAutoScaling(m_screenAutoScaling->currentData().toInt());
	}

	if (m_screenScaleFactor)
	{
		appSettings->setScreenScaleFactor(m_screenScaleFactor->currentData().toFloat());
	}

	float scrollSpeed = m_scrollSpeed->text().toFloat();
	if (scrollSpeed)
		appSettings->setScrollSpeed(scrollSpeed);

	appSettings->setControlsGraphZoomOnMouseWheel(m_graphZooming->isChecked());

	appSettings->setLoggingEnabled(m_loggingEnabled->isChecked());
	appSettings->setVerboseIndexerLoggingEnabled(m_verboseIndexerLoggingEnabled->isChecked());
	if (m_logPath && m_logPath->getText().toStdWString() != appSettings->getLogDirectoryPath().wstr())
	{
		appSettings->setLogDirectoryPath(FilePath((m_logPath->getText() + '/').toStdWString()));
		Logger* logger = LogManager::getInstance()->getLoggerByType("FileLogger");
		if (logger)
		{
			const auto fileLogger = dynamic_cast<FileLogger*>(logger);
			fileLogger->setLogDirectory(appSettings->getLogDirectoryPath());
			fileLogger->setFileName(FileLogger::generateDatedFileName(L"log"));
		}
	}

	int sourcetrailPort = m_sourcetrailPort->text().toInt();
	if (sourcetrailPort)
		appSettings->setSourcetrailPort(sourcetrailPort);

	int pluginPort = m_pluginPort->text().toInt();
	if (pluginPort)
		appSettings->setPluginPort(pluginPort);

	appSettings->setIndexerThreadCount(m_threads->currentIndex());	  // index and value are the same
	appSettings->setMultiProcessIndexingEnabled(m_multiProcessIndexing->isChecked());

	if (m_javaPath)
	{
		appSettings->setJavaPath(FilePath(m_javaPath->getText().toStdWString()));
	}

	appSettings->setJreSystemLibraryPaths(m_jreSystemLibraryPaths->getPathsAsAbsolute());

	if (m_mavenPath)
	{
		appSettings->setMavenPath(FilePath(m_mavenPath->getText().toStdWString()));
	}

	appSettings->setPythonPostProcessingEnabled(m_pythonPostProcessing->isChecked());

	appSettings->save();
}

bool QtProjectWizardContentPreferences::check()
{
	return true;
}

void QtProjectWizardContentPreferences::colorSchemeChanged(int index)
{
	m_newColorSchemeIndex = index;
	MessageSwitchColorScheme(m_colorSchemePaths[index]).dispatch();
}

void QtProjectWizardContentPreferences::javaPathDetectionClicked()
{
	std::vector<FilePath> paths = m_javaPathDetector->getPathsForDetector(
		m_javaPathDetectorBox->currentText().toStdString());
	if (!paths.empty())
	{
		m_javaPath->setText(QString::fromStdWString(paths.front().wstr()));
	}
}

void QtProjectWizardContentPreferences::jreSystemLibraryPathsDetectionClicked()
{
	std::vector<FilePath> paths = m_jreSystemLibraryPathsDetector->getPathsForDetector(
		m_jreSystemLibraryPathsDetectorBox->currentText().toStdString());
	std::vector<FilePath> oldPaths = m_jreSystemLibraryPaths->getPathsAsAbsolute();
	m_jreSystemLibraryPaths->setPaths(utility::unique(utility::concat(oldPaths, paths)));
}

void QtProjectWizardContentPreferences::mavenPathDetectionClicked()
{
	std::vector<FilePath> paths = m_mavenPathDetector->getPathsForDetector(
		m_mavenPathDetectorBox->currentText().toStdString());
	if (!paths.empty())
	{
		m_mavenPath->setText(QString::fromStdWString(paths.front().wstr()));
	}
}

void QtProjectWizardContentPreferences::loggingEnabledChanged()
{
	m_verboseIndexerLoggingEnabled->setEnabled(m_loggingEnabled->isChecked());
}

void QtProjectWizardContentPreferences::indexerThreadsChanges(int index)
{
	if (index == 0)
	{
		m_threadsInfoLabel->setText(
			("detected " + std::to_string(utility::getIdealThreadCount()) + " threads to be ideal.")
				.c_str());
		m_threadsInfoLabel->show();
	}
	else
	{
		m_threadsInfoLabel->hide();
	}
}

void QtProjectWizardContentPreferences::uiAutoScalingChanges(int index)
{
	if (index == 0)
	{
		QString autoScale(qgetenv("QT_AUTO_SCREEN_SCALE_FACTOR_SOURCETRAIL"));
		if (autoScale == QLatin1String("1"))
		{
			autoScale = QStringLiteral("on");
		}
		else
		{
			autoScale = QStringLiteral("off");
		}

		m_screenAutoScalingInfoLabel->setText(
			QStringLiteral("detected: '") + autoScale + QStringLiteral("'"));
		m_screenAutoScalingInfoLabel->show();
	}
	else
	{
		m_screenAutoScalingInfoLabel->hide();
	}
}

void QtProjectWizardContentPreferences::uiScaleFactorChanges(int index)
{
	if (index == 0)
	{
		QString scale = QStringLiteral("100");
		bool ok;
		double scaleFactor = qgetenv("QT_SCALE_FACTOR_SOURCETRAIL").toDouble(&ok);
		if (ok)
		{
			scale = QString::number(int(scaleFactor * 100));
		}

		m_screenScaleFactorInfoLabel->setText(
			QStringLiteral("detected: '") + scale + QStringLiteral("%'"));
		m_screenScaleFactorInfoLabel->show();
	}
	else
	{
		m_screenScaleFactorInfoLabel->hide();
	}
}

void QtProjectWizardContentPreferences::addJavaPathDetection(QGridLayout* layout, int& row)
{
	std::vector<std::string> detectorNames = m_javaPathDetector->getWorkingDetectorNames();
	if (detectorNames.empty())
	{
		return;
	}

	QLabel* label = new QLabel(QStringLiteral("Auto detection from:"));

	m_javaPathDetectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_javaPathDetectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton(QStringLiteral("detect"));
	button->setObjectName(QStringLiteral("windowButton"));
	connect(
		button,
		&QPushButton::clicked,
		this,
		&QtProjectWizardContentPreferences::javaPathDetectionClicked);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_javaPathDetectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(
		detectionWidget, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
	row++;
}

void QtProjectWizardContentPreferences::addJreSystemLibraryPathsDetection(QGridLayout* layout, int& row)
{
	const std::vector<std::string> detectorNames =
		m_jreSystemLibraryPathsDetector->getWorkingDetectorNames();
	if (detectorNames.empty())
	{
		return;
	}

	QLabel* label = new QLabel(QStringLiteral("Auto detection from:"));

	m_jreSystemLibraryPathsDetectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_jreSystemLibraryPathsDetectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton(QStringLiteral("detect"));
	button->setObjectName(QStringLiteral("windowButton"));
	connect(
		button,
		&QPushButton::clicked,
		this,
		&QtProjectWizardContentPreferences::jreSystemLibraryPathsDetectionClicked);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_jreSystemLibraryPathsDetectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(
		detectionWidget, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
	row++;
}

void QtProjectWizardContentPreferences::addMavenPathDetection(QGridLayout* layout, int& row)
{
	std::vector<std::string> detectorNames = m_mavenPathDetector->getWorkingDetectorNames();
	if (detectorNames.empty())
	{
		return;
	}

	QLabel* label = new QLabel(QStringLiteral("Auto detection from:"));

	m_mavenPathDetectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_mavenPathDetectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton(QStringLiteral("detect"));
	button->setObjectName(QStringLiteral("windowButton"));
	connect(
		button,
		&QPushButton::clicked,
		this,
		&QtProjectWizardContentPreferences::mavenPathDetectionClicked);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_mavenPathDetectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(
		detectionWidget, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
	row++;
}

void QtProjectWizardContentPreferences::addTitle(const QString& title, QGridLayout* layout, int& row)
{
	layout->addWidget(createFormTitle(title), row++, QtProjectWizardWindow::FRONT_COL, Qt::AlignLeft);
}

void QtProjectWizardContentPreferences::addLabel(const QString& label, QGridLayout* layout, int row)
{
	layout->addWidget(createFormLabel(label), row, QtProjectWizardWindow::FRONT_COL, Qt::AlignRight);
}

void QtProjectWizardContentPreferences::addWidget(
	QWidget* widget, QGridLayout* layout, int row, Qt::Alignment widgetAlignment)
{
	layout->addWidget(widget, row, QtProjectWizardWindow::BACK_COL, widgetAlignment);
}

void QtProjectWizardContentPreferences::addLabelAndWidget(
	const QString& label, QWidget* widget, QGridLayout* layout, int row, Qt::Alignment widgetAlignment)
{
	addLabel(label, layout, row);
	addWidget(widget, layout, row, widgetAlignment);
}

void QtProjectWizardContentPreferences::addGap(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 20);
}

QCheckBox* QtProjectWizardContentPreferences::addCheckBox(
	const QString& label, const QString& text, const QString& helpText, QGridLayout* layout, int& row)
{
	QCheckBox* checkBox = new QCheckBox(text, this);
	addLabelAndWidget(label, checkBox, layout, row, Qt::AlignLeft);

	if (helpText.size())
	{
		addHelpButton(label, helpText, layout, row);
	}

	row++;

	return checkBox;
}

QComboBox* QtProjectWizardContentPreferences::addComboBox(
	const QString& label, const QString& helpText, QGridLayout* layout, int& row)
{
	QComboBox* comboBox = new QComboBox(this);
	addLabelAndWidget(label, comboBox, layout, row, Qt::AlignLeft);

	if (helpText.size())
	{
		addHelpButton(label, helpText, layout, row);
	}

	row++;

	return comboBox;
}

QComboBox* QtProjectWizardContentPreferences::addComboBoxWithWidgets(
	const QString& label,
	const QString& helpText,
	std::vector<QWidget*> widgets,
	QGridLayout* layout,
	int& row)
{
	QComboBox* comboBox = new QComboBox(this);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(comboBox);

	for (QWidget* widget: widgets)
	{
		hlayout->addWidget(widget);
	}

	QWidget* container = new QWidget();
	container->setLayout(hlayout);

	addLabelAndWidget(label, container, layout, row, Qt::AlignLeft);

	if (helpText.size())
	{
		addHelpButton(label, helpText, layout, row);
	}

	row++;

	return comboBox;
}

QComboBox* QtProjectWizardContentPreferences::addComboBox(
	const QString& label, int min, int max, const QString& helpText, QGridLayout* layout, int& row)
{
	QComboBox* comboBox = addComboBox(label, helpText, layout, row);

	if (min != max)
	{
		for (int i = min; i <= max; i++)
		{
			comboBox->insertItem(i, QString::number(i));
		}
	}

	return comboBox;
}

QComboBox* QtProjectWizardContentPreferences::addComboBoxWithWidgets(
	const QString& label,
	int min,
	int max,
	const QString& helpText,
	std::vector<QWidget*> widgets,
	QGridLayout* layout,
	int& row)
{
	QComboBox* comboBox = addComboBoxWithWidgets(label, helpText, widgets, layout, row);

	if (min != max)
	{
		for (int i = min; i <= max; i++)
		{
			comboBox->insertItem(i, QString::number(i));
		}
	}

	return comboBox;
}

QLineEdit* QtProjectWizardContentPreferences::addLineEdit(
	const QString& label, const QString& helpText, QGridLayout* layout, int& row)
{
	QLineEdit* lineEdit = new QLineEdit(this);
	lineEdit->setObjectName(QStringLiteral("name"));
	lineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);

	addLabelAndWidget(label, lineEdit, layout, row);

	if (helpText.size())
	{
		addHelpButton(label, helpText, layout, row);
	}

	row++;

	return lineEdit;
}
