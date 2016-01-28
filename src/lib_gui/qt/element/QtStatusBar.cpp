#include "qt/element/QtStatusBar.h"

#include <QMovie>

#include "qt/utility/utilityQt.h"
#include "utility/messaging/type/MessageShowErrors.h"
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
	m_errorButton.setStyleSheet("QPushButton { color: #D00000; margin-right: 0; spacing: none; }");
	m_errorButton.setIcon(utility::colorizePixmap(
		QPixmap((ResourcePaths::getGuiPath() + "statusbar_view/octagon.png").c_str()),
		"#D00000"
	).scaledToHeight(10));
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

void QtStatusBar::setErrorCount(size_t count)
{
	if (count > 0)
	{
		m_errorButton.setText(QString::number(count) + " error(s)");
		m_errorButton.show();
	}
	else
	{
		m_errorButton.hide();
	}
}

void QtStatusBar::showErrors()
{
	MessageShowErrors(-1).dispatch();
}
