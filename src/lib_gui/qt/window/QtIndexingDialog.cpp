#include "QtIndexingDialog.h"

#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>

#include "utilityQt.h"
#include "QtHelpButton.h"
#include "QtProgressBar.h"
#include "MessageErrorsHelpMessage.h"
#include "MessageIndexingInterrupted.h"
#include "ResourcePaths.h"
#include "utility.h"

QtIndexingDialog::QtIndexingDialog(QWidget* parent)
	: QtWindow(true, parent)
	, m_type(DIALOG_MESSAGE)
	, m_top(nullptr)
	, m_topRatio(0)
	, m_progressBar(nullptr)
	, m_percentLabel(nullptr)
	, m_messageLabel(nullptr)
	, m_filePathLabel(nullptr)
	, m_errorWidget(nullptr)
	, m_sizeHint(QSize(450, 450))
{
	setSizeGripStyle(false);
}

QSize QtIndexingDialog::sizeHint() const
{
	return m_sizeHint;
}

QtIndexingDialog::DialogType QtIndexingDialog::getType() const
{
	return m_type;
}

void QtIndexingDialog::setupStart(const std::vector<RefreshMode>& enabledModes, const RefreshMode initialMode)
{
	setType(DIALOG_START_INDEXING);

	QBoxLayout* layout = createLayout();

	addTitle("Start Indexing", layout);
	layout->addSpacing(5);

	m_clearLabel = createMessageLabel(layout);
	m_indexLabel = createMessageLabel(layout);

	m_clearLabel->setVisible(false);
	m_indexLabel->setVisible(false);

	layout->addStretch();

	QHBoxLayout* subLayout = new QHBoxLayout();
	subLayout->addStretch();

	QVBoxLayout* modeLayout = new QVBoxLayout();
	modeLayout->setSpacing(7);

	QHBoxLayout* modeTitleLayout = new QHBoxLayout();
	modeTitleLayout->setSpacing(7);

	QLabel* modeLabel = createMessageLabel(modeTitleLayout);
	modeLabel->setText("Mode:");
	modeLabel->setAlignment(Qt::AlignLeft);

	QtHelpButton* helpButton = new QtHelpButton(
		"Indexing Modes",
		"<b>Updated files:</b> Reindexes all files that were modified since the last indexing, all files depending "
			"on those and new files.<br /><br />"
		"<b>Incomplete & updated files:</b> Reindexes all files that had errors during last indexing, all files "
			"depending on those and all updated files.<br /><br />"
		"<b>All files:</b> Deletes the previous index and reindexes all files.<br /><br />"
	);
	helpButton->setColor(Qt::white);
	modeTitleLayout->addWidget(helpButton);

	modeTitleLayout->addStretch();

	modeLayout->addLayout(modeTitleLayout);
	modeLayout->addSpacing(5);

	m_refreshModeButtons.emplace(REFRESH_UPDATED_FILES, new QRadioButton("Updated files"));
	m_refreshModeButtons.emplace(REFRESH_UPDATED_AND_INCOMPLETE_FILES, new QRadioButton("Incomplete && updated files"));
	m_refreshModeButtons.emplace(REFRESH_ALL_FILES, new QRadioButton("All files"));

	std::function<void(bool)> func =
		[=](bool checked)
		{
			if (!checked)
			{
				return;
			}

			for (auto p : m_refreshModeButtons)
			{
				if (p.second->isChecked())
				{
					emit setMode(p.first);
					return;
				}
			}
		};

	for (auto p : m_refreshModeButtons)
	{
		QRadioButton* button = p.second;
		button->setObjectName("option");
		button->setEnabled(false);
		if (p.first == initialMode)
		{
			button->setChecked(true);
		}
		modeLayout->addWidget(button);
		connect(button, &QRadioButton::toggled, func);
	}

	for (RefreshMode mode : enabledModes)
	{
		m_refreshModeButtons[mode]->setEnabled(true);
	}

	subLayout->addLayout(modeLayout);
	layout->addLayout(subLayout);

	layout->addSpacing(20);

	addButtons(layout);
	updateNextButton("Start");
	updateCloseButton("Cancel");

	m_sizeHint = QSize(350, 310);

	finishSetup();

	emit setMode(initialMode);
}

