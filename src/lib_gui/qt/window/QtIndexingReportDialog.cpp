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
		QtIndexingDialog::createTitleLabel("Interrupted Indexing", m_layout);
	}
	else if (shallow)
	{
		QtIndexingDialog::createTitleLabel("Finished Shallow Indexing", m_layout);
	}
	else
	{
		QtIndexingDialog::createTitleLabel("Finished Indexing", m_layout);
	}

	m_layout->addSpacing(5);

	QtIndexingDialog::createMessageLabel(m_layout)->setText(
		"Source files indexed:   " + QString::number(indexedFileCount) + "/" +
		QString::number(totalIndexedFileCount));

	QtIndexingDialog::createMessageLabel(m_layout)->setText(
		"Total files completed:   " + QString::number(completedFileCount) + "/" +
		QString::number(totalFileCount));

	m_layout->addSpacing(12);
	QtIndexingDialog::createMessageLabel(m_layout)->setText(
		"Time:   " + QString::fromStdString(TimeStamp::secondsToString(time)));

	m_layout->addSpacing(12);
	m_errorWidget = QtIndexingDialog::createErrorWidget(m_layout);

	m_layout->addStretch();

	if (shallow)
	{
		createMessageLabel(m_layout)->setText(
			"<i>You can now browse your project while running a second pass for in-depth "
			"indexing!</i>");
		m_layout->addSpacing(12);
	}

	{
		QHBoxLayout* buttons = new QHBoxLayout();
		if (interrupted)
		{
			QPushButton* discardButton = new QPushButton("Discard");
			discardButton->setObjectName("windowButton");
			connect(
				discardButton, &QPushButton::clicked, this, &QtIndexingReportDialog::onDiscardPressed);
			buttons->addWidget(discardButton);
		}
		else if (shallow)
		{
			QPushButton* startInDepthButton = new QPushButton("Start In-Depth Indexing");
			startInDepthButton->setObjectName("windowButton");
			connect(
				startInDepthButton,
				&QPushButton::clicked,
				this,
				&QtIndexingReportDialog::onStartInDepthPressed);
			buttons->addWidget(startInDepthButton);
		}

		buttons->addStretch();

		QPushButton* confirmButton = new QPushButton(
			interrupted ? "Keep" : (shallow ? "Later" : "OK"));
		confirmButton->setObjectName("windowButton");
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
