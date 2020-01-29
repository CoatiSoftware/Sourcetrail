#include "QtIndexingProgressDialog.h"

#include <QLabel>
#include <QPushButton>

#include "MessageIndexingInterrupted.h"

QtIndexingProgressDialog::QtIndexingProgressDialog(bool hideable, QWidget* parent)
	: QtProgressBarDialog(0.38, true, parent), m_filePathLabel(nullptr), m_errorWidget(nullptr)
{
	setSizeGripStyle(false);

	updateTitle(QStringLiteral("Indexing Files"));

	m_filePathLabel = new QLabel();
	m_filePathLabel->setObjectName(QStringLiteral("filePath"));
	m_filePathLabel->setAlignment(Qt::AlignRight);
	m_layout->addWidget(m_filePathLabel);

	m_layout->addSpacing(12);
	m_errorWidget = QtIndexingDialog::createErrorWidget(m_layout);

	m_layout->addStretch();

	{
		QHBoxLayout* buttons = new QHBoxLayout();

		QPushButton* stopButton = new QPushButton(QStringLiteral("Stop"));
		stopButton->setObjectName(QStringLiteral("windowButton"));
		connect(stopButton, &QPushButton::clicked, this, &QtIndexingProgressDialog::onStopPressed);
		buttons->addWidget(stopButton);

		buttons->addStretch();

		QPushButton* hideButton = new QPushButton(QStringLiteral("Hide"));
		hideButton->setObjectName(QStringLiteral("windowButton"));
		hideButton->setDefault(true);
		connect(hideButton, &QPushButton::clicked, this, &QtIndexingProgressDialog::onHidePressed);
		buttons->addWidget(hideButton);

		m_layout->addLayout(buttons);
	}

	setupDone();
	setGeometries();
}

QSize QtIndexingProgressDialog::sizeHint() const
{
	return QSize(350, 350);
}

void QtIndexingProgressDialog::updateIndexingProgress(
	size_t fileCount, size_t totalFileCount, const FilePath& sourcePath)
{
	updateMessage(
		QString::number(fileCount) + "/" + QString::number(totalFileCount) + " File" +
		(totalFileCount > 1 ? "s" : ""));

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

void QtIndexingProgressDialog::updateErrorCount(size_t errorCount, size_t fatalCount)
{
	if (m_errorWidget && errorCount)
	{
		QString str = QString::number(errorCount) + " Error";
		if (errorCount > 1)
		{
			str += QLatin1String("s");
		}

		if (fatalCount)
		{
			str += " (" + QString::number(fatalCount) + " Fatal)";
		}

		QPushButton* errorCount = m_errorWidget->findChild<QPushButton*>(QStringLiteral("errorCount"));
		errorCount->setText(str);

		m_errorWidget->show();
	}
}

void QtIndexingProgressDialog::onHidePressed()
{
	emit visibleChanged(false);
}

void QtIndexingProgressDialog::onStopPressed()
{
	if (isVisible())
	{
		MessageIndexingInterrupted().dispatch();
	}
}

void QtIndexingProgressDialog::closeEvent(QCloseEvent* event)
{
	emit visibleChanged(false);
}

void QtIndexingProgressDialog::setGeometries()
{
	QtProgressBarDialog::setGeometries();

	if (m_filePathLabel)
	{
		m_filePathLabel->setText(m_filePathLabel->fontMetrics().elidedText(
			m_sourcePath, Qt::ElideLeft, m_filePathLabel->width()));
	}
}
