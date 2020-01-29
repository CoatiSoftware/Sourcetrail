#include "QtKnownProgressDialog.h"

#include <QPushButton>

QtKnownProgressDialog::QtKnownProgressDialog(bool hideable, QWidget* parent)
	: QtProgressBarDialog(0.5, true, parent)
{
	setSizeGripStyle(false);

	updateTitle(QStringLiteral("Progress"));

	m_layout->addStretch();

	if (hideable)
	{
		QHBoxLayout* buttons = new QHBoxLayout();
		buttons->addStretch();

		QPushButton* hideButton = new QPushButton(QStringLiteral("Hide"));
		hideButton->setObjectName(QStringLiteral("windowButton"));
		hideButton->setDefault(true);
		connect(hideButton, &QPushButton::clicked, this, &QtKnownProgressDialog::onHidePressed);
		buttons->addWidget(hideButton);

		m_layout->addLayout(buttons);
	}

	setupDone();
	setGeometries();
}

QSize QtKnownProgressDialog::sizeHint() const
{
	return QSize(350, 280);
}

void QtKnownProgressDialog::closeEvent(QCloseEvent* event)
{
	emit visibleChanged(false);
}

void QtKnownProgressDialog::onHidePressed()
{
	emit visibleChanged(false);
}
