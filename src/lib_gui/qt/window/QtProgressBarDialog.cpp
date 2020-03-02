#include "QtProgressBarDialog.h"

#include <QLabel>

#include "QtProgressBar.h"

QtProgressBarDialog::QtProgressBarDialog(float topRatio, bool hideable, QWidget* parent)
	: QtIndexingDialog(true, parent), m_title(nullptr), m_topRatio(topRatio)
{
	m_top = new QWidget(m_window);
	m_top->setObjectName(QStringLiteral("topHalf"));
	m_top->setGeometry(0, 0, 0, 0);
	m_top->show();
	m_top->lower();

	m_progressBar = new QtProgressBar(m_window);
	m_progressBar->setGeometry(0, 0, 0, 0);

	m_title = new QLabel(QLatin1String(""), this);
	m_title->setObjectName(QStringLiteral("title"));
	m_title->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	m_title->show();

	m_percentLabel = new QLabel(QStringLiteral("0% Progress"));
	m_percentLabel->setObjectName(QStringLiteral("percent"));
	m_layout->addWidget(m_percentLabel, 0, Qt::AlignRight);

	m_messageLabel = QtIndexingDialog::createMessageLabel(m_layout);
}

void QtProgressBarDialog::updateTitle(const QString& title)
{
	if (m_title)
	{
		m_title->setText(title);
	}
}

std::wstring QtProgressBarDialog::getTitle() const
{
	if (m_title)
	{
		return m_title->text().toStdWString();
	}
	return L"";
}

void QtProgressBarDialog::updateMessage(const QString& message)
{
	if (m_messageLabel)
	{
		m_messageLabel->setText(message);
	}
}

std::wstring QtProgressBarDialog::getMessage() const
{
	if (m_messageLabel)
	{
		return m_messageLabel->text().toStdWString();
	}
	return L"";
}

void QtProgressBarDialog::setUnknownProgress()
{
	m_progressBar->showUnknownProgressAnimated();
	m_percentLabel->hide();
}

void QtProgressBarDialog::updateProgress(size_t progress)
{
	size_t percent = std::min<size_t>(std::max<size_t>(progress, 0), 100);

	m_progressBar->showProgress(percent);
	m_percentLabel->setText(QString::number(percent) + "% Progress");
	m_percentLabel->show();
	setGeometries();
}

size_t QtProgressBarDialog::getProgress() const
{
	return m_progressBar->getProgress();
}

void QtProgressBarDialog::resizeEvent(QResizeEvent* event)
{
	QtIndexingDialog::resizeEvent(event);
	setGeometries();
}

void QtProgressBarDialog::setGeometries()
{
	if (m_top)
	{
		QMargins margins = m_content->layout()->contentsMargins();
		margins.setTop(static_cast<int>(m_window->size().height() * m_topRatio + 10));
		m_content->layout()->setContentsMargins(margins);

		m_top->setGeometry(
			0, 0, m_window->size().width(), static_cast<int>(m_window->size().height() * m_topRatio));

		m_title->setGeometry(
			45,
			static_cast<int>(m_window->size().height() * m_topRatio - 50),
			m_window->size().width() - 60,
			40);
	}

	if (m_progressBar)
	{
		m_progressBar->setGeometry(
			0,
			static_cast<int>(m_window->size().height() * m_topRatio - 5),
			m_window->size().width(),
			10);
	}
}
