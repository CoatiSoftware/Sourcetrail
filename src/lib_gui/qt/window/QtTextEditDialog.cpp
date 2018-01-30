#include "qt/window/QtTextEditDialog.h"

#include <QLabel>
#include <QPlainTextEdit>

QtTextEditDialog::QtTextEditDialog(const QString& title, const QString& description, QWidget* parent)
	: QtWindow(false, parent)
	, m_title(title)
	, m_description(description)
{
}

QSize QtTextEditDialog::sizeHint() const
{
	return QSize(550, 550);
}

void QtTextEditDialog::setText(const std::string& text)
{
	m_text->setPlainText(QString::fromStdString(text));
}

std::string QtTextEditDialog::getText()
{
	return m_text->toPlainText().toStdString();
}

void QtTextEditDialog::setWText(const std::wstring& text)
{
	m_text->setPlainText(QString::fromStdWString(text));
}

std::wstring QtTextEditDialog::getWText()
{
	return m_text->toPlainText().toStdWString();
}

void QtTextEditDialog::setReadOnly(bool readOnly)
{
	m_text->setReadOnly(readOnly);

	if (readOnly)
	{
		updateNextButton("OK");
	}
	else
	{
		updateNextButton("Save");
	}
}

void QtTextEditDialog::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QLabel* description = new QLabel(m_description);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description);

	m_text = new QPlainTextEdit();
	m_text->setObjectName("textField");
	m_text->setLineWrapMode(QPlainTextEdit::NoWrap);
	m_text->setTabStopWidth(8 * m_text->fontMetrics().width('9'));
	layout->addWidget(m_text);

	widget->setLayout(layout);
}

void QtTextEditDialog::windowReady()
{
	updateNextButton("Save");
	updateCloseButton("Cancel");

	setPreviousVisible(false);

	updateTitle(m_title);
}
