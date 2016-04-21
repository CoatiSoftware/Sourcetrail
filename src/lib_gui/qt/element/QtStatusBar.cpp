#include "qt/element/QtStatusBar.h"

#include <QMovie>

#include "utility/messaging/type/MessageSearch.h"
#include "utility/ResourcePaths.h"

QtStatusBar::QtStatusBar()
    : m_text(this)
{
	QMovie* movie = new QMovie((ResourcePaths::getGuiPath() + "statusbar_view/loader.gif").c_str());
	// if movie doesn't loop forever, force it to.
	if (movie->loopCount() != -1)
	{
		connect(movie, SIGNAL(finished()), movie, SLOT(start()));
	}
	movie->start();

	m_loader.setMovie(movie);
	m_loader.hide();
	addWidget(&m_loader);

	m_text.setText("");
	addWidget(&m_text);

	m_errorButton.hide();
	m_errorButton.setFlat(true);
	m_errorButton.setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_errorButton.setStyleSheet(
		"QPushButton { color: #D00000; background: none; border: none; margin-right: 0; spacing: none; } "
		"QPushButton:hover { text-decoration: underline; }"
	);
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
		m_text.setStyleSheet("QLabel { color: #D00000 }");
	}
	else
	{
		m_text.setStyleSheet("");
	}

	if (showLoader)
	{
		m_loader.show();
	}
	else
	{
		m_loader.hide();
	}

	m_text.setText(text.c_str());
}

void QtStatusBar::setErrorCount(ErrorCountInfo errorCount)
{
	if (errorCount.total > 0)
	{
		m_errorButton.setText(
			QString::fromStdString("\u25CF ") + QString::number(errorCount.total) + " error" + (errorCount.total > 1 ? "s" : "") +
			(errorCount.fatal > 0 ? " (" + QString::number(errorCount.fatal) + " fatal)" : ""));
		m_errorButton.show();
	}
	else
	{
		m_errorButton.hide();
	}
}

void QtStatusBar::showErrors()
{
	SearchMatch match = SearchMatch::createCommand(SearchMatch::COMMAND_ERROR);
	MessageSearch(std::vector<SearchMatch>(1, match)).dispatch();
}
