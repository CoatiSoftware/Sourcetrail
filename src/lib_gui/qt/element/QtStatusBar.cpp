#include "QtStatusBar.h"

#include <QHBoxLayout>
#include <QMovie>
#include <QProgressBar>

#include "MessageErrorsAll.h"
#include "MessageIndexingShowDialog.h"
#include "MessageShowStatus.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtStatusBar::QtStatusBar(): m_text(this), m_ideStatusText(this)
{
	addWidget(new QWidget());	 // add some space

	m_movie = std::make_shared<QMovie>(QString::fromStdWString(
		ResourcePaths::getGuiPath().concatenate(L"statusbar_view/loader.gif").wstr()));
	// if movie doesn't loop forever, force it to.
	if (m_movie->loopCount() != -1)
	{
		connect(m_movie.get(), &QMovie::finished, m_movie.get(), &QMovie::start);
	}
	m_movie->start();

	m_loader.setMovie(m_movie.get());
	m_loader.hide();
	addWidget(&m_loader);

	m_text.setFlat(true);
	m_text.setAttribute(Qt::WA_LayoutUsesWidgetRect);	 // fixes layouting on Mac
	m_text.setSizePolicy(QSizePolicy::Ignored, m_text.sizePolicy().verticalPolicy());
	m_text.setCursor(Qt::PointingHandCursor);
	addWidget(&m_text, 1);
	setText(L"", false, false);

	connect(&m_text, &QPushButton::clicked, this, &QtStatusBar::showStatus);

	// ide status
	{
		addPermanentVLine();
		addPermanentWidget(&m_ideStatusText);
	}

	// errors
	{
		m_vlineError = addPermanentVLine();
		m_vlineError->hide();

		m_errorButton.hide();
		m_errorButton.setFlat(true);
		m_errorButton.setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
		m_errorButton.setStyleSheet(
			QStringLiteral("QPushButton { color: #D00000; margin-right: 0; spacing: none; }"));
		m_errorButton.setIcon(
			utility::colorizePixmap(
				QPixmap(QString::fromStdWString(
					ResourcePaths::getGuiPath().concatenate(L"statusbar_view/dot.png").wstr())),
				QColor(0xD0, 0, 0))
				.scaledToHeight(12));
		m_errorButton.setCursor(Qt::PointingHandCursor);
		addPermanentWidget(&m_errorButton);

		connect(&m_errorButton, &QPushButton::clicked, this, &QtStatusBar::showErrors);
	}

	// indexing status
	{
		m_vlineIndexing = addPermanentVLine();
		m_vlineIndexing->hide();

		m_indexingStatus = new QPushButton(this);
		m_indexingStatus->setFlat(true);
		m_indexingStatus->setMinimumWidth(150);
		m_indexingStatus->setStyleSheet(
			QStringLiteral("QPushButton { margin-right: 0; spacing: none; }"));
		m_indexingStatus->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
		m_indexingStatus->setCursor(Qt::PointingHandCursor);

		connect(m_indexingStatus, &QPushButton::clicked, this, &QtStatusBar::clickedIndexingProgress);

		QHBoxLayout* layout = new QHBoxLayout();
		layout->setContentsMargins(0, 0, 0, 0);

		layout->addWidget(new QLabel(QStringLiteral("Indexing:")));

		m_indexingProgress = new QProgressBar();
		m_indexingProgress->setMinimum(0);
		m_indexingProgress->setMaximum(100);
		m_indexingProgress->setValue(100);
		layout->addWidget(m_indexingProgress);

		m_indexingStatus->setLayout(layout);
		m_indexingStatus->hide();

		addPermanentWidget(m_indexingStatus);
	}
}

void QtStatusBar::setText(const std::wstring& text, bool isError, bool showLoader)
{
	if (isError)
	{
		m_text.setStyleSheet(QStringLiteral(
			"QPushButton { color: #D00000; margin-right: 0; spacing: none; text-align: left; }"));
	}
	else
	{
		m_text.setStyleSheet(
			QStringLiteral("QPushButton { margin-right: 0; spacing: none; text-align: left; }"));
	}

	if (showLoader)
	{
		m_loader.show();
	}
	else
	{
		m_loader.hide();
	}

	m_textString = text;
	m_text.setText(m_text.fontMetrics().elidedText(
		QString::fromStdWString(m_textString), Qt::ElideRight, m_text.width()));
}

void QtStatusBar::setErrorCount(ErrorCountInfo errorCount)
{
	if (errorCount.total > 0)
	{
		m_errorButton.setText(
			QString::number(errorCount.total) + " error" + (errorCount.total > 1 ? "s" : "") +
			(errorCount.fatal > 0 ? " (" + QString::number(errorCount.fatal) + " fatal)"
								  : QLatin1String("")));

		m_errorButton.setMinimumWidth(
			m_errorButton.fontMetrics().width(QString(m_errorButton.text().size(), 'a')));

		if (errorCount.fatal > 0)
		{
			m_errorButton.setStyleSheet(
				QStringLiteral("QPushButton { color: #D00000; margin-right: 0; spacing: none; }"));
		}
		else
		{
			m_errorButton.setStyleSheet(
				QStringLiteral("QPushButton { margin-right: 0; spacing: none; }"));
		}

		m_errorButton.show();
		m_vlineError->show();
	}
	else
	{
		m_errorButton.hide();
		m_vlineError->hide();
	}
}

void QtStatusBar::setIdeStatus(const std::wstring& text)
{
	m_ideStatusText.setText(QString::fromStdWString(text));
}

void QtStatusBar::showIndexingProgress(size_t progressPercent)
{
	m_indexingStatus->show();
	m_vlineIndexing->show();

	m_indexingProgress->setValue(static_cast<int>(progressPercent));
}

void QtStatusBar::hideIndexingProgress()
{
	m_indexingStatus->hide();
	m_vlineIndexing->hide();
}

void QtStatusBar::resizeEvent(QResizeEvent* event)
{
	m_text.setText(m_text.fontMetrics().elidedText(
		QString::fromStdWString(m_textString), Qt::ElideRight, m_text.width()));
}

void QtStatusBar::showStatus()
{
	MessageShowStatus().dispatch();
}

void QtStatusBar::showErrors()
{
	MessageErrorsAll().dispatch();
}

void QtStatusBar::clickedIndexingProgress()
{
	MessageIndexingShowDialog().dispatch();
}

QWidget* QtStatusBar::addPermanentVLine()
{
	QFrame* vline = new QFrame(this);
	vline->setFrameShape(QFrame::VLine);
	vline->setStyleSheet(QStringLiteral("color: #777"));
	addPermanentWidget(vline);
	return vline;
}
