#include "data/parser/cxx/CxxVerboseAstVisitor.h"

#include <sstream>

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"

#include "utility/logging/logging.h"
#include "utility/ScopedSwitcher.h"

CxxVerboseAstVisitor::CxxVerboseAstVisitor(clang::ASTContext* context, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister)
	: base(context, preprocessor, client, fileRegister)
	, m_currentFilePath("")
	, m_indentation(0)
{
}

CxxVerboseAstVisitor::~CxxVerboseAstVisitor()
{
}

bool CxxVerboseAstVisitor::TraverseDecl(clang::Decl *d)
{
	if (d)
	{
		std::stringstream stream;
		stream << getIndentString() << d->getDeclKindName() << "Decl";
		if (clang::NamedDecl *namedDecl = clang::dyn_cast_or_null<clang::NamedDecl>(d))
		{
			stream << " [" << obfuscateName(namedDecl->getNameAsString()) << "]";
		}

		ParseLocation loc = getParseLocation(d->getSourceRange());
		stream << " <" << loc.startLineNumber << ":" << loc.startColumnNumber << ", " << loc.endLineNumber << ":" << loc.endColumnNumber << ">";

		if (m_currentFilePath != loc.filePath.str())
		{
			m_currentFilePath = loc.filePath.str();
			LOG_INFO_STREAM_BARE(<< "Indexer - Traversing \"" + m_currentFilePath + "\"" );
		}

		LOG_INFO_STREAM_BARE(<< "Indexer - " << stream.str());

		{
			ScopedSwitcher<unsigned int> switcher(m_indentation, m_indentation + 1);
			return base::TraverseDecl(d);
		}
	}
	return true;
}

bool CxxVerboseAstVisitor::TraverseStmt(clang::Stmt *stmt)
{
	if (stmt)
	{
		ParseLocation loc = getParseLocation(stmt->getSourceRange());
		LOG_INFO_STREAM_BARE(
			<< "Indexer - "
			<< getIndentString() << stmt->getStmtClassName()
			<< " <" << loc.startLineNumber << ":" << loc.startColumnNumber
			<< ", " << loc.endLineNumber << ":" << loc.endColumnNumber << ">"
		);

		{
			ScopedSwitcher<unsigned int> switcher(m_indentation, m_indentation + 1);
			return base::TraverseStmt(stmt);
		}
	}
	return true;
}

bool CxxVerboseAstVisitor::TraverseTypeLoc(clang::TypeLoc tl)
{
	if (!tl.isNull())
	{
		ParseLocation loc = getParseLocation(tl.getSourceRange());
		LOG_INFO_STREAM_BARE(
			<< "Indexer - "
			<< getIndentString() << typeLocClassToString(tl)
			<< "TypeLoc <" << loc.startLineNumber << ":" << loc.startColumnNumber
			<< ", " << loc.endLineNumber << ":" << loc.endColumnNumber << ">"
		);
		{
			ScopedSwitcher<unsigned int> switcher(m_indentation, m_indentation + 1);
			return base::TraverseTypeLoc(tl);
		}
	}
	return true;
}

std::string CxxVerboseAstVisitor::getIndentString() const
{
	std::string indentString = "";
	for (unsigned int i = 0; i < m_indentation; i++)
	{
		indentString += "| ";
	}
	return indentString;
}

std::string CxxVerboseAstVisitor::obfuscateName(const std::string& name) const
{
	if (name.length() <= 2)
	{
		return name;
	}
	return name.substr(0, 1) + ".." + name.substr(name.length() - 1);
}
