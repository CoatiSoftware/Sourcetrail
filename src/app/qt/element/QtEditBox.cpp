#include "qt/element/QtEditBox.h"

QtEditBox::QtEditBox(QWidget *parent)
	: QLineEdit(parent)
	, m_onReturnPressed(nullptr)
{
	connect(this, SIGNAL(returnPressed()), this, SLOT(slotOnReturnPressed()));
}

QtEditBox::~QtEditBox()
{
}

void QtEditBox::setCallbackOnReturnPressed(std::function<void(void)> callback)
{
	m_onReturnPressed = callback;
}

void QtEditBox::slotOnReturnPressed()
{
	if (m_onReturnPressed)
		m_onReturnPressed();
}
