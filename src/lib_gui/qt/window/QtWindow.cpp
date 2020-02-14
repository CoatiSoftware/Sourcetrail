#include "QtWindow.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

#include "ApplicationSettings.h"
#include "QtDeviceScaledPixmap.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtWindow::QtWindow(bool isSubWindow, QWidget* parent)
	: QtWindowBase(isSubWindow, parent)
	, m_title(nullptr)
	, m_subTitle(nullptr)
	, m_nextButton(nullptr)
	, m_previousButton(nullptr)
	, m_closeButton(nullptr)
	, m_cancelAble(true)
	, m_scrollAble(false)
	, m_hasLogo(false)
{
}

void QtWindow::setup()
{
	setStyleSheet(
		utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")).c_str());

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(10, 10, 10, 10);

	{
		QHBoxLayout* hlayout = new QHBoxLayout();
		hlayout->setContentsMargins(0, 0, 0, 0);

		m_title = new QLabel();
		m_title->setObjectName(QStringLiteral("title"));
		hlayout->addWidget(m_title);

		hlayout->addStretch();

		m_subTitle = new QLabel();
		m_subTitle->setObjectName(QStringLiteral("subTitle"));
		hlayout->addWidget(m_subTitle);

		layout->addLayout(hlayout);
	}

	layout->addSpacing(10);

	QWidget* contentWidget = new QWidget();
	contentWidget->setObjectName(QStringLiteral("form"));

	populateWindow(contentWidget);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setObjectName(QStringLiteral("formArea"));
	scrollArea->setWidgetResizable(true);

	scrollArea->setWidget(contentWidget);
	layout->addWidget(scrollArea);

	if (!m_scrollAble)
	{
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		scrollArea->setObjectName(QStringLiteral("scrollArea"));
	}

	QHBoxLayout* buttonLayout = createButtons();
	layout->addLayout(buttonLayout);

	m_content->setLayout(layout);

	setupDone();
}

void QtWindow::setCancelAble(bool cancelable)
{
	setCloseEnabled(cancelable);

	m_cancelAble = cancelable;
}

void QtWindow::setScrollAble(bool scrollAble)
{
	m_scrollAble = scrollAble;
}

bool QtWindow::isScrollAble() const
{
	return m_scrollAble;
}

void QtWindow::updateTitle(const QString& title)
{
	if (m_title)
	{
		m_title->setText(title);
	}
}

std::wstring QtWindow::getTitle() const
{
	if (m_title)
	{
		return m_title->text().toStdWString();
	}
	return L"";
}

void QtWindow::updateSubTitle(const QString& subTitle)
{
	if (m_subTitle)
	{
		m_subTitle->setText(subTitle);
	}
}

void QtWindow::updateNextButton(const QString& text)
{
	if (m_nextButton)
	{
		m_nextButton->setText(text);
	}
}

void QtWindow::updatePreviousButton(const QString& text)
{
	if (m_previousButton)
	{
		m_previousButton->setText(text);
	}
}

void QtWindow::updateCloseButton(const QString& text)
{
	if (m_closeButton)
	{
		m_closeButton->setText(text);
	}
}

void QtWindow::setNextEnabled(bool enabled)
{
	if (m_nextButton)
	{
		m_nextButton->setEnabled(enabled);
	}
}

void QtWindow::setPreviousEnabled(bool enabled)
{
	if (m_previousButton)
	{
		m_previousButton->setEnabled(enabled);
	}
}

void QtWindow::setCloseEnabled(bool enabled)
{
	if (m_closeButton)
	{
		m_closeButton->setEnabled(enabled);
	}
}

void QtWindow::setNextVisible(bool visible)
{
	if (m_nextButton)
	{
		m_nextButton->setVisible(visible);
	}
}

void QtWindow::setPreviousVisible(bool visible)
{
	if (m_previousButton)
	{
		m_previousButton->setVisible(visible);
	}
}

void QtWindow::setCloseVisible(bool visible)
{
	if (m_closeButton)
	{
		m_closeButton->setVisible(visible);
	}
}

void QtWindow::setNextDefault(bool isDefault)
{
	if (m_nextButton)
	{
		m_nextButton->setDefault(isDefault);
	}
}

void QtWindow::setPreviousDefault(bool isDefault)
{
	if (m_previousButton)
	{
		m_previousButton->setDefault(isDefault);
	}
}

void QtWindow::setCloseDefault(bool isDefault)
{
	if (m_closeButton)
	{
		m_closeButton->setDefault(isDefault);
	}
}

void QtWindow::closeEvent(QCloseEvent* event)
{
	handleClose();
}

void QtWindow::resizeEvent(QResizeEvent* event)
{
	QSize size = event->size();

	if (size.width() < 300)
	{
		size.setWidth(300);
		resize(size);
		return;
	}

	if (size.height() < 200)
	{
		size.setHeight(200);
		resize(size);
		return;
	}

	if (!m_isSubWindow)
	{
		m_window->resize(size);
		return;
	}

	int displacement = 0;
	if (m_hasLogo)
	{
		displacement = 58;
	}

	QSize windowSize = size - QSize(30, 30 + displacement);

	m_window->resize(windowSize);
	m_window->move(15, displacement + 15);
}

void QtWindow::keyPressEvent(QKeyEvent* event)
{
	if (m_cancelAble && event->key() == Qt::Key_Escape)
	{
		handleClose();
		return;
	}

	std::vector<QPushButton*> buttons;
	if (m_nextButton && m_nextButton->isVisible())
		buttons.push_back(m_nextButton);
	if (m_closeButton && m_closeButton->isVisible())
		buttons.push_back(m_closeButton);
	if (m_previousButton && m_previousButton->isVisible())
		buttons.push_back(m_previousButton);

	if (event->key() == Qt::Key_Return)
	{
		for (QPushButton* button: buttons)
		{
			if (button->isDefault())
			{
				button->animateClick();
				return;
			}
		}
	}

	if (event->key() == Qt::Key_Tab)
	{
		for (size_t i = 0; i < buttons.size(); i++)
		{
			if (buttons[i]->isDefault())
			{
				buttons[i]->setDefault(false);
				buttons[(i + 1) % buttons.size()]->setDefault(true);
				return;
			}
		}

		if (buttons.size())
		{
			buttons[0]->setDefault(true);
			return;
		}
	}

	QWidget::keyPressEvent(event);
}

void QtWindow::populateWindow(QWidget* widget) {}

void QtWindow::windowReady() {}

void QtWindow::handleNext()
{
	emit finished();
}

void QtWindow::handlePrevious()
{
	emit previous();
}

void QtWindow::handleClose()
{
	emit canceled();
}

void QtWindow::setupDone()
{
	windowReady();

	QSize actualSize = m_window->sizeHint() + QSize(50, 50);
	QSize preferredSize = sizeHint();

	QSize size(
		qMax(actualSize.width(), preferredSize.width()),
		qMax(actualSize.height(), preferredSize.height()));
	resize(size);

	moveToCenter();
}

void QtWindow::addLogo()
{
	QtDeviceScaledPixmap sourcetrailLogo(
		QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"window/logo.png").wstr()));
	sourcetrailLogo.scaleToWidth(240);

	QLabel* sourcetrailLogoLabel = new QLabel(this);
	sourcetrailLogoLabel->setPixmap(sourcetrailLogo.pixmap());
	sourcetrailLogoLabel->resize(
		static_cast<int>(sourcetrailLogo.width()), static_cast<int>(sourcetrailLogo.height()));
	sourcetrailLogoLabel->move(m_isSubWindow ? 40 : 23, 25);
	sourcetrailLogoLabel->show();

	m_hasLogo = true;
	resize(sizeHint());
}

QHBoxLayout* QtWindow::createButtons()
{
	m_nextButton = new QPushButton(QStringLiteral("Next"));
	m_nextButton->setObjectName(QStringLiteral("windowButton"));
	connect(m_nextButton, &QPushButton::clicked, this, &QtWindow::handleNextPress);

	m_previousButton = new QPushButton(QStringLiteral("Previous"));
	m_previousButton->setObjectName(QStringLiteral("windowButton"));
	connect(m_previousButton, &QPushButton::clicked, this, &QtWindow::handlePreviousPress);

	m_closeButton = new QPushButton(QStringLiteral("Cancel"));
	m_closeButton->setObjectName(QStringLiteral("windowButton"));
	connect(m_closeButton, &QPushButton::clicked, this, &QtWindow::handleClosePress);

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(m_closeButton);
	buttons->addStretch();
	buttons->addWidget(m_previousButton);
	buttons->addSpacing(3);
	buttons->addWidget(m_nextButton);
	return buttons;
}

void QtWindow::handleNextPress(bool)
{
	handleNext();
}

void QtWindow::handlePreviousPress()
{
	handlePrevious();
}

void QtWindow::handleClosePress()
{
	handleClose();
}
