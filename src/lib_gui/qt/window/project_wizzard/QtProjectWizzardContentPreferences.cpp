#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"

#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityApp.h"
#include "utility/utility.h"

QtProjectWizzardContentPreferences::QtProjectWizzardContentPreferences(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_oldColorSchemeIndex(-1)
	, m_newColorSchemeIndex(-1)
{
	std::vector<std::string> colorSchemePaths =
		FileSystem::getFileNamesFromDirectory(ResourcePaths::getColorSchemesPath(), std::vector<std::string>(1, ".xml"));
	for (const std::string& colorScheme : colorSchemePaths)
	{
		m_colorSchemePaths.push_back(FilePath(colorScheme));
	}
}

QtProjectWizzardContentPreferences::~QtProjectWizzardContentPreferences()
{
	if (m_oldColorSchemeIndex != -1 && m_oldColorSchemeIndex != m_newColorSchemeIndex)
	{
		colorSchemeChanged(m_oldColorSchemeIndex);
	}
}

void QtProjectWizzardContentPreferences::populate(QGridLayout* layout, int& row)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	// ui
	addTitle("USER INTERFACE", layout, row);

	// font face
	m_fontFace = new QtFontPicker(this);
	m_fontFace->setObjectName("name");
	m_fontFace->setAttribute(Qt::WA_MacShowFocusRect, 0);

	addLabelAndWidget("Font face", m_fontFace, layout, row);
	row++;

	// font size
	m_fontSize = addComboBox("Font size", appSettings->getFontSizeMin(), appSettings->getFontSizeMax(), "", layout, row);

	// tab width
	m_tabWidth = addComboBox("Tab width", 1, 16, "", layout, row);

	// color scheme
	m_colorSchemes = addComboBox("Color scheme", 0, 0, "", layout, row);
	for (size_t i = 0; i < m_colorSchemePaths.size(); i++)
	{
		m_colorSchemes->insertItem(i, m_colorSchemePaths[i].withoutExtension().fileName().c_str());
	}
	connect(m_colorSchemes, SIGNAL(activated(int)), this, SLOT(colorSchemeChanged(int)));

	// animations
	m_useAnimations = addCheckBox("Animations", "Enable animations",
		"Enable/disable animations throughout the user interface.", layout, row);

	// logging
	m_loggingEnabled = addCheckBox("Logging", "Enable console and file logging",
		"Save log files and show log information in the console.", layout, row);
	connect(m_loggingEnabled, SIGNAL(clicked()), this, SLOT(loggingEnabledChanged()));

	m_verboseIndexerLoggingEnabled = addCheckBox("Indexer Logging", "Enable verbose indexer logging",
		"Logs information of abstract syntax tree traversal during indexing. This information can help us "
		"reproduce crashes in indexing.\n\nThis slows down indexing performance a lot.", layout, row);

	addGap(layout, row);

	// Controls
	addTitle("CONTROLS", layout, row);

	// scroll speed
	m_scrollSpeed = addLineEdit("Scroll Speed", "Multiplier for scroll speed. Set to a value between 0 and 1 to scroll "
		"slower, or set to larger than 1 to scroll faster.", layout, row);

	// graph zooming
	QString modifierName = QSysInfo::macVersion() == QSysInfo::MV_None ? "Ctrl" : "Cmd";
	m_graphZooming = addCheckBox("Graph Zoom", "Zoom on mouse wheel",
		"Switch graph zooming to mouse wheel only, instead of " + modifierName + " + Mouse Wheel.", layout, row);

	addGap(layout, row);

	// indexing
	addTitle("INDEXING", layout, row);

	// indexer threads
	const int minThreadCount = 0;
	const int maxThreadCount = 24;

	m_threads = new QComboBox(this);
	connect(m_threads, SIGNAL(activated(int)), this, SLOT(indexerThreadsChanges(int)));
	for (int i = minThreadCount; i <= maxThreadCount; i++)
	{
		m_threads->insertItem(i, QString::number(i));
	}

	m_threadsInfoLabel = new QLabel("");
	utility::setWidgetRetainsSpaceWhenHidden(m_threadsInfoLabel);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(m_threads);
	hlayout->addWidget(m_threadsInfoLabel);

	QWidget* threadsWidget = new QWidget();
	threadsWidget->setLayout(hlayout);

	addLabelAndWidget("Indexer threads", threadsWidget, layout, row, Qt::AlignLeft);
	addHelpButton("Number of parallel threads used to index your projects.\nWhen setting this to 0 Sourcetrail tries to use the ideal thread count for your computer.", layout, row);
	row++;

	// multi process indexing
	m_multiProcessIndexing = addCheckBox("Multi process C/C++ indexing", "Use processes instead of threads for C/C++ indexing.",
		"Using processes instead of threads prevents the application from crashing on unforseen exceptions during indexing.",
		layout, row);

	addGap(layout, row);


	// Plugins
	addTitle("PLUGIN", layout, row);

	// Sourcetrail port
	m_sourcetrailPort = addLineEdit("Sourcetrail Port",
		"Port number that Sourcetrail uses to listen for incoming messages from plugins.", layout, row);

	// Sourcetrail port
	m_pluginPort = addLineEdit("Plugin Port", "Port number that Sourcetrail sends outgoing messages to.", layout, row);

	addGap(layout, row);

	// Java
	addTitle("JAVA", layout, row);

	// jvm library path
	m_javaPath = new QtLocationPicker(this);

	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		m_javaPath->setFileFilter("JVM Library (jvm.dll)");
		m_javaPath->setPlaceholderText("<jre_path>/bin/client/jvm.dll");
	}
	else if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		m_javaPath->setFileFilter("JLI or JVM Library (libjli.dylib libjvm.dylib)");
		m_javaPath->setPlaceholderText("<jre_path>/Contents/Home/jre/lib/jli/libjli.dylib");
	}
	else
	{
		m_javaPath->setFileFilter("JVM Library (libjvm.so)");
		m_javaPath->setPlaceholderText("<jre_path>/bin/<arch>/server/libjvm.so");
	}

	addLabelAndWidget("Java Path", m_javaPath, layout, row);

	std::string javaVersionString = "";
	switch (utility::getApplicationArchitectureType())
	{
	case APPLICATION_ARCHITECTURE_X86_32:
		javaVersionString = "32 Bit ";
		break;
	case APPLICATION_ARCHITECTURE_X86_64:
		javaVersionString = "64 Bit ";
		break;
	default:
		break;
	}
	javaVersionString += "Java 8";

	addHelpButton((
		"Only required for indexing Java projects.\n"
		"Provide the location of the jvm library inside the installation of your " + javaVersionString +
		" runtime environment (for information on how to set these take a look at "
		"<a href=\"https://sourcetrail.com/documentation/#FindingJavaRuntimeLibraryLocation\">"
		"Finding Java Runtime Library Location</a> or use the auto detection below)").c_str()
		, layout, row
	);
	row++;

	m_javaPathDetector = utility::getJavaRuntimePathDetector();
	addJavaPathDetection(layout, row);

	// jvm max memory
	m_jvmMaximumMemory = addLineEdit("JVM Maximum Memory", "Specify the maximum amount of memory that should be "
		"allocated by the indexer's JVM. A value of -1 ignores this setting.", layout, row);
	layout->setRowMinimumHeight(row - 1, 30);

	// maven path
	m_mavenPath = new QtLocationPicker(this);

	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		m_mavenPath->setFileFilter("Maven command (mvn.cmd)");
		m_mavenPath->setPlaceholderText("<maven_path>/bin/mvn.cmd");
	}
	else
	{
		m_mavenPath->setFileFilter("Maven command (mvn)");
		m_mavenPath->setPlaceholderText("<binarypath>/mvn");
	}

	addLabelAndWidget("Maven Path", m_mavenPath, layout, row);

	addHelpButton(
		"Only required for indexing projects using Maven.\n"
		"Provide the location of the Maven executable. You can also use the auto detection below."
		, layout, row
	);
	row++;

	m_mavenPathDetector = utility::getMavenExecutablePathDetector();
	addMavenPathDetection(layout, row);

	addGap(layout, row);

	// C/C++
	addTitle("C/C++", layout, row);
}

