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

	m_movie = std::make_shared<QMovie>((ResourcePaths::getGuiPath().str() + "statusbar_view/loader.gif").c_str());
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
	m_text.setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_text.setSizePolicy(QSizePolicy::Ignored, m_text.sizePolicy().verticalPolicy());
	addWidget(&m_text, 1);
	setText(L"", false, false);

	connect(&m_text, &QPushButton::clicked, this, &QtStatusBar::showStatus);

	addPermanentWidget(&m_ideStatusText);

	m_errorButton.hide();
	m_errorButton.setFlat(true);
	m_errorButton.setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_errorButton.setStyleSheet("QPushButton { color: #D00000; margin-right: 0; spacing: none; }");
	m_errorButton.setIcon(utility::colorizePixmap(
		QPixmap(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"statusbar_view/dot.png").wstr())),
//		"#D00000"
		QColor(0xD0, 0, 0)
	).scaledToHeight(12));
	addPermanentWidget(&m_errorButton);

	connect(&m_errorButton, &QPushButton::clicked, this, &QtStatusBar::showErrors);
}

void QtStatusBar::setText(const std::wstring& text, bool isError, bool showLoader)
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

	if (!text.empty())
	{
		m_textString = text;
		m_text.setText(m_text.fontMetrics().elidedText(QString::fromStdWString(m_textString), Qt::ElideRight, m_text.width()));
	}
}

void QtStatusBar::setErrorCount(ErrorCountInfo errorCount)
{
	if (errorCount.total > 0)
	{
		m_errorButton.setText(
			QString::number(errorCount.total) + " error" + (errorCount.total > 1 ? "s" : "") +
			(errorCount.fatal > 0 ? " (" + QString::number(errorCount.fatal) + " fatal)" : ""));

		if (errorCount.fatal > 0)
		{
			m_errorButton.setStyleSheet("QPushButton { color: #D00000; margin-right: 0; spacing: none; }");
		}
		else
		{
			m_errorButton.setStyleSheet("QPushButton { color: #000000; margin-right: 0; spacing: none; }");
		}

		m_errorButton.show();
	}
	else
	{
		m_errorButton.hide();
	}
}

void QtStatusBar::setIdeStatus(const std::wstring& text)
{
	m_ideStatusText.setText(QString::fromStdWString(text));
}

void QtStatusBar::resizeEvent(QResizeEvent* event)
{
	m_text.setText(m_text.fontMetrics().elidedText(QString::fromStdWString(m_textString), Qt::ElideRight, m_text.width()));
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
