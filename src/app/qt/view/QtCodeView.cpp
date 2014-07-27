#include "qt/view/QtCodeView.h"

#include "qt/element/QtCodeFileList.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/text/TextAccess.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_clearCodeSnippetsFunctor(std::bind(&QtCodeView::doClearCodeSnippets, this))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
	, m_addCodeSnippetFunctor(std::bind(&QtCodeView::doAddCodeSnippet, this, std::placeholders::_1))
{
}

QtCodeView::~QtCodeView()
{
}

void QtCodeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(
		std::shared_ptr<QtCodeFileList>(createQtCodeFileList())
	));
}

void QtCodeView::initView()
{
}

void QtCodeView::refreshView()
{
	m_refreshViewFunctor();
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::addCodeSnippet(const CodeSnippetParams& params)
{
	m_addCodeSnippetFunctor(params);
}

void QtCodeView::clearCodeSnippets()
{
	m_clearCodeSnippetsFunctor();
}

void QtCodeView::doRefreshView()
{
	setStyleSheet(getQtCodeFileList());
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	QtCodeFileList* list = createQtCodeFileList();
	list->addCodeSnippet(1, params.code, params.locationFile, params.activeTokenIds);
	list->show();
}

void QtCodeView::doAddCodeSnippet(const CodeSnippetParams& params)
{
	getQtCodeFileList()->addCodeSnippet(params.startLineNumber, params.code, params.locationFile, params.activeTokenIds);
}

void QtCodeView::doClearCodeSnippets()
{
	getQtCodeFileList()->clearCodeSnippets();
}

QtCodeFileList* QtCodeView::getQtCodeFileList() const
{
	return dynamic_cast<QtCodeFileList*>(QtViewWidgetWrapper::getWidgetOfView(this));
}

QtCodeFileList* QtCodeView::createQtCodeFileList() const
{
	QtCodeFileList* list = new QtCodeFileList();
	setStyleSheet(list);
	return list;
}

void QtCodeView::setStyleSheet(QWidget* widget) const
{
	widget->setStyleSheet(TextAccess::createFromFile("data/gui/code_view/code_view.css")->getText().c_str());
}
