#include "qt/window/project_wizzard/QtProjectWizzardContentCrossCompilationOptions.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include "settings/SourceGroupSettings.h"
#include "settings/SourceGroupSettingsCxx.h"
#include "utility/logging/logging.h"

QtProjectWizzardContentCrossCompilationOptions::QtProjectWizzardContentCrossCompilationOptions(
	std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_sourceGroupSettings(sourceGroupSettings)
	, m_useTargetOptions(nullptr)
	, m_arch(nullptr)
	, m_vendor(nullptr)
	, m_sys(nullptr)
	, m_abi(nullptr)
{
}

void QtProjectWizzardContentCrossCompilationOptions::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	layout->addWidget(createFormLabel("Cross-Compilation"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	addHelpButton(
		"<p>Use these options to specify the target architecture for the provided source code. Even though Sourcetrail will "
		"not generate a target binary, providing these options will affect which headers the indexer will be looking for "
		"while analyzing your source code.</p>"
		"<p>If you are not sure which value to pick for a certain option just choose \"unknown\" and Sourcetrail will try "
		"to guess the correct value.</p>",
		layout, row
	);

	{
		m_useTargetOptions = new QCheckBox("Use specific target");
		connect(m_useTargetOptions, &QCheckBox::stateChanged, this, &QtProjectWizzardContentCrossCompilationOptions::onUseTargetOptionsChanged);

		QHBoxLayout* rowLayout = new QHBoxLayout();
		rowLayout->setContentsMargins(0, 0, 0, 0);
		rowLayout->addWidget(m_useTargetOptions);

		QWidget* rowWidget = new QWidget();
		rowWidget->setLayout(rowLayout);

		layout->addWidget(rowWidget, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
		row++;
	}

	QGridLayout* gridLayout = new QGridLayout();

	{
		QLabel* label = new QLabel("Architecture:");

		m_arch = new QComboBox();
		std::vector<std::string> archTypes = SourceGroupSettingsCxx::getAvailableArchTypes();
		std::sort(archTypes.begin(), archTypes.end());
		for (size_t i = 0; i < archTypes.size(); i++)
		{
			m_arch->insertItem(i, archTypes[i].c_str());
		}
		m_arch->setCurrentIndex(m_arch->findText("x86_64"));

		gridLayout->addWidget(label, 0, 0, Qt::AlignRight);
		gridLayout->addWidget(m_arch, 0, 1, Qt::AlignLeft);
	}

	{
		QLabel* label = new QLabel("Vendor:");

		m_vendor = new QComboBox();
		std::vector<std::string> vendorTypes = SourceGroupSettingsCxx::getAvailableVendorTypes();
		std::sort(vendorTypes.begin() + 1, vendorTypes.end());
		for (size_t i = 0; i < vendorTypes.size(); i++)
		{
			m_vendor->insertItem(i, vendorTypes[i].c_str());
		}

		gridLayout->addWidget(label, 1, 0, Qt::AlignRight);
		gridLayout->addWidget(m_vendor, 1, 1, Qt::AlignLeft);
	}

	{
		QLabel* label = new QLabel("OS:");

		m_sys = new QComboBox();
		std::vector<std::string> osTypes = SourceGroupSettingsCxx::getAvailableOsTypes();
		std::sort(osTypes.begin() + 1, osTypes.end());
		for (size_t i = 0; i < osTypes.size(); i++)
		{
			m_sys->insertItem(i, osTypes[i].c_str());
		}

		gridLayout->addWidget(label, 2, 0, Qt::AlignRight);
		gridLayout->addWidget(m_sys, 2, 1, Qt::AlignLeft);
	}

	{
		QLabel* label = new QLabel("Environment:");

		m_abi = new QComboBox();
		std::vector<std::string> environmentTypes = SourceGroupSettingsCxx::getAvailableEnvironmentTypes();
		std::sort(environmentTypes.begin() + 1, environmentTypes.end());
		for (size_t i = 0; i < environmentTypes.size(); i++)
		{
			m_abi->insertItem(i, environmentTypes[i].c_str());
		}

		gridLayout->addWidget(label, 3, 0, Qt::AlignRight);
		gridLayout->addWidget(m_abi, 3, 1, Qt::AlignLeft);
	}

	layout->addLayout(gridLayout, row++, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentCrossCompilationOptions::load()
{
	if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_sourceGroupSettings))
	{
		m_useTargetOptions->setChecked(cxxSettings->getTargetOptionsEnabled());
		m_arch->setCurrentText(QString::fromStdString(cxxSettings->getTargetArch()));
		m_vendor->setCurrentText(QString::fromStdString(cxxSettings->getTargetVendor()));
		m_sys->setCurrentText(QString::fromStdString(cxxSettings->getTargetSys()));
		m_abi->setCurrentText(QString::fromStdString(cxxSettings->getTargetAbi()));
	}

	updateTargetOptionsEnabled();
}

void QtProjectWizzardContentCrossCompilationOptions::save()
{
	if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_sourceGroupSettings))
	{
		cxxSettings->setTargetOptionsEnabled(m_useTargetOptions->isChecked());
		cxxSettings->setTargetArch(m_arch->currentText().toStdString());
		cxxSettings->setTargetVendor(m_vendor->currentText().toStdString());
		cxxSettings->setTargetSys(m_sys->currentText().toStdString());
		cxxSettings->setTargetAbi(m_abi->currentText().toStdString());
	}
}

bool QtProjectWizzardContentCrossCompilationOptions::check()
{
	return true;
}

void QtProjectWizzardContentCrossCompilationOptions::onUseTargetOptionsChanged()
{
	updateTargetOptionsEnabled();
}

void QtProjectWizzardContentCrossCompilationOptions::updateTargetOptionsEnabled()
{
	const bool useTargetOptions = m_useTargetOptions->isChecked();
	m_arch->setEnabled(useTargetOptions);
	m_vendor->setEnabled(useTargetOptions);
	m_sys->setEnabled(useTargetOptions);
	m_abi->setEnabled(useTargetOptions);
}