void QtProjectWizzardContentPreferences::load()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	m_fontFace->setText(QString::fromStdString(appSettings->getFontName()));

	m_fontSize->setCurrentIndex(appSettings->getFontSize() - appSettings->getFontSizeMin());
	m_tabWidth->setCurrentIndex(appSettings->getCodeTabWidth() - 1);

	FilePath colorSchemePath = appSettings->getColorSchemePath();
	for (size_t i = 0; i < m_colorSchemePaths.size(); i++)
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

	m_loggingEnabled->setChecked(appSettings->getLoggingEnabled());
	m_verboseIndexerLoggingEnabled->setChecked(appSettings->getVerboseIndexerLoggingEnabled());
	m_verboseIndexerLoggingEnabled->setEnabled(m_loggingEnabled->isChecked());

	m_scrollSpeed->setText(QString::number(appSettings->getScrollSpeed(), 'f', 1));
	m_graphZooming->setChecked(appSettings->getControlsGraphZoomOnMouseWheel());

	m_sourcetrailPort->setText(QString::number(appSettings->getSourcetrailPort()));
	m_pluginPort->setText(QString::number(appSettings->getPluginPort()));

	m_threads->setCurrentIndex(appSettings->getIndexerThreadCount()); // index and value are the same
	indexerThreadsChanges(m_threads->currentIndex());
	m_multiProcessIndexing->setChecked(appSettings->getMultiProcessIndexingEnabled());

	if (m_javaPath)
	{
		m_javaPath->setText(QString::fromStdString(appSettings->getJavaPath()));
	}

	m_jvmMaximumMemory->setText(QString::number(appSettings->getJavaMaximumMemory()));

	if (m_mavenPath)
	{
		m_mavenPath->setText(QString::fromStdString(appSettings->getMavenPath().str()));
	}
}

