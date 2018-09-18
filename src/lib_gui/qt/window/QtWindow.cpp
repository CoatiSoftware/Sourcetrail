#include "QtWindow.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

#include "QtDeviceScaledPixmap.h"
#include "utilityQt.h"
#include "ApplicationSettings.h"
#include "ResourcePaths.h"

QtWindow::QtWindow(bool isSubWindow, QWidget* parent)
	: QtWindowStackElement(parent)
	, m_isSubWindow(isSubWindow)
	, m_window(nullptr)
	, m_title(nullptr)
	, m_subTitle(nullptr)
	, m_nextButton(nullptr)
	, m_previousButton(nullptr)
	, m_closeButton(nullptr)
	, m_cancelAble(true)
	, m_scrollAble(false)
	, m_hasLogo(false)
	, m_mousePressedInWindow(false)
	, m_sizeGrip(nullptr)
{
	if (isSubWindow)
	{
		setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground, true);
		setFocusPolicy(Qt::StrongFocus);
		setFocus();
	}
	else
	{
		setWindowFlags(Qt::Window);
	}

	m_window = new QWidget(this);
	m_window->setObjectName("window");

	QVBoxLayout* layout = new QVBoxLayout(m_window);
	layout->setSpacing(0);

	m_content = new QWidget();
	layout->addWidget(m_content);

	if (isSubWindow)
	{
		std::string frameStyle =
			"#window {"
			"	border: 1px solid lightgray;"
			"	border-radius: 15px;"
			"	background: white;"
			"}";
		m_window->setStyleSheet(frameStyle.c_str());

		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
		effect->setBlurRadius(15);
		effect->setXOffset(0);
		effect->setYOffset(5);
		effect->setColor(Qt::darkGray);
		m_window->setGraphicsEffect(effect);

		QHBoxLayout* gripLayout = new QHBoxLayout();
		m_sizeGrip = new QSizeGrip(m_window);
		setSizeGripStyle(true);
		gripLayout->addWidget(new QWidget());
		gripLayout->addWidget(m_sizeGrip);
		layout->addLayout(gripLayout);
	}

	resize(sizeHint());

    moveToCenter();

	this->raise();
}

QSize QtWindow::sizeHint() const
{
	return QSize(
		ApplicationSettings::getInstance()->getWindowBaseWidth(),
		ApplicationSettings::getInstance()->getWindowBaseHeight()
	);
}

void QtWindow::setup()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")).c_str());

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(10, 10, 10, 10);

	{
		QHBoxLayout* hlayout = new QHBoxLayout();
		hlayout->setContentsMargins(0, 0, 0, 0);

		m_title = new QLabel();
		m_title->setObjectName("title");
		hlayout->addWidget(m_title);

		hlayout->addStretch();

		m_subTitle = new QLabel();
		m_subTitle->setObjectName("subTitle");
		hlayout->addWidget(m_subTitle);

		layout->addLayout(hlayout);
	}

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

	QHBoxLayout* buttonLayout = createButtons();
	layout->addLayout(buttonLayout);

	m_content->setLayout(layout);

	setupDone();
}

void QtWindow::setSizeGripStyle(bool isBlack)
{
	if (!m_sizeGrip)
	{
		return;
	}

	const std::wstring path = isBlack ? L"size_grip_black.png" : L"size_grip_white.png";

	m_sizeGrip->setStyleSheet(QString::fromStdWString(
		L"QSizeGrip {"
		"	max-height: 16px;"
		"	max-width: 16px;"
		"	border-image: url(" + ResourcePaths::getGuiPath().wstr() + L"window/" + path + L");"
		"}"
	));
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

bool QtWindow::isSubWindow() const
{
    return m_isSubWindow;
}

void QtWindow::moveToCenter()
{
    if (parentWidget())
    {
        if (m_isSubWindow)
        {
            move(
                parentWidget()->width() / 2 - sizeHint().width() / 2,
                parentWidget()->height() / 2 - sizeHint().height() / 2
            );
        }
        else
        {
            move(
                parentWidget()->pos().x() + parentWidget()->width() / 2 - sizeHint().width() / 2,
                parentWidget()->pos().y() + parentWidget()->height() / 2 - sizeHint().height() / 2
            );
        }
    }
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

void QtWindow::updateSubTitle(QString subTitle)
{
	if (m_subTitle)
	{
		m_subTitle->setText(subTitle);
	}
}

void QtWindow::updateNextButton(QString text)
{
	if (m_nextButton)
	{
		m_nextButton->setText(text);
	}
}

void QtWindow::updatePreviousButton(QString text)
{
	if (m_previousButton)
	{
		m_previousButton->setText(text);
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

void QtWindow::showWindow()
{
	show();
}

void QtWindow::hideWindow()
{
	hide();
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
	if (m_nextButton && m_nextButton->isVisible()) buttons.push_back(m_nextButton);
	if (m_closeButton && m_closeButton->isVisible()) buttons.push_back(m_closeButton);
	if (m_previousButton && m_previousButton->isVisible()) buttons.push_back(m_previousButton);

	if (event->key() == Qt::Key_Return)
	{
		for (QPushButton* button : buttons)
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

void QtWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton && m_mousePressedInWindow)
	{
        if (m_isSubWindow)
        {
            QPoint pos = event->globalPos() - m_dragPosition;
            QRect parentRect = parentWidget()->rect();

            if (pos.x() < parentRect.left())
            {
                pos.setX(parentRect.left());
            }
            pos.setX(qBound(parentRect.left(), parentRect.right() - width(), pos.x()));
            pos.setY(qBound(parentRect.top(), parentRect.bottom() - height(), pos.y()));

            move(pos);
            event->accept();
        }
        else
        {
            move(event->globalPos() - m_dragPosition);
            event->accept();
        }
	}
}

void QtWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = true;
	}
}

void QtWindow::mouseReleaseEvent(QMouseEvent* event)
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

void QtWindow::setupDone()
{
	windowReady();

	QSize actualSize = m_window->sizeHint() + QSize(50, 50);
	QSize preferredSize = sizeHint();

	QSize size(qMax(actualSize.width(), preferredSize.width()), qMax(actualSize.height(), preferredSize.height()));
	resize(size);

    moveToCenter();
}

void QtWindow::addLogo()
{
	QtDeviceScaledPixmap sourcetrailLogo(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"window/logo.png").wstr()));
	sourcetrailLogo.scaleToWidth(240);

	QLabel* sourcetrailLogoLabel = new QLabel(this);
	sourcetrailLogoLabel->setPixmap(sourcetrailLogo.pixmap());
	sourcetrailLogoLabel->resize(sourcetrailLogo.width(), sourcetrailLogo.height());
	sourcetrailLogoLabel->move(m_isSubWindow ? 40 : 23, 25);
	sourcetrailLogoLabel->show();

	m_hasLogo = true;
	resize(sizeHint());
}

QHBoxLayout* QtWindow::createButtons()
{
	m_nextButton = new QPushButton("Next");
	m_nextButton->setObjectName("windowButton");
	connect(m_nextButton, &QPushButton::clicked, this, &QtWindow::handleNextPress);

	m_previousButton = new QPushButton("Previous");
	m_previousButton->setObjectName("windowButton");
	connect(m_previousButton, &QPushButton::clicked, this, &QtWindow::handlePreviousPress);

	m_closeButton = new QPushButton("Cancel");
	m_closeButton->setObjectName("windowButton");
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
