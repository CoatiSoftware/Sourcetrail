#include "qt/window/QtSettingsWindow.h"

#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSysInfo>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/ResourcePaths.h"

QtSettingsWindow::QtSettingsWindow(QWidget *parent, int displacement)
	: QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint)
	, m_title(nullptr)
	, m_cancelButton(nullptr)
	, m_doneButton(nullptr)
	, m_mousePressedInWindow(false)
	, m_cancelAble(true)
{
	QSize windowSize = sizeHint();
	move(parent->pos().x() + parent->width() / 2 - 250, parent->pos().y() + parent->height() / 2 - 250);

	setAttribute(Qt::WA_TranslucentBackground, true);
	m_displacment = displacement;

	m_window = new QWidget(this);
	windowSize.setHeight(windowSize.height() - displacement - 10);
	windowSize.setWidth(windowSize.width() - 10);
	m_window->move(0, displacement);
	m_window->resize(windowSize);

	std::string frameStyle =
		"#SettingWindow {"
			"font-size: 17pt; "
			"border: 1px solid lightgray;"
			"border-radius: 15px; "
			"background: white; "
		"}";
	m_window->setStyleSheet(frameStyle.c_str());
	m_window->setObjectName("SettingWindow");

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

QSize QtSettingsWindow::sizeHint() const
{
	return QSize(500, 500);
}

void QtSettingsWindow::setCancelAble(bool cancelable)
{
	if (m_cancelButton)
	{
		m_cancelButton->setEnabled(cancelable);
	}

	m_cancelAble = cancelable;
}

void QtSettingsWindow::keyPressEvent(QKeyEvent *event)
{
	if (m_cancelAble && event->key() == Qt::Key_Escape)
	{
		emit canceled();
	}

	QWidget::keyPressEvent(event);
}

void QtSettingsWindow::resizeEvent(QResizeEvent *event)
{
	QSize windowSize = event->size()-QSize(10, 10 + m_displacment);
	m_window->resize(windowSize);
	m_window->move(0, m_displacment);
}

void QtSettingsWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && m_mousePressedInWindow)
	{
		move(event->globalPos() - m_dragPosition);
		event->accept();
	}
}

void QtSettingsWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = true;
	}
}

void QtSettingsWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = false;
	}
}

void QtSettingsWindow::setupForm()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "setting_window/window.css").c_str());
	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(25, 30, 25, 20);

	m_title = new QLabel();
	m_title->setObjectName("titleLabel");
	windowLayout->addWidget(m_title);
	windowLayout->addSpacing(30);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setObjectName("formArea");
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setWidgetResizable(true);

	QWidget* form = new QWidget();
	form->setObjectName("form");
	scrollArea->setWidget(form);

	populateWindow(form);

	windowLayout->addWidget(scrollArea);
	windowLayout->addSpacing(20);

	showButtons(windowLayout);

	m_window->setLayout(windowLayout);
	resize(QSize(600, 620));

	scrollArea->raise();
}

void QtSettingsWindow::populateWindow(QWidget* widget)
{
}

void QtSettingsWindow::addLogo()
{
	QtDeviceScaledPixmap coatiLogo((ResourcePaths::getGuiPath() + "startscreen/logo.png").c_str());
	coatiLogo.scaleToWidth(200);

	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coatiLogo.pixmap());
	coatiLogoLabel->resize(coatiLogo.width(), coatiLogo.height());
	coatiLogoLabel->move(30,10);
}

void QtSettingsWindow::showButtons(QVBoxLayout* layout)
{
	m_cancelButton = new QPushButton("Cancel");
	m_cancelButton->setObjectName("windowButton");
	m_doneButton = new QPushButton("Done");
	m_doneButton->setObjectName("windowButton");

	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(handleCancelButtonPress()));
	connect(m_doneButton, SIGNAL(clicked()), this, SLOT(handleUpdateButtonPress()));

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(m_cancelButton);
	buttons->addStretch();
	buttons->addWidget(m_doneButton);
	m_buttonsLayout = buttons;

	layout->addLayout(buttons);
}

void QtSettingsWindow::updateTitle(QString title)
{
	if (m_title)
	{
		m_title->setText(title);
	}
}

void QtSettingsWindow::updateDoneButton(QString text)
{
	if (m_doneButton)
	{
		m_doneButton->setText(text);
	}
}

void QtSettingsWindow::hideCancelButton(bool hidden)
{
	if(m_cancelButton)
	{
		m_cancelButton->setVisible(!hidden);
	}
}