void QtProjectWizzardContentPreferences::save()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	appSettings->setFontName(m_fontFace->getText().toStdString());

	appSettings->setFontSize(m_fontSize->currentIndex() + appSettings->getFontSizeMin());
	appSettings->setCodeTabWidth(m_tabWidth->currentIndex() + 1);

	appSettings->setColorSchemePath(m_colorSchemePaths[m_colorSchemes->currentIndex()]);
	m_oldColorSchemeIndex = -1;

	appSettings->setUseAnimations(m_useAnimations->isChecked());

	appSettings->setLoggingEnabled(m_loggingEnabled->isChecked());
	appSettings->setVerboseIndexerLoggingEnabled(m_verboseIndexerLoggingEnabled->isChecked());

	float scrollSpeed = m_scrollSpeed->text().toFloat();
	if (scrollSpeed) appSettings->setScrollSpeed(scrollSpeed);

	appSettings->setControlsGraphZoomOnMouseWheel(m_graphZooming->isChecked());

	int sourcetrailPort = m_sourcetrailPort->text().toInt();
	if (sourcetrailPort) appSettings->setSourcetrailPort(sourcetrailPort);

	int pluginPort = m_pluginPort->text().toInt();
	if (pluginPort) appSettings->setPluginPort(pluginPort);

	appSettings->setIndexerThreadCount(m_threads->currentIndex()); // index and value are the same
	appSettings->setMultiProcessIndexingEnabled(m_multiProcessIndexing->isChecked());

	if (m_javaPath)
	{
		appSettings->setJavaPath(m_javaPath->getText().toStdString());
	}

	int jvmMaximumMemory = m_jvmMaximumMemory->text().toInt();
	if (jvmMaximumMemory) appSettings->setJavaMaximumMemory(jvmMaximumMemory);

	if (m_mavenPath)
	{
		appSettings->setMavenPath(FilePath(m_mavenPath->getText().toStdString()));
	}
}

bool QtProjectWizzardContentPreferences::check()
{
	return true;
}

void QtProjectWizzardContentPreferences::colorSchemeChanged(int index)
{
	m_newColorSchemeIndex = index;
	MessageSwitchColorScheme(m_colorSchemePaths[index]).dispatch();
}

void QtProjectWizzardContentPreferences::javaPathDetectionClicked()
{
	std::vector<FilePath> paths = m_javaPathDetector->getPaths(m_javaPathDetectorBox->currentText().toStdString());
	if (!paths.empty())
	{
		m_javaPath->setText(paths.front().str().c_str());
	}
}

