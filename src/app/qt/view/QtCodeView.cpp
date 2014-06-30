#include "qt/view/QtCodeView.h"

#include <QtWidgets>

#include "qt/element/QtArea.h"
#include "qt/QtWidgetWrapper.h"
#include "qt/utility/QtHighLighter.h"

QtCodeView::QtCodeView(std::shared_ptr<ViewManager> viewManager)
	: CodeView(viewManager, std::make_shared<QtArea>())
{
	getRootElement()->setBackgroundColor(255, 125, 0, 255);

	std::shared_ptr<QWidget> widget = QtWidgetWrapper::getWidgetOfElement(getRootElement());
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setSpacing(3);
	layout->setContentsMargins(3, 3, 3, 3);
	widget->setLayout(layout);

	m_font.setFamily("Courier");
	m_font.setFixedPitch(true);
	m_font.setPointSize(10);
}

QtCodeView::~QtCodeView()
{
}

void QtCodeView::addCodeSnippet(std::string str)
{
	std::shared_ptr<Snippet> snippet = std::make_shared<Snippet>();

	snippet->textField = std::make_shared<QTextEdit>();
	snippet->textField->setReadOnly(true);
	snippet->textField->setFont(m_font);

	snippet->highlighter = std::make_shared<QtHighlighter>(snippet->textField->document());

	snippet->textField->setPlainText(QString::fromUtf8(str.c_str()));

	std::shared_ptr<QWidget> widget = QtWidgetWrapper::getWidgetOfElement(getRootElement());
	widget->layout()->addWidget(snippet->textField.get());

	m_snippets.push_back(snippet);
}

void QtCodeView::clearCodeSnippets()
{
	m_snippets.clear();
}
