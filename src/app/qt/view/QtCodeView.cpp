#include "qt/view/QtCodeView.h"

#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/QtWidgetWrapper.h"
#include "qt/utility/utilityQt.h"
#include "utility/FileSystem.h"
#include "utility/messaging/type/MessageActivateToken.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_clearCodeSnippetsFunctor(std::bind(&QtCodeView::doClearCodeSnippets, this))
	, m_addCodeSnippetFunctor(std::bind(&QtCodeView::doAddCodeSnippet, this, std::placeholders::_1))
{
}

QtCodeView::~QtCodeView()
{
}

void QtCodeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtWidgetWrapper>(std::make_shared<QScrollArea>()));
}

void QtCodeView::initGui()
{
	QWidget* widget = QtWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, Colori(255, 125, 0, 255));

	QScrollArea* scroll = dynamic_cast<QScrollArea*>(widget);
	m_frame = std::make_shared<QFrame>(scroll);

	QVBoxLayout* layout = new QVBoxLayout(m_frame.get());
	layout->setSpacing(10);
	layout->setContentsMargins(3, 3, 3, 3);
	layout->setAlignment(Qt::AlignTop);
	m_frame->setLayout(layout);

	scroll->setWidgetResizable(true);
	scroll->setWidget(m_frame.get());
}

void QtCodeView::addCodeSnippet(const CodeSnippetParams params)
{
	m_addCodeSnippetFunctor(params);
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

void QtCodeView::doAddCodeSnippet(const CodeSnippetParams params)
{
	std::string fileName = FileSystem::fileName(params.locationFile.getFilePath());
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
		m_frame->layout()->addWidget(file);
	}

	file->addCodeSnippet(params.code, params.locationFile, params.startLineNumber, params.activeTokenId);
}

void QtCodeView::doClearCodeSnippets()
{
	m_files.clear();
}
