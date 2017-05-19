#include "qt/element/QtStatusBar.h"

#include <QMovie>

#include "qt/utility/utilityQt.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageShowStatus.h"
#include "utility/ResourcePaths.h"

QtStatusBar::QtStatusBar()
    : m_text(this)
	, m_ideStatusText(this)
{
	addWidget(new QWidget()); // add some space

	QMovie* movie = new QMovie((ResourcePaths::getGuiPath().str() + "statusbar_view/loader.gif").c_str());
	// if movie doesn't loop forever, force it to.
	if (movie->loopCount() != -1)
	{
		connect(movie, SIGNAL(finished()), movie, SLOT(start()));
	}
	movie->start();

	m_loader.setMovie(movie);
	m_loader.hide();
	addWidget(&m_loader);

	m_text.setFlat(true);
	m_text.setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_text.setSizePolicy(QSizePolicy::Ignored, m_text.sizePolicy().verticalPolicy());
	addWidget(&m_text, 1);
	setText("", false, false);

	connect(&m_text, SIGNAL(clicked()), this, SLOT(showStatus()));

	addPermanentWidget(&m_ideStatusText);

	m_errorButton.hide();
	m_errorButton.setFlat(true);
	m_errorButton.setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_errorButton.setStyleSheet("QPushButton { color: #D00000; margin-right: 0; spacing: none; }");
	m_errorButton.setIcon(utility::colorizePixmap(
		QPixmap((ResourcePaths::getGuiPath().str() + "statusbar_view/dot.png").c_str()),
		"#D00000"
	).scaledToHeight(12));
	addPermanentWidget(&m_errorButton);

	connect(&m_errorButton, SIGNAL(clicked()), this, SLOT(showErrors()));
}

QtStatusBar::~QtStatusBar()
{
}

void QtStatusBar::setText(const std::string& text, bool isError, bool showLoader)
{
	if (isError)
	{
		m_text.setStyleSheet("QPushButton { color: #D00000; margin-right: 0; spacing: none; text-align: left; }");
	}
	else
	{
		m_text.setStyleSheet("QPushButton { color: #000000; margin-right: 0; spacing: none; text-align: left; }");
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
	m_text.setText(m_text.fontMetrics().elidedText(QString::fromStdString(m_textString), Qt::ElideRight, m_text.width()));
}

void QtStatusBar::setErrorCount(ErrorCountInfo errorCount)
{
	if (errorCount.total > 0)
	{
		m_errorButton.setText(
			QString::number(errorCount.total) + " error" + (errorCount.total > 1 ? "s" : "") +
			(errorCount.fatal > 0 ? " (" + QString::number(errorCount.fatal) + " fatal)" : ""));
		m_errorButton.show();
	}
	else
	{
		m_errorButton.hide();
	}
}

void QtStatusBar::setIdeStatus(const std::string& text)
{
	m_ideStatusText.setText(text.c_str());
}

void QtStatusBar::resizeEvent(QResizeEvent* event)
{
	m_text.setText(m_text.fontMetrics().elidedText(QString::fromStdString(m_textString), Qt::ElideRight, m_text.width()));
}

void QtStatusBar::showStatus()
{
	MessageShowStatus().dispatch();
}

void QtStatusBar::showErrors()
{
	SearchMatch match = SearchMatch::createCommand(SearchMatch::COMMAND_ERROR);
	MessageSearch(std::vector<SearchMatch>(1, match)).dispatch();
}