void QtIndexingDialog::updateRefreshInfo(const RefreshInfo& info)
{
	QRadioButton* button = m_refreshModeButtons.find(info.mode)->second;
	if (!button->isChecked())
	{
		button->setChecked(true);
	}

	size_t clearCount = info.filesToClear.size();
	size_t indexCount = info.filesToIndex.size();

	m_clearLabel->setText("Files to clear: " + QString::number(clearCount));
	m_indexLabel->setText("Source files to index: " + QString::number(indexCount));

	m_clearLabel->setVisible(clearCount && info.mode != REFRESH_ALL_FILES);
	m_indexLabel->setVisible(true);
}

void QtIndexingDialog::setupIndexing(bool hideable)
{
	setType(DIALOG_INDEXING);

	QBoxLayout* layout = createLayout();

	addTopAndProgressBar(0.38);
	addTitle("Indexing Files", layout);

	addPercentLabel(layout);
	addMessageLabel(layout);
	addFilePathLabel(layout);

	layout->addSpacing(12);
	addErrorWidget(layout);

	layout->addStretch();

	addButtons(layout);
	updateNextButton("Hide");
	setNextVisible(hideable);
	updateCloseButton("Stop");

	m_sizeHint = QSize(350, 350);

	finishSetup();
}

void QtIndexingDialog::setupReport(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, bool interrupted)
{
	setType(DIALOG_REPORT);

	QBoxLayout* layout = createLayout();

	addTitle("Finished Indexing", layout);
	layout->addSpacing(5);

	createMessageLabel(layout)->setText(
		"Source files indexed:   " + QString::number(indexedFileCount) + "/" + QString::number(totalIndexedFileCount)
	);

	createMessageLabel(layout)->setText(
		"Total files completed:   " + QString::number(completedFileCount) + "/" + QString::number(totalFileCount)
	);

	layout->addSpacing(12);
	createMessageLabel(layout)->setText("Time:   " + QString::fromStdString(utility::timeToString(time)));

	layout->addSpacing(12);
	addErrorWidget(layout);

	layout->addStretch();

	addButtons(layout);
	if (interrupted)
	{
		updateNextButton("Keep");
		updateCloseButton("Discard");
	}
	else
	{
		updateNextButton("OK");
		setCloseVisible(false);
	}

	m_sizeHint = QSize(interrupted ? 400 : 430, 280);

	if (interrupted)
	{
		updateTitle("Interrupted Indexing");
	}
	else
	{
		addFlag();
	}

	finishSetup();
}

void QtIndexingDialog::setupUnknownProgress(bool hideable)
{
	setType(DIALOG_UNKNOWN_PROGRESS);

	QBoxLayout* layout = createLayout();

	addTopAndProgressBar(0.5);
	addTitle("Status", layout);
	addMessageLabel(layout);

	layout->addStretch();

	if (hideable)
	{
		addButtons(layout);
		updateNextButton("Hide");
		setCloseVisible(false);
	}

	m_sizeHint = QSize(350, 280);

	m_progressBar->showUnknownProgressAnimated();

	setCancelAble(false);

	finishSetup();
}

void QtIndexingDialog::setupProgress(bool hideable)
{
	setType(DIALOG_PROGRESS);

	QBoxLayout* layout = createLayout();

	addTopAndProgressBar(0.5);
	addTitle("Progress", layout);
	addPercentLabel(layout);
	addMessageLabel(layout);

	layout->addStretch();

	if (hideable)
	{
		addButtons(layout);
		updateNextButton("Hide");
		setCloseVisible(false);
	}

	m_sizeHint = QSize(350, 280);

	setCancelAble(false);

	finishSetup();
}

void QtIndexingDialog::updateMessage(const QString& message)
{
	if (m_messageLabel)
	{
		m_messageLabel->setText(message);
	}
}

std::wstring QtIndexingDialog::getMessage() const
{
	if (m_messageLabel)
	{
		return m_messageLabel->text().toStdWString();
	}
	return L"";
}

