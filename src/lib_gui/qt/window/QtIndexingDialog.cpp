#include "qt/window/QtIndexingDialog.h"

#include <QCheckBox>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QPushButton>

#include "qt/utility/utilityQt.h"
#include "qt/element/QtHelpButton.h"
#include "qt/element/QtProgressBar.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/ResourcePaths.h"
#include "utility/utility.h"

QtIndexingDialog::QtIndexingDialog(QWidget* parent)
	: QtWindow(true, parent)
	, m_type(DIALOG_MESSAGE)
	, m_top(nullptr)
	, m_topRatio(0)
	, m_progressBar(nullptr)
	, m_percentLabel(nullptr)
	, m_messageLabel(nullptr)
	, m_filePathLabel(nullptr)
	, m_errorLabel(nullptr)
	, m_fullRefreshCheckBox(nullptr)
	, m_sizeHint(QSize(450, 450))
	, m_callback([](DialogView::IndexingOptions){})
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

void QtIndexingDialog::setupStart(
	size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount,
	DialogView::IndexingOptions options, std::function<void(DialogView::IndexingOptions)> callback)
{
	QBoxLayout* layout = createLayout();

	addTitle("Start Indexing", layout);
	layout->addSpacing(5);

	QLabel* clearLabel = createMessageLabel(layout);
	QLabel* indexLabel = createMessageLabel(layout);
	QLabel* fullLabel = createMessageLabel(layout);

	clearLabel->setText("Clear: " + QString::number(cleanFileCount) + " File" + (cleanFileCount != 1 ? "s" : ""));
	indexLabel->setText("Index: " + QString::number(indexFileCount) + " File" + (indexFileCount != 1 ? "s" : ""));
	fullLabel->setText("Index: " + QString::number(totalFileCount) + " File" + (totalFileCount != 1 ? "s" : ""));

	layout->addStretch();

	if (options.fullRefreshVisible)
	{
		m_fullRefreshCheckBox = new QCheckBox("full refresh", this);
		m_fullRefreshCheckBox->setObjectName("message");

		connect(m_fullRefreshCheckBox, &QCheckBox::toggled,
			[=](bool checked = false)
			{
				clearLabel->setVisible(!checked);
				indexLabel->setVisible(!checked);
				fullLabel->setVisible(checked);
			}
		);

		m_fullRefreshCheckBox->setChecked(!options.fullRefresh);
		m_fullRefreshCheckBox->setChecked(options.fullRefresh);

		QHBoxLayout* subLayout = new QHBoxLayout();
		subLayout->addStretch();
		subLayout->addWidget(m_fullRefreshCheckBox);

		layout->addLayout(subLayout);
	}
	else
	{
		clearLabel->hide();
		indexLabel->hide();
	}

	if (m_fullRefreshCheckBox)
	{
		layout->addSpacing(20);
	}

	addButtons(layout);
	updateNextButton("Start");
	updateCloseButton("Cancel");

	m_sizeHint = QSize(350, 270);
	m_callback = callback;

	finishSetup();
}

void QtIndexingDialog::setupIndexing()
{
	setType(DIALOG_INDEXING);

	QBoxLayout* layout = createLayout();

	addTopAndProgressBar(0.38);
	addTitle("Indexing Files", layout);

	addPercentLabel(layout);
	addMessageLabel(layout);
	addFilePathLabel(layout);

	layout->addSpacing(12);
	addErrorLabel(layout);

	layout->addStretch();

	addButtons(layout);
	setNextVisible(false);
	updateCloseButton("Stop");

	m_sizeHint = QSize(350, 350);

	finishSetup();
}

