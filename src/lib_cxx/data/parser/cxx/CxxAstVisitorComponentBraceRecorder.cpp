#include "CxxAstVisitorComponentBraceRecorder.h"

#include <clang/Lex/Preprocessor.h>

#include "CanonicalFilePathCache.h"
#include "CxxAstVisitor.h"
#include "CxxAstVisitorComponentContext.h"
#include "utilityClang.h"
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
			recordBraces(
				getFilePath(d->getBraceRange().getBegin()),
				getParseLocation(d->getBraceRange().getBegin()),
				getParseLocation(d->getBraceRange().getEnd())
			);
		}
	}
}

void CxxAstVisitorComponentBraceRecorder::visitNamespaceDecl(clang::NamespaceDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		recordBraces(
			getFilePath(d->getBeginLoc()),
			getParseLocation(getFirstLBraceLocation(d->getBeginLoc(), d->getEndLoc())),
			getParseLocation(getLastRBraceLocation(d->getBeginLoc(), d->getEndLoc()))
		);
	}
}

void CxxAstVisitorComponentBraceRecorder::visitCompoundStmt(clang::CompoundStmt* s)
{
	if (getAstVisitor()->shouldVisitStmt(s))
	{
		const clang::NamedDecl* contextDecl =
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl();
		if (!contextDecl || !utility::isImplicit(contextDecl))
		{
			recordBraces(
				getFilePath(s->getLBracLoc()),
				getParseLocation(s->getLBracLoc()),
				getParseLocation(s->getRBracLoc())
			);
		}
	}
}

void CxxAstVisitorComponentBraceRecorder::visitInitListExpr(clang::InitListExpr* s)
{
	if (getAstVisitor()->shouldVisitStmt(s))
	{
		if (s->isSyntacticForm())
		{
			const clang::NamedDecl* contextDecl =
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl();
			if (!contextDecl || !utility::isImplicit(contextDecl))
			{
				recordBraces(
					getFilePath(s->getLBraceLoc()),
					getParseLocation(s->getLBraceLoc()),
					getParseLocation(s->getRBraceLoc()));
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
			const clang::NamedDecl* contextDecl =
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl();
			if (!contextDecl || !utility::isImplicit(contextDecl))
			{
				recordBraces(
					getFilePath(s->getLBraceLoc()),
					getParseLocation(s->getLBraceLoc()),
					getParseLocation(getLastRBraceLocation(s->getBeginLoc(), s->getEndLoc()))
				);
			}
		}
	}
}

ParseLocation CxxAstVisitorComponentBraceRecorder::getParseLocation(const clang::SourceLocation& loc) const
{
	return getAstVisitor()->getParseLocation(loc);
}

FilePath CxxAstVisitorComponentBraceRecorder::getFilePath(const clang::SourceLocation& loc)
{
	const clang::SourceManager& sm = m_astContext->getSourceManager();
	return getAstVisitor()->getCanonicalFilePathCache()->getCanonicalFilePath(sm.getFileID(loc), sm);
}

void CxxAstVisitorComponentBraceRecorder::recordBraces(
	const FilePath& filePath, const ParseLocation& lbraceLoc, const ParseLocation& rbraceLoc)
{
	if (lbraceLoc.startColumnNumber != rbraceLoc.startColumnNumber ||
		lbraceLoc.endColumnNumber != rbraceLoc.endColumnNumber ||
		lbraceLoc.startLineNumber != rbraceLoc.startLineNumber ||
		lbraceLoc.endLineNumber != rbraceLoc.endLineNumber)
	{
		std::wstring name =
			filePath.fileName() + L"<" +
			std::to_wstring(lbraceLoc.startLineNumber) + L":" +
			std::to_wstring(lbraceLoc.startColumnNumber) + L">";

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

clang::SourceLocation CxxAstVisitorComponentBraceRecorder::getFirstLBraceLocation(
	clang::SourceLocation searchStartLoc, clang::SourceLocation searchEndLoc) const
{
	const clang::SourceManager& sm = m_astContext->getSourceManager();
	const clang::LangOptions& opts = m_astContext->getLangOpts();

	searchStartLoc = sm.getExpansionLoc(searchStartLoc);
	searchEndLoc = sm.getExpansionLoc(searchEndLoc);

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

		if (searchEndLoc < searchStartLoc)
		{
			break;
		}
	}
	return clang::SourceLocation();
}

clang::SourceLocation CxxAstVisitorComponentBraceRecorder::getLastRBraceLocation(
	clang::SourceLocation searchStartLoc, clang::SourceLocation searchEndLoc) const
{
	const clang::SourceManager& sm = m_astContext->getSourceManager();
	const clang::LangOptions& opts = m_astContext->getLangOpts();

	searchStartLoc = sm.getExpansionLoc(searchStartLoc);
	searchEndLoc = sm.getExpansionLoc(searchEndLoc);

	{
		searchEndLoc = searchEndLoc.getLocWithOffset(-1);
		llvm::Optional<clang::Token> token = clang::Lexer::findNextToken(searchEndLoc, sm, opts);
		if (token.hasValue() && token.getValue().getKind() == clang::tok::r_brace)
		{
			return token.getValue().getLocation();
		}
	}

	while (true)
	{
		clang::Token token;
		if (clang::Lexer::getRawToken(searchEndLoc, token, sm, opts) && token.getKind() == clang::tok::r_brace)
		{
			return token.getLocation();
		}

		if (searchEndLoc < searchStartLoc)
		{
			break;
		}
		searchEndLoc = searchEndLoc.getLocWithOffset(-1);
	}
	return clang::SourceLocation();
}