void QtIndexingDialog::updateProgress(size_t progress)
{
	size_t percent = std::min<size_t>(std::max<size_t>(progress, 0), 100);

	m_progressBar->showProgress(percent);
	m_percentLabel->setText(QString::number(percent) + "% Progress");
	setGeometries();
}

size_t QtIndexingDialog::getProgress() const
{
	return m_progressBar->getProgress();
}

void QtIndexingDialog::updateIndexingProgress(size_t fileCount, size_t totalFileCount, const FilePath& sourcePath)
{
	updateMessage(QString::number(fileCount) + "/" + QString::number(totalFileCount) + " File" + (totalFileCount > 1 ? "s" : ""));

	int progress = 0;
	if (totalFileCount > 0)
	{
		progress = fileCount * 100 / totalFileCount;
	}

	if (!sourcePath.empty())
	{
		m_sourcePath = QString::fromStdWString(sourcePath.wstr());
	}

	updateProgress(progress);
}

void QtIndexingDialog::updateErrorCount(size_t errorCount, size_t fatalCount)
{
	if (m_errorWidget && errorCount)
	{
		QString str = QString::number(errorCount) + " Error";
		if (errorCount > 1)
		{
			str += "s";
		}

		if (fatalCount)
		{
			str += " (" + QString::number(fatalCount) + " Fatal)";
		}

		QPushButton* errorCount = m_errorWidget->findChild<QPushButton*>("errorCount");
		errorCount->setText(str);

		m_errorWidget->show();
	}
}

void QtIndexingDialog::resizeEvent(QResizeEvent* event)
{
	QtWindow::resizeEvent(event);

	setGeometries();
}

void QtIndexingDialog::handleNext()
{
	if (m_type == DIALOG_START_INDEXING)
	{
		for (auto p : m_refreshModeButtons)
		{
			if (p.second->isChecked())
			{
				emit startIndexing(p.first);
				return;
			}
		}
	}

	if (m_type == DIALOG_INDEXING || m_type == DIALOG_PROGRESS || m_type == DIALOG_UNKNOWN_PROGRESS)
	{
		emit visibleChanged(false);
		return;
	}

	if (m_type == DIALOG_REPORT)
	{
		MessageErrorsHelpMessage().dispatch();
	}

	QtWindow::handleNext();
}

void QtIndexingDialog::handleClose()
{
	if (m_type == DIALOG_INDEXING || m_type == DIALOG_PROGRESS || m_type == DIALOG_UNKNOWN_PROGRESS)
	{
		emit visibleChanged(false);
		return;
	}

	QtWindow::handleClose();
}

void QtIndexingDialog::handleCancelPress()
{
	if (m_type == DIALOG_INDEXING)
	{
		if (isVisible())
		{
			MessageIndexingInterrupted().dispatch();
		}
		return;
	}

	QtWindow::handleClose();
}

void QtIndexingDialog::setType(DialogType type)
{
	m_type = type;
}

QBoxLayout* QtIndexingDialog::createLayout()
{
	m_window->setStyleSheet(
		m_window->styleSheet() +
		"#window { "
			"background: #2E3C86;"
			"border: none;"
		"}"
	);

	setStyleSheet((
		utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")) +
		utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/indexing_dialog.css"))
	).c_str());

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(20, 20, 20, 0);
	layout->setSpacing(3);

	m_content->setLayout(layout);

	return layout;
}

void QtIndexingDialog::addTopAndProgressBar(float topRatio)
{
	m_topRatio = topRatio;

	m_top = new QWidget(m_window);
	m_top->setObjectName("topHalf");
	m_top->setGeometry(0, 0, 0, 0);
	m_top->show();
	m_top->lower();

	m_progressBar = new QtProgressBar(m_window);
	m_progressBar->setGeometry(0, 0, 0, 0);
}

void QtIndexingDialog::addTitle(QString title, QBoxLayout* layout)
{
	m_title = new QLabel(title, this);
	m_title->setObjectName("title");
	m_title->setAlignment(Qt::AlignRight | Qt::AlignBottom);

	if (m_top)
	{
		m_title->show();
	}
	else if (layout)
	{
		layout->addWidget(m_title, 0, Qt::AlignRight);
	}
}

