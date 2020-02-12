#include "QtProjectWizardContent.h"

#include <thread>

#include "QtTextEditDialog.h"
#include "utility.h"
#include "utilityString.h"

QtProjectWizardContent::QtProjectWizardContent(QtProjectWizardWindow* window)
	: QWidget(window)
	, m_window(window)
	, m_showFilesFunctor(
		  std::bind(&QtProjectWizardContent::showFilesDialog, this, std::placeholders::_1))
{
}

void QtProjectWizardContent::populate(QGridLayout* layout, int& row) {}

void QtProjectWizardContent::windowReady() {}

void QtProjectWizardContent::load() {}

void QtProjectWizardContent::save() {}

void QtProjectWizardContent::refresh() {}

bool QtProjectWizardContent::check()
{
	return true;
}

std::vector<FilePath> QtProjectWizardContent::getFilePaths() const
{
	return {};
}

QString QtProjectWizardContent::getFileNamesTitle() const
{
	return QStringLiteral("File List");
}

QString QtProjectWizardContent::getFileNamesDescription() const
{
	return QStringLiteral("files");
}

bool QtProjectWizardContent::isRequired() const
{
	return m_isRequired;
}

void QtProjectWizardContent::setIsRequired(bool isRequired)
{
	m_isRequired = isRequired;
}

QLabel* QtProjectWizardContent::createFormTitle(const QString& name) const
{
	QLabel* label = new QLabel(name);
	label->setObjectName(QStringLiteral("titleLabel"));
	label->setWordWrap(true);
	return label;
}

QLabel* QtProjectWizardContent::createFormLabel(QString name) const
{
	if (m_isRequired)
	{
		name += QStringLiteral("*");
	}

	return createFormSubLabel(name);
}

QLabel* QtProjectWizardContent::createFormSubLabel(const QString& name) const
{
	QLabel* label = new QLabel(name);
	label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	label->setObjectName(QStringLiteral("label"));
	label->setWordWrap(true);
	return label;
}

QToolButton* QtProjectWizardContent::createSourceGroupButton(
	const QString& name, const QString& iconPath) const
{
	QToolButton* button = new QToolButton();
	button->setObjectName(QStringLiteral("sourceGroupButton"));
	button->setText(name);
	button->setIcon(QPixmap(iconPath));
	button->setIconSize(QSize(64, 64));
	button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	button->setCheckable(true);
	return button;
}

QtHelpButton* QtProjectWizardContent::addHelpButton(
	const QString& helpTitle, const QString& helpText, QGridLayout* layout, int row) const
{
	QtHelpButton* button = new QtHelpButton(QtHelpButtonInfo(helpTitle, helpText));
	button->setMessageBoxParent(m_window);
	layout->addWidget(button, row, QtProjectWizardWindow::HELP_COL, Qt::AlignTop);
	return button;
}

QPushButton* QtProjectWizardContent::addFilesButton(const QString& name, QGridLayout* layout, int row) const
{
	QPushButton* button = new QPushButton(name);
	button->setObjectName(QStringLiteral("windowButton"));
	button->setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
	if (layout)
	{
		layout->addWidget(
			button, row, QtProjectWizardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
	}
	connect(button, &QPushButton::clicked, this, &QtProjectWizardContent::filesButtonClicked);

	return button;
}

QFrame* QtProjectWizardContent::addSeparator(QGridLayout* layout, int row) const
{
	QFrame* separator = new QFrame();
	separator->setFrameShape(QFrame::HLine);

	QPalette palette = separator->palette();
	palette.setColor(QPalette::WindowText, Qt::lightGray);
	separator->setPalette(palette);

	layout->addWidget(separator, row, 0, 1, -1);
	return separator;
}

void QtProjectWizardContent::filesButtonClicked()
{
	m_window->saveContent();
	m_window->refreshContent();

	std::thread([&]() {
		const std::vector<FilePath> filePaths = getFilePaths();
		m_showFilesFunctor(filePaths);
	}).detach();
}

void QtProjectWizardContent::showFilesDialog(const std::vector<FilePath>& filePaths)
{
	if (!m_filesDialog)
	{
		m_filesDialog = new QtTextEditDialog(
			getFileNamesTitle(), QString::number(filePaths.size()) + " " + getFileNamesDescription(), m_window);
		m_filesDialog->setup();

		m_filesDialog->setText(utility::join(utility::toWStrings(filePaths), L"\n"));
		m_filesDialog->setCloseVisible(false);
		m_filesDialog->setReadOnly(true);

		connect(
			m_filesDialog,
			&QtTextEditDialog::finished,
			this,
			&QtProjectWizardContent::closedFilesDialog);
		connect(
			m_filesDialog,
			&QtTextEditDialog::canceled,
			this,
			&QtProjectWizardContent::closedFilesDialog);
	}

	m_filesDialog->showWindow();
	m_filesDialog->raise();
}

void QtProjectWizardContent::closedFilesDialog()
{
	m_filesDialog->hide();
	m_filesDialog->deleteLater();
	m_filesDialog = nullptr;

	window()->raise();
}