void QtProjectWizzardContentPreferences::mavenPathDetectionClicked()
{
	std::vector<FilePath> paths = m_mavenPathDetector->getPaths(m_mavenPathDetectorBox->currentText().toStdString());
	if (!paths.empty())
	{
		m_mavenPath->setText(paths.front().str().c_str());
	}
}

void QtProjectWizzardContentPreferences::loggingEnabledChanged()
{
	m_verboseIndexerLoggingEnabled->setEnabled(m_loggingEnabled->isChecked());
}

void QtProjectWizzardContentPreferences::indexerThreadsChanges(int index)
{
	if (index == 0)
	{
		m_threadsInfoLabel->setText(("detected " + std::to_string(utility::getIdealThreadCount()) + " threads to be ideal.").c_str());
		m_threadsInfoLabel->show();
	}
	else
	{
		m_threadsInfoLabel->hide();
	}
}

void QtProjectWizzardContentPreferences::addJavaPathDetection(QGridLayout* layout, int& row)
{
	std::vector<std::string> detectorNames = m_javaPathDetector->getWorkingDetectorNames();
	if (detectorNames.empty())
	{
		return;
	}

	QLabel* label = new QLabel("Auto detection from:");

	m_javaPathDetectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_javaPathDetectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton("detect");
	button->setObjectName("windowButton");
	connect(button, SIGNAL(clicked()), this, SLOT(javaPathDetectionClicked()));

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_javaPathDetectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(detectionWidget, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
	row++;
}

void QtProjectWizzardContentPreferences::addMavenPathDetection(QGridLayout* layout, int& row)
{
	std::vector<std::string> detectorNames = m_mavenPathDetector->getWorkingDetectorNames();
	if (detectorNames.empty())
	{
		return;
	}

	QLabel* label = new QLabel("Auto detection from:");

	m_mavenPathDetectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_mavenPathDetectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton("detect");
	button->setObjectName("windowButton");
	connect(button, SIGNAL(clicked()), this, SLOT(mavenPathDetectionClicked()));

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_mavenPathDetectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(detectionWidget, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
	row++;
}

void QtProjectWizzardContentPreferences::addTitle(QString title, QGridLayout* layout, int& row)
{
	layout->addWidget(createFormTitle(title), row++, QtProjectWizzardWindow::FRONT_COL, Qt::AlignLeft);
}

void QtProjectWizzardContentPreferences::addLabelAndWidget(
	QString label, QWidget* widget, QGridLayout* layout, int& row, Qt::Alignment widgetAlignment)
{
	layout->addWidget(createFormLabel(label), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(widget, row, QtProjectWizzardWindow::BACK_COL, widgetAlignment);
}

void QtProjectWizzardContentPreferences::addGap(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 20);
}

QCheckBox* QtProjectWizzardContentPreferences::addCheckBox(
	QString label, QString text, QString help, QGridLayout* layout, int& row)
{
	QCheckBox* checkBox = new QCheckBox(text, this);
	addLabelAndWidget(label, checkBox, layout, row, Qt::AlignLeft);

	if (help.size())
	{
		addHelpButton(help, layout, row);
	}

	row++;

	return checkBox;
}

QComboBox* QtProjectWizzardContentPreferences::addComboBox(
	QString label, int min, int max, QString help, QGridLayout* layout, int& row)
{
	QComboBox* comboBox = new QComboBox(this);
	addLabelAndWidget(label, comboBox, layout, row, Qt::AlignLeft);

	if (min != max)
	{
		for (int i = min; i <= max; i++)
		{
			comboBox->insertItem(i, QString::number(i));
		}
	}

	if (help.size())
	{
		addHelpButton(help, layout, row);
	}

	row++;

	return comboBox;
}

QLineEdit* QtProjectWizzardContentPreferences::addLineEdit(QString label, QString help, QGridLayout* layout, int& row)
{
	QLineEdit* lineEdit = new QLineEdit(this);
	lineEdit->setObjectName("name");
	lineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);

	addLabelAndWidget(label, lineEdit, layout, row);

	if (help.size())
	{
		addHelpButton(help, layout, row);
	}

	row++;

	return lineEdit;
}
