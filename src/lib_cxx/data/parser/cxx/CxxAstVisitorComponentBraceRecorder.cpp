#include "CxxAstVisitorComponentBraceRecorder.h"

#include <clang/Lex/Preprocessor.h>

#include "CxxAstVisitorComponentContext.h"
#include "utilityClang.h"
#include "ParseLocation.h"
#include "ParserClient.h"

CxxAstVisitorComponentBraceRecorder::CxxAstVisitorComponentBraceRecorder(
	CxxAstVisitor* astVisitor, clang::ASTContext* astContext, std::shared_ptr<ParserClient> client
)
	: CxxAstVisitorComponent(astVisitor)
	, m_astContext(astContext)
	, m_client(client)
{
}

void CxxAstVisitorComponentBraceRecorder::visitTagDecl(clang::TagDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		if (d->isThisDeclarationADefinition() &&
			(
				!clang::isa<clang::CXXRecordDecl>(d) ||
				clang::dyn_cast<clang::CXXRecordDecl>(d)->getTemplateSpecializationKind() != clang::TSK_ImplicitInstantiation
				))
		{
			recordBraces(getParseLocation(d->getBraceRange().getBegin()), getParseLocation(d->getBraceRange().getEnd()));
		}
	}
}

void CxxAstVisitorComponentBraceRecorder::visitNamespaceDecl(clang::NamespaceDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		recordBraces(
			getParseLocation(getFirstLBraceLocation(d->getLocStart())),
			getParseLocation(getLastRBraceLocation(d->getLocStart(), d->getLocEnd()))
		);
	}
}

void CxxAstVisitorComponentBraceRecorder::visitCompoundStmt(clang::CompoundStmt* s)
{
	if (getAstVisitor()->shouldVisitStmt(s))
	{
		const clang::NamedDecl* contextDecl = getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl();
		if (!contextDecl || !utility::isImplicit(contextDecl))
		{
			recordBraces(getParseLocation(s->getLBracLoc()), getParseLocation(s->getRBracLoc()));
		}
	}
}

void CxxAstVisitorComponentBraceRecorder::visitInitListExpr(clang::InitListExpr* s)
{
	if (getAstVisitor()->shouldVisitStmt(s))
	{
		if (s->isSyntacticForm())
		{
			const clang::NamedDecl* contextDecl = getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl();
			if (!contextDecl || !utility::isImplicit(contextDecl))
			{
				recordBraces(getParseLocation(s->getLBraceLoc()), getParseLocation(s->getRBraceLoc()));
			}
		}
	}
}

void CxxAstVisitorComponentBraceRecorder::visitMSAsmStmt(clang::MSAsmStmt* s)
{
	if (getAstVisitor()->shouldVisitStmt(s))
	{
		if (s->hasBraces())
		{
			const clang::NamedDecl* contextDecl = getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl();
			if (!contextDecl || !utility::isImplicit(contextDecl))
			{
				recordBraces(
					getParseLocation(s->getLBraceLoc()),
					getParseLocation(getLastRBraceLocation(s->getLocStart(), s->getLocEnd()))
				);
			}
		}
	}
}

ParseLocation CxxAstVisitorComponentBraceRecorder::getParseLocation(const clang::SourceLocation& loc) const
{
	return getAstVisitor()->getParseLocation(loc);
}

void CxxAstVisitorComponentBraceRecorder::recordBraces(const ParseLocation& lbraceLoc, const ParseLocation& rbraceLoc)
{
	std::wstring name =
		lbraceLoc.filePath.fileName() + L"<" +
		std::to_wstring(lbraceLoc.startLineNumber) + L":" +
		std::to_wstring(lbraceLoc.startColumnNumber) + L">";

	if (lbraceLoc.startColumnNumber != rbraceLoc.startColumnNumber ||
		lbraceLoc.endColumnNumber != rbraceLoc.endColumnNumber ||
		lbraceLoc.startLineNumber != rbraceLoc.startLineNumber ||
		lbraceLoc.endLineNumber != rbraceLoc.endLineNumber)
	{
		if (lbraceLoc.startColumnNumber == lbraceLoc.endColumnNumber &&
			lbraceLoc.startLineNumber == lbraceLoc.endLineNumber)
		{
			m_client->recordLocalSymbol(name, lbraceLoc);
		}
		if (rbraceLoc.startColumnNumber == rbraceLoc.endColumnNumber &&
			rbraceLoc.startLineNumber == rbraceLoc.endLineNumber)
		{
			m_client->recordLocalSymbol(name, rbraceLoc);
		}
	}
}

clang::SourceLocation CxxAstVisitorComponentBraceRecorder::getFirstLBraceLocation(clang::SourceLocation searchStartLoc) const
{
	const clang::SourceManager& sm = m_astContext->getSourceManager();
	const clang::LangOptions& opts = m_astContext->getLangOpts();

	{
		clang::Token token;
		if (clang::Lexer::getRawToken(searchStartLoc, token, sm, opts))
		{
			if (token.getKind() == clang::tok::l_brace)
			{
				return token.getLocation();
			}
		}
	}

	while (true)
	{
		llvm::Optional<clang::Token> token = clang::Lexer::findNextToken(searchStartLoc, sm, opts);
		if (token.hasValue())
		{
			if (token.getValue().getKind() == clang::tok::l_brace)
			{
				return token.getValue().getLocation();
			}
			searchStartLoc = token.getValue().getLocation();
		}
		else
		{
			break;
		}
	}
	return clang::SourceLocation();
}

clang::SourceLocation CxxAstVisitorComponentBraceRecorder::getLastRBraceLocation(clang::SourceLocation searchStartLoc, clang::SourceLocation searchEndLoc) const
{
	const clang::SourceManager& sm = m_astContext->getSourceManager();
	const clang::LangOptions& opts = m_astContext->getLangOpts();

	{
		searchEndLoc = searchEndLoc.getLocWithOffset(-1);
		llvm::Optional<clang::Token> token = clang::Lexer::findNextToken(searchEndLoc, sm, opts);
		if (token.hasValue())
		{
			if (token.getValue().getKind() == clang::tok::r_brace)
			{
				return token.getValue().getLocation();
			}
		}
	}

	while (true)
	{
		clang::Token token;
		if (clang::Lexer::getRawToken(searchEndLoc, token, sm, opts))
		{
			if (token.getKind() == clang::tok::r_brace)
			{
				return token.getLocation();
			}
		}
		if (searchEndLoc < searchStartLoc)
		{
			break;
		}
		searchEndLoc = searchEndLoc.getLocWithOffset(-1);
	}
	return clang::SourceLocation();
}
