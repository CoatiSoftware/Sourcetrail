#include "qt/element/QtEditBox.h"
#include "utility/logging/logging.h"

QtEditBox::QtEditBox(QWidget *parent)
	: QLineEdit(parent)
	, m_onReturnPressed(nullptr)
	, m_onTextEdited(nullptr)
{
	connect(this, SIGNAL(returnPressed()), this, SLOT(slotOnReturnPressed()));
	connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(slotOnTextEdited(const QString&)));
}

QtEditBox::~QtEditBox()
{
}

void QtEditBox::setCallbackOnReturnPressed(std::function<void(void)> callback)
{
	m_onReturnPressed = callback;
}

void QtEditBox::setCallbackOnTextEdited(std::function<void(const std::string&)> callback)
{
	m_onTextEdited = callback;
}

void QtEditBox::slotOnReturnPressed()
{
	if (m_onReturnPressed)
	{
		m_onReturnPressed();
	}
}

void QtEditBox::slotOnTextEdited(const QString& text)
{
	if (m_onTextEdited)
	{
		m_onTextEdited(text.toStdString());
	}
}
