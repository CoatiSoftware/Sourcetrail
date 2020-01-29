#include "QtIndexingReportDialog.h"

#include <QLabel>
#include <QPushButton>

#include "MessageErrorsHelpMessage.h"
#include "MessageIndexingShowDialog.h"
#include "MessageRefresh.h"
#include "TimeStamp.h"

QtIndexingReportDialog::QtIndexingReportDialog(
	size_t indexedFileCount,
	size_t totalIndexedFileCount,
	size_t completedFileCount,
	size_t totalFileCount,
	float time,
	bool interrupted,
	bool shallow,
	QWidget* parent)
	: QtIndexingDialog(true, parent), m_interrupted(interrupted)
{
	setSizeGripStyle(false);

	if (interrupted)
	{
		QtIndexingDialog::createTitleLabel(QStringLiteral("Interrupted Indexing"), m_layout);
	}
	else if (shallow)
	{
		QtIndexingDialog::createTitleLabel(QStringLiteral("Finished Shallow Indexing"), m_layout);
	}
	else
	{
		QtIndexingDialog::createTitleLabel(QStringLiteral("Finished Indexing"), m_layout);
	}

	m_layout->addSpacing(5);

	QtIndexingDialog::createMessageLabel(m_layout)->setText(
		QStringLiteral("Source files indexed:   ") + QString::number(indexedFileCount) + "/" +
		QString::number(totalIndexedFileCount));

	QtIndexingDialog::createMessageLabel(m_layout)->setText(
		QStringLiteral("Total files completed:   ") + QString::number(completedFileCount) + "/" +
		QString::number(totalFileCount));

	m_layout->addSpacing(12);
	QtIndexingDialog::createMessageLabel(m_layout)->setText(
		QStringLiteral("Time:   ") + QString::fromStdString(TimeStamp::secondsToString(time)));

	m_layout->addSpacing(12);
	m_errorWidget = QtIndexingDialog::createErrorWidget(m_layout);

	m_layout->addStretch();

	if (shallow)
	{
		createMessageLabel(m_layout)->setText(QStringLiteral(
			"<i>You can now browse your project while running a second pass for in-depth "
			"indexing!</i>"));
		m_layout->addSpacing(12);
	}

	{
		QHBoxLayout* buttons = new QHBoxLayout();
		if (interrupted)
		{
			QPushButton* discardButton = new QPushButton(QStringLiteral("Discard"));
			discardButton->setObjectName(QStringLiteral("windowButton"));
			connect(
				discardButton, &QPushButton::clicked, this, &QtIndexingReportDialog::onDiscardPressed);
			buttons->addWidget(discardButton);
		}
		else if (shallow)
		{
			QPushButton* startInDepthButton = new QPushButton(QStringLiteral("Start In-Depth Indexing"));
			startInDepthButton->setObjectName(QStringLiteral("windowButton"));
			connect(
				startInDepthButton,
				&QPushButton::clicked,
				this,
				&QtIndexingReportDialog::onStartInDepthPressed);
			buttons->addWidget(startInDepthButton);
		}

		buttons->addStretch();

		QPushButton* confirmButton = new QPushButton(
			interrupted ? QStringLiteral("Keep") :
						  (shallow ? QStringLiteral("Later") : QStringLiteral("OK")));
		confirmButton->setObjectName(QStringLiteral("windowButton"));
		confirmButton->setDefault(true);
		connect(
			confirmButton, &QPushButton::clicked, this, &QtIndexingReportDialog::onConfirmPressed);
		buttons->addWidget(confirmButton);

		m_layout->addLayout(buttons);
	}

	if (!interrupted)
	{
		QtIndexingDialog::createFlagLabel(this);
	}

	setupDone();
}

QSize QtIndexingReportDialog::sizeHint() const
{
	return QSize(m_interrupted ? 400 : 430, 280);
}

void QtIndexingReportDialog::updateErrorCount(size_t errorCount, size_t fatalCount)
{
	if (m_errorWidget && errorCount)
	{
		QString str = QString::number(errorCount) + QStringLiteral(" Error");
		if (errorCount > 1)
		{
			str += QStringLiteral("s");
		}

		if (fatalCount)
		{
			str += QStringLiteral(" (") + QString::number(fatalCount) + QStringLiteral(" Fatal)");
		}

		QPushButton* errorCount = m_errorWidget->findChild<QPushButton*>(QStringLiteral("errorCount"));
		errorCount->setText(str);

		m_errorWidget->show();
	}
}

void QtIndexingReportDialog::closeEvent(QCloseEvent* event)
{
	emit QtIndexingDialog::canceled();
}

void QtIndexingReportDialog::keyPressEvent(QKeyEvent* event)
{
	if (!m_interrupted)	   // in this case we only show one button, so it is clear what to do
	{
		switch (event->key())
		{
		case Qt::Key_Escape:
		case Qt::Key_Return:
			onConfirmPressed();
			break;
		}
	}

	QWidget::keyPressEvent(event);
}

void QtIndexingReportDialog::onConfirmPressed()
{
	MessageErrorsHelpMessage().dispatch();

	emit QtIndexingDialog::finished();
}

void QtIndexingReportDialog::onDiscardPressed()
{
	emit QtIndexingDialog::canceled();
}

void QtIndexingReportDialog::onStartInDepthPressed()
{
	emit requestReindexing();
}