void QtIndexingDialog::addPercentLabel(QBoxLayout* layout)
{
	m_percentLabel = new QLabel("0% Progress");
	m_percentLabel->setObjectName("percent");
	layout->addWidget(m_percentLabel, 0, Qt::AlignRight);
}

void QtIndexingDialog::addMessageLabel(QBoxLayout* layout)
{
	m_messageLabel = createMessageLabel(layout);
}

QLabel* QtIndexingDialog::createMessageLabel(QBoxLayout* layout)
{
	QLabel* label = new QLabel();
	label->setObjectName("message");
	label->setAlignment(Qt::AlignRight);
	label->setWordWrap(true);
	layout->addWidget(label);
	return label;
}

void QtIndexingDialog::addFilePathLabel(QBoxLayout* layout)
{
	m_filePathLabel = new QLabel();
	m_filePathLabel->setObjectName("filePath");
	m_filePathLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(m_filePathLabel);
}

void QtIndexingDialog::addErrorWidget(QBoxLayout* layout)
{
	m_errorWidget = new QWidget();
	QHBoxLayout* errorLayout = new QHBoxLayout(m_errorWidget);
	errorLayout->setContentsMargins(0, 0, 0, 0);
	errorLayout->setSpacing(5);

	QPushButton* errorCount = new QPushButton();
	errorCount->setObjectName("errorCount");
	errorCount->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	errorCount->setIcon(QPixmap(QString::fromStdWString(
		ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/error.png").wstr())));
	errorLayout->addWidget(errorCount);

	QtHelpButton* helpButton = new QtHelpButton("aaa", "bbb");
	helpButton->setColor(Qt::white);

	helpButton->disconnect();
	connect(helpButton, &QtHelpButton::clicked,
		[]()
		{
			MessageErrorsHelpMessage(true).dispatch();
		}
	);

	errorLayout->addWidget(helpButton);

	layout->addWidget(m_errorWidget, 0, Qt::AlignRight);
	m_errorWidget->hide();
}

void QtIndexingDialog::addButtons(QBoxLayout* layout)
{
	m_nextButton = new QPushButton("Next");
	m_nextButton->setObjectName("windowButton");
	connect(m_nextButton, &QPushButton::clicked, this, &QtIndexingDialog::handleNextPress);

	m_closeButton = new QPushButton("Cancel");
	m_closeButton->setObjectName("windowButton");
	connect(m_closeButton, &QPushButton::clicked, this, &QtIndexingDialog::handleCancelPress);

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(m_closeButton);
	buttons->addStretch();
	buttons->addWidget(m_nextButton);

	layout->addLayout(buttons);

	setNextDefault(true);
}

void QtIndexingDialog::addFlag()
{
	QtDeviceScaledPixmap flag(QString::fromStdWString(
		ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/flag.png").wstr()));
	flag.scaleToWidth(120);

	QLabel* flagLabel = new QLabel(this);
	flagLabel->setPixmap(flag.pixmap());
	flagLabel->resize(flag.width(), flag.height());
	flagLabel->move(15, 75);
	flagLabel->show();
}

void QtIndexingDialog::setGeometries()
{
	if (m_top)
	{
		QMargins margins = m_content->layout()->contentsMargins();
		margins.setTop(m_window->size().height() * m_topRatio + 10);
		m_content->layout()->setContentsMargins(margins);

		m_top->setGeometry(0, 0, m_window->size().width(), m_window->size().height() * m_topRatio);

		m_title->setGeometry(
			45,
			m_window->size().height() * m_topRatio - 50,
			m_window->size().width() - 60,
			40
		);
	}

	if (m_progressBar)
	{
		m_progressBar->setGeometry(0, m_window->size().height() * m_topRatio - 5, m_window->size().width(), 10);
	}

	if (m_filePathLabel)
	{
		m_filePathLabel->setText(m_filePathLabel->fontMetrics().elidedText(
			m_sourcePath, Qt::ElideLeft, m_filePathLabel->width()));
	}
}

void QtIndexingDialog::finishSetup()
{
	setupDone();

	setGeometries();
}
