#include "qt/window/QtWindow.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSysInfo>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"

QtWindow::QtWindow(QWidget* parent, int displacement)
	: QtWindowStackElement(parent)
	, m_window(nullptr)
	, m_title(nullptr)
	, m_nextButton(nullptr)
	, m_previousButton(nullptr)
	, m_closeButton(nullptr)
	, m_displacement(displacement)
	, m_cancelAble(true)
	, m_scrollAble(false)
	, m_showAsPopup(false)
	, m_mousePressedInWindow(false)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);

	move(parent->pos().x() + parent->width() / 2 - 250, parent->pos().y() + parent->height() / 2 - 250);

	QSize windowSize = sizeHint();

	m_window = new QWidget(this);
	windowSize.setHeight(windowSize.height() - displacement - 10);
	windowSize.setWidth(windowSize.width() - 10);
	m_window->move(0, displacement);
	m_window->resize(windowSize);

	std::string frameStyle =
		"#window {"
			"font-size: 17pt; "
			"border: 1px solid lightgray;"
			"border-radius: 15px; "
			"background: white; "
		"}";
	m_window->setStyleSheet(frameStyle.c_str());
	m_window->setObjectName("window");

	// window shadow
	if (QSysInfo::macVersion() == QSysInfo::MV_None)
	{
		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
		effect->setBlurRadius(5);
		effect->setXOffset(2);
		effect->setYOffset(2);
		effect->setColor(Qt::darkGray);
		m_window->setGraphicsEffect(effect);
	}

	this->raise();
}

QSize QtWindow::sizeHint() const
{
	return QSize(500, 500);
}

void QtWindow::setup()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "window/window.css").c_str());
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(25, 30, 25, 20);

	m_title = new QLabel();
	m_title->setObjectName("title");
	layout->addWidget(m_title);
	layout->addSpacing(10);

	QWidget* contentWidget = new QWidget();
	contentWidget->setObjectName("form");

	populateWindow(contentWidget);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setObjectName("formArea");
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
		scrollArea->setObjectName("scrollArea");
	}

	{
		m_nextButton = new QPushButton("Next");
		m_nextButton->setObjectName("windowButton");
		connect(m_nextButton, SIGNAL(clicked()), this, SLOT(handleNextPress()));

		m_previousButton = new QPushButton("Previous");
		m_previousButton->setObjectName("windowButton");
		connect(m_previousButton, SIGNAL(clicked()), this, SLOT(handlePreviousPress()));

		m_closeButton = new QPushButton("Cancel");
		m_closeButton->setObjectName("windowButton");
		connect(m_closeButton, SIGNAL(clicked()), this, SLOT(handleClosePress()));

		QHBoxLayout* buttons = new QHBoxLayout();
		buttons->addWidget(m_closeButton);
		buttons->addStretch();
		buttons->addWidget(m_previousButton);
		buttons->addSpacing(3);
		buttons->addWidget(m_nextButton);

		layout->addLayout(buttons);
	}

	m_window->setLayout(layout);

	if (m_showAsPopup)
	{
		updateNextButton("Ok");
		setCloseVisible(false);
		setPreviousVisible(false);
	}

	windowReady();
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

void QtWindow::setShowAsPopup(bool showAsPopup)
{
	m_showAsPopup = showAsPopup;
}

bool QtWindow::isScrollAble() const
{
	return m_scrollAble;
}

bool QtWindow::isPopup() const
{
	return m_showAsPopup;
}

void QtWindow::updateTitle(QString title)
{
	if (m_title)
	{
		m_title->setText(title);
	}
}

void QtWindow::updateNextButton(QString text)
{
	if (m_nextButton)
	{
		m_nextButton->setText(text);
	}
}

void QtWindow::updateCloseButton(QString text)
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

void QtWindow::showWindow()
{
	show();
}

void QtWindow::hideWindow()
{
	hide();
}

void QtWindow::resizeEvent(QResizeEvent *event)
{
	QSize windowSize = event->size() - QSize(10, 10 + m_displacement);
	m_window->resize(windowSize);
	m_window->move(0, m_displacement);
}

void QtWindow::keyPressEvent(QKeyEvent *event)
{
	if (m_cancelAble && event->key() == Qt::Key_Escape)
	{
		emit canceled();
	}

	QWidget::keyPressEvent(event);
}

void QtWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && m_mousePressedInWindow)
	{
		move(event->globalPos() - m_dragPosition);
		event->accept();
	}
}

void QtWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = true;
	}
}

void QtWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = false;
	}
}

void QtWindow::populateWindow(QWidget* widget)
{
}

void QtWindow::windowReady()
{
}

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

void QtWindow::addLogo()
{
	QtDeviceScaledPixmap coatiLogo((ResourcePaths::getGuiPath() + "window/logo.png").c_str());
	coatiLogo.scaleToWidth(150);

	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coatiLogo.pixmap());
	coatiLogoLabel->resize(coatiLogo.width(), coatiLogo.height());
	coatiLogoLabel->move(30, 25);
	coatiLogoLabel->show();
}

void QtWindow::handleNextPress()
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