void QtIndexingDialog::setupReport(
	size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
	float time, bool interrupted)
{
	QBoxLayout* layout = createLayout();

	addTitle("Finished Indexing", layout);
	layout->addSpacing(5);

	createMessageLabel(layout)->setText(
		"Source Files indexed:   " + QString::number(indexedFileCount) + "/" + QString::number(totalIndexedFileCount)
	);

	createMessageLabel(layout)->setText(
		"Files completed:   " + QString::number(completedFileCount) + "/" + QString::number(totalFileCount)
	);

	layout->addSpacing(12);
	createMessageLabel(layout)->setText("Total Time:   " + QString::fromStdString(utility::timeToString(time)));

	layout->addSpacing(12);
	addErrorLabel(layout);

	layout->addStretch();

	addButtons(layout);
	updateNextButton("OK");
	setCloseVisible(false);

	m_sizeHint = QSize(400, 280);

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

void QtIndexingDialog::setupUnknownProgress()
{
	setType(DIALOG_UNKNOWN_PROGRESS);

	QBoxLayout* layout = createLayout();

	addTopAndProgressBar(0.5);
	addTitle("Status", layout);
	addMessageLabel(layout);

	layout->addStretch();

	m_sizeHint = QSize(350, 280);

	m_progressBar->showUnknownProgressAnimated();

	setCancelAble(false);

	finishSetup();
}

void QtIndexingDialog::setupProgress()
{
	setType(DIALOG_PROGRESS);

	QBoxLayout* layout = createLayout();

	addTopAndProgressBar(0.5);
	addTitle("Progress", layout);
	addPercentLabel(layout);
	addMessageLabel(layout);

	layout->addStretch();

	m_sizeHint = QSize(350, 280);

	setCancelAble(false);

	finishSetup();
}

void QtIndexingDialog::updateMessage(QString message)
{
	if (m_messageLabel)
	{
		m_messageLabel->setText(message);
	}
}

void QtIndexingDialog::updateProgress(int progress)
{
	int percent = std::min(std::max(progress, 0), 100);

	m_progressBar->showProgress(percent);
	m_percentLabel->setText(QString::number(percent) + "% Progress");
	setGeometries();
}

void QtIndexingDialog::updateIndexingProgress(size_t fileCount, size_t totalFileCount, std::string sourcePath)
{
	updateMessage(QString::number(fileCount) + "/" + QString::number(totalFileCount) + " File" + (totalFileCount > 1 ? "s" : ""));

	int progress = 0;
	if (totalFileCount > 0)
	{
		progress = fileCount * 100 / totalFileCount;
	}

	if (sourcePath.size())
	{
		m_sourcePath = QString::fromStdString(sourcePath);
	}

	updateProgress(progress);
}

void QtIndexingDialog::updateErrorCount(size_t errorCount, size_t fatalCount)
{
	if (m_errorLabel && errorCount)
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

		m_errorLabel->setText(str);
		m_errorLabel->show();
	}
}

void QtIndexingDialog::resizeEvent(QResizeEvent* event)
{
	QtWindow::resizeEvent(event);

	setGeometries();
}

void QtIndexingDialog::handleNext()
{
	if (m_type == DIALOG_MESSAGE)
	{
		DialogView::IndexingOptions options;
		options.startIndexing = true;
		options.fullRefresh = m_fullRefreshCheckBox && m_fullRefreshCheckBox->isChecked();
		m_callback(options);
	}

	QtWindow::handleNext();
}

void QtIndexingDialog::handleClose()
{
	if (m_type == DIALOG_MESSAGE)
	{
		DialogView::IndexingOptions options;
		m_callback(options);
	}

	if (m_type == DIALOG_INDEXING)
	{
		MessageInterruptTasks().dispatch();
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
		utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("window/window.css"))) +
		utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("indexing_dialog/indexing_dialog.css")))
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
	else
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

void QtIndexingDialog::addErrorLabel(QBoxLayout* layout)
{
	m_errorLabel = new QPushButton();
	m_errorLabel->setObjectName("errorCount");
	m_errorLabel->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	std::string text = ResourcePaths::getGuiPath().str() + "indexing_dialog/error.png";
	m_errorLabel->setIcon(QPixmap(text.c_str()));

	layout->addWidget(m_errorLabel, 0, Qt::AlignRight);
	m_errorLabel->hide();
}

void QtIndexingDialog::addButtons(QBoxLayout* layout)
{
	m_nextButton = new QPushButton("Next");
	m_nextButton->setObjectName("windowButton");
	connect(m_nextButton, &QPushButton::clicked, this, &QtIndexingDialog::handleNextPress);

	m_closeButton = new QPushButton("Cancel");
	m_closeButton->setObjectName("windowButton");
	connect(m_closeButton, &QPushButton::clicked, this, &QtIndexingDialog::handleClosePress);

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(m_closeButton);
	buttons->addStretch();
	buttons->addWidget(m_nextButton);

	layout->addLayout(buttons);

	setNextDefault(true);
}

void QtIndexingDialog::addFlag()
{
	QtDeviceScaledPixmap flag((ResourcePaths::getGuiPath().str() + "indexing_dialog/flag.png").c_str());
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
			margins.left(),
			m_window->size().height() * m_topRatio - 50,
			m_window->size().width() - margins.left() - margins.right(),
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
