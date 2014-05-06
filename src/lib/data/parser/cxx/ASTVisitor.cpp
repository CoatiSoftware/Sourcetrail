#include "data/parser/cxx/ASTVisitor.h"

ASTVisitor::ASTVisitor(clang::ASTContext* context, std::shared_ptr<ParserClient> client)
	: m_context(context)
	, m_client(client)
{
}

ASTVisitor::~ASTVisitor()
{
}

bool ASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *declaration)
{
	clang::FullSourceLoc location = m_context->getFullLoc(declaration->getLocStart());

	if (location.isValid() && m_context->getSourceManager().isWrittenInMainFile(location))
	{
		ParseObject parseObject(
			declaration->getQualifiedNameAsString(),
			m_context->getSourceManager().getFilename(location),
			location.getSpellingLineNumber(),
			location.getSpellingColumnNumber()
		);

		m_client->addClass(parseObject);
	}

	return true;
}
