#include "qt/view/QtCodeView.h"

#include <iostream>
#include <QtWidgets>

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/QtWidgetWrapper.h"
#include "qt/utility/utilityQt.h"
#include "utility/FileSystem.h"
#include "utility/messaging/type/MessageActivateToken.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_clearCodeSnippetsFunctor(std::bind(&QtCodeView::doClearCodeSnippets, this))
	, m_addCodeSnippetFunctor(std::bind(&QtCodeView::doAddCodeSnippet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
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
}

void QtCodeView::addCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber)
{
	m_addCodeSnippetFunctor(str, locationFile, startLineNumber);
}

void QtCodeView::clearCodeSnippets()
{
	m_clearCodeSnippetsFunctor();
}

void QtCodeView::activateToken(Id tokenId) const
{
	MessageActivateToken message(tokenId);
	message.dispatch();
}

void QtCodeView::doAddCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber)
{
	std::string fileName = FileSystem::fileName(locationFile.getFilePath());
	QtCodeFile* file = nullptr;

	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		if (filePtr->getFileName() == fileName)
		{
			file = filePtr.get();
			break;
		}
	}

	if (!file)
	{
		QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(this, fileName, widget);

		m_files.push_back(filePtr);
		file = filePtr.get();
		widget->layout()->addWidget(file);
	}

	file->addCodeSnippet(str, locationFile, startLineNumber);
}

void QtCodeView::doClearCodeSnippets()
{
	m_files.clear();
}
