#include "qt/element/QtStatusBar.h"

#include <QMovie>

QtStatusBar::QtStatusBar()
    : m_text(this)
{
	QMovie* movie = new QMovie("data/gui/statusbar_view/loader.gif");
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
}

QtStatusBar::~QtStatusBar()
{
}

void QtStatusBar::setText(const std::string& text, bool isError, bool showLoader)
{
	if (isError)
	{
		m_text.setStyleSheet("QLabel { color: red }");
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
