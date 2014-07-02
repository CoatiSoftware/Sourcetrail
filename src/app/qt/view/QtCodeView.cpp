#include "qt/view/QtCodeView.h"

#include <QtWidgets>

#include "qt/QtWidgetWrapper.h"
#include "qt/utility/QtHighLighter.h"
#include "qt/utility/utilityQt.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
{
}

QtCodeView::~QtCodeView()
{
}

void QtCodeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtWidgetWrapper>(std::make_shared<QFrame>()));
}

void QtCodeView::initGui()
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, Colori(255, 125, 0, 255));

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setSpacing(3);
	layout->setContentsMargins(3, 3, 3, 3);
	widget->setLayout(layout);

	m_font.setFamily("Courier");
	m_font.setFixedPitch(true);
	m_font.setPointSize(10);
}

void QtCodeView::addCodeSnippet(std::string str)
{
	std::shared_ptr<Snippet> snippet = std::make_shared<Snippet>();

	snippet->textField = std::make_shared<QTextEdit>();
	snippet->textField->setReadOnly(true);
	snippet->textField->setFont(m_font);

	snippet->highlighter = std::make_shared<QtHighlighter>(snippet->textField->document());

	snippet->textField->setPlainText(QString::fromUtf8(str.c_str()));

	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);
	widget->layout()->addWidget(snippet->textField.get());

	m_snippets.push_back(snippet);
}

void QtCodeView::clearCodeSnippets()
{
	m_snippets.clear();
}
