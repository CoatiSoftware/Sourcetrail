#include "QtUnknownProgressDialog.h"

#include <QPushButton>

QtUnknownProgressDialog::QtUnknownProgressDialog(bool hideable, QWidget* parent)
	: QtProgressBarDialog(0.5, true, parent)
{
	setSizeGripStyle(false);

	updateTitle(QStringLiteral("Status"));

	m_layout->addStretch();

	if (hideable)
	{
		QHBoxLayout* buttons = new QHBoxLayout();
		buttons->addStretch();

		QPushButton* hideButton = new QPushButton(QStringLiteral("Hide"));
		hideButton->setObjectName(QStringLiteral("windowButton"));
		hideButton->setDefault(true);
		connect(hideButton, &QPushButton::clicked, this, &QtUnknownProgressDialog::onHidePressed);
		buttons->addWidget(hideButton);

		m_layout->addLayout(buttons);
	}

	setUnknownProgress();

	setupDone();
	setGeometries();
}

QSize QtUnknownProgressDialog::sizeHint() const
{
	return QSize(350, 280);
}

void QtUnknownProgressDialog::closeEvent(QCloseEvent* event)
{
	emit visibleChanged(false);
}

void QtUnknownProgressDialog::onHidePressed()
{
	emit visibleChanged(false);
}
