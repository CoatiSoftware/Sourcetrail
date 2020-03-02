#include "QtProjectWizardContentCrossCompilationOptions.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include "SourceGroupSettingsWithCxxCrossCompilationOptions.h"
#include "logging.h"

QtProjectWizardContentCrossCompilationOptions::QtProjectWizardContentCrossCompilationOptions(
	std::shared_ptr<SourceGroupSettingsWithCxxCrossCompilationOptions> sourceGroupSettings,
	QtProjectWizardWindow* window)
	: QtProjectWizardContent(window)
	, m_sourceGroupSettings(sourceGroupSettings)
	, m_useTargetOptions(nullptr)
	, m_arch(nullptr)
	, m_vendor(nullptr)
	, m_sys(nullptr)
	, m_abi(nullptr)
{
}

void QtProjectWizardContentCrossCompilationOptions::populate(QGridLayout* layout, int& row)
{
	layout->addWidget(
		createFormLabel("Cross-Compilation"), row, QtProjectWizardWindow::FRONT_COL, Qt::AlignRight);
	addHelpButton(
		"Cross-Compilation",
		QStringLiteral(
			"<p>Use these options to specify the target architecture for the provided source code. "
			"Even though Sourcetrail will "
			"not generate a target binary, providing these options will affect which headers the "
			"indexer will be looking for "
			"while analyzing your source code.</p>"
			"<p>If you are not sure which value to pick for a certain option just choose "
			"\"unknown\" "
			"and Sourcetrail will try "
			"to guess the correct value.</p>"),
		layout,
		row);

	{
		m_useTargetOptions = new QCheckBox("Use specific target");
		connect(
			m_useTargetOptions,
			&QCheckBox::stateChanged,
			this,
			&QtProjectWizardContentCrossCompilationOptions::onUseTargetOptionsChanged);

		QHBoxLayout* rowLayout = new QHBoxLayout();
		rowLayout->setContentsMargins(0, 0, 0, 0);
		rowLayout->addWidget(m_useTargetOptions);

		QWidget* rowWidget = new QWidget();
		rowWidget->setLayout(rowLayout);

		layout->addWidget(
			rowWidget, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
		row++;
	}

	QGridLayout* gridLayout = new QGridLayout();

	{
		QLabel* label = new QLabel("Architecture:");

		m_arch = new QComboBox();
		std::vector<std::wstring> archTypes =
			SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableArchTypes();
		std::sort(archTypes.begin(), archTypes.end());
		for (size_t i = 0; i < archTypes.size(); i++)
		{
			m_arch->insertItem(static_cast<int>(i), QString::fromStdWString(archTypes[i]));
		}
		m_arch->setCurrentIndex(m_arch->findText("x86_64"));

		gridLayout->addWidget(label, 0, 0, Qt::AlignRight);
		gridLayout->addWidget(m_arch, 0, 1, Qt::AlignLeft);
	}

	{
		QLabel* label = new QLabel("Vendor:");

		m_vendor = new QComboBox();
		std::vector<std::wstring> vendorTypes =
			SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableVendorTypes();
		std::sort(vendorTypes.begin() + 1, vendorTypes.end());
		for (size_t i = 0; i < vendorTypes.size(); i++)
		{
			m_vendor->insertItem(static_cast<int>(i), QString::fromStdWString(vendorTypes[i]));
		}

		gridLayout->addWidget(label, 1, 0, Qt::AlignRight);
		gridLayout->addWidget(m_vendor, 1, 1, Qt::AlignLeft);
	}

	{
		QLabel* label = new QLabel("OS:");

		m_sys = new QComboBox();
		std::vector<std::wstring> osTypes =
			SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableOsTypes();
		std::sort(osTypes.begin() + 1, osTypes.end());
		for (size_t i = 0; i < osTypes.size(); i++)
		{
			m_sys->insertItem(static_cast<int>(i), QString::fromStdWString(osTypes[i]));
		}

		gridLayout->addWidget(label, 2, 0, Qt::AlignRight);
		gridLayout->addWidget(m_sys, 2, 1, Qt::AlignLeft);
	}

	{
		QLabel* label = new QLabel("Environment:");

		m_abi = new QComboBox();
		std::vector<std::wstring> environmentTypes =
			SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableEnvironmentTypes();
		std::sort(environmentTypes.begin() + 1, environmentTypes.end());
		for (size_t i = 0; i < environmentTypes.size(); i++)
		{
			m_abi->insertItem(static_cast<int>(i), QString::fromStdWString(environmentTypes[i]));
		}

		gridLayout->addWidget(label, 3, 0, Qt::AlignRight);
		gridLayout->addWidget(m_abi, 3, 1, Qt::AlignLeft);
	}

	layout->addLayout(
		gridLayout, row++, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
}

void QtProjectWizardContentCrossCompilationOptions::load()
{
	m_useTargetOptions->setChecked(m_sourceGroupSettings->getTargetOptionsEnabled());
	m_arch->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getTargetArch()));
	m_vendor->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getTargetVendor()));
	m_sys->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getTargetSys()));
	m_abi->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getTargetAbi()));

	updateTargetOptionsEnabled();
}

void QtProjectWizardContentCrossCompilationOptions::save()
{
	m_sourceGroupSettings->setTargetOptionsEnabled(m_useTargetOptions->isChecked());
	m_sourceGroupSettings->setTargetArch(m_arch->currentText().toStdWString());
	m_sourceGroupSettings->setTargetVendor(m_vendor->currentText().toStdWString());
	m_sourceGroupSettings->setTargetSys(m_sys->currentText().toStdWString());
	m_sourceGroupSettings->setTargetAbi(m_abi->currentText().toStdWString());
}

bool QtProjectWizardContentCrossCompilationOptions::check()
{
	return true;
}

void QtProjectWizardContentCrossCompilationOptions::onUseTargetOptionsChanged()
{
	updateTargetOptionsEnabled();
}

void QtProjectWizardContentCrossCompilationOptions::updateTargetOptionsEnabled()
{
	const bool useTargetOptions = m_useTargetOptions->isChecked();
	m_arch->setEnabled(useTargetOptions);
	m_vendor->setEnabled(useTargetOptions);
	m_sys->setEnabled(useTargetOptions);
	m_abi->setEnabled(useTargetOptions);
}
