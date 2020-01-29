#include "QtIndexingDialog.h"

#include <QLabel>
#include <QPushButton>

#include "MessageErrorsHelpMessage.h"
#include "QtDeviceScaledPixmap.h"
#include "QtHelpButton.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QLabel* QtIndexingDialog::createTitleLabel(const QString &title, QBoxLayout* layout)
{
	QLabel* label = new QLabel(title);
	label->setObjectName(QStringLiteral("title"));
	label->setAlignment(Qt::AlignRight | Qt::AlignBottom);

	if (layout)
	{
		layout->addWidget(label, 0, Qt::AlignRight);
	}

	return label;
}

QLabel* QtIndexingDialog::createMessageLabel(QBoxLayout* layout)
{
	QLabel* label = new QLabel();
	label->setObjectName(QStringLiteral("message"));
	label->setAlignment(Qt::AlignRight);
	label->setWordWrap(true);
	layout->addWidget(label);
	return label;
}

QWidget* QtIndexingDialog::createErrorWidget(QBoxLayout* layout)
{
	QWidget* errorWidget = new QWidget();
	QHBoxLayout* errorLayout = new QHBoxLayout(errorWidget);
	errorLayout->setContentsMargins(0, 0, 0, 0);
	errorLayout->setSpacing(5);

	QPushButton* errorCount = new QPushButton();
	errorCount->setObjectName(QStringLiteral("errorCount"));
	errorCount->setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac

	errorCount->setIcon(QPixmap(QString::fromStdWString(
		ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/error.png").wstr())));
	errorLayout->addWidget(errorCount);

	QtHelpButton* helpButton = new QtHelpButton(QStringLiteral("aaa"), QStringLiteral("bbb"));
	helpButton->setColor(Qt::white);

	helpButton->disconnect();
	connect(helpButton, &QtHelpButton::clicked, []() { MessageErrorsHelpMessage(true).dispatch(); });

	errorLayout->addWidget(helpButton);

	layout->addWidget(errorWidget, 0, Qt::AlignRight);
	errorWidget->hide();
	return errorWidget;
}

QLabel* QtIndexingDialog::createFlagLabel(QWidget* parent)
{
	QtDeviceScaledPixmap flag(QString::fromStdWString(
		ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/flag.png").wstr()));
	flag.scaleToWidth(120);

	QLabel* flagLabel = new QLabel(parent);
	flagLabel->setPixmap(flag.pixmap());
	flagLabel->resize(flag.width(), flag.height());
	flagLabel->move(15, 75);
	flagLabel->show();

	return flagLabel;
}


QtIndexingDialog::QtIndexingDialog(bool isSubWindow, QWidget* parent)
	: QtWindowBase(isSubWindow, parent)
{
	m_window->setStyleSheet(
		m_window->styleSheet() +
		QStringLiteral("#window { "
		"background: #2E3C86;"
		"border: none;"
		"}"));

	setStyleSheet(
		(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")) +
		 utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/"
																		L"indexing_dialog.css")))
			.c_str());

	m_layout = new QVBoxLayout(this);
	m_layout->setContentsMargins(20, 20, 20, 0);
	m_layout->setSpacing(3);

	m_content->setLayout(m_layout);
}

void QtIndexingDialog::resizeEvent(QResizeEvent* event)
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

	const QSize windowSize = size - QSize(30, 30);

	m_window->resize(windowSize);
	m_window->move(15, 15);
}

void QtIndexingDialog::setupDone()
{
	QSize actualSize = m_window->sizeHint() + QSize(50, 50);
	QSize preferredSize = sizeHint();

	QSize size(
		qMax(actualSize.width(), preferredSize.width()),
		qMax(actualSize.height(), preferredSize.height()));
	resize(size);

	moveToCenter();
}
