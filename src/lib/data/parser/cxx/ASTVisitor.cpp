#include "data/parser/cxx/ASTVisitor.h"

#include "data/parser/ParseLocation.h"
#include "data/parser/ParseVariable.h"

ASTVisitor::ASTVisitor(clang::ASTContext* context, std::shared_ptr<ParserClient> client)
	: m_context(context)
	, m_client(client)
{
}

ASTVisitor::~ASTVisitor()
{
}

bool ASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* declaration)
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		if (declaration->isClass())
		{
			m_client->onClassParsed(
				getParseLocation(location),
				declaration->getQualifiedNameAsString(),
				convertAccessType(declaration->getAccess())
			);
		}
		else if (declaration->isStruct())
		{
			m_client->onStructParsed(
				getParseLocation(location),
				declaration->getQualifiedNameAsString(),
				convertAccessType(declaration->getAccess())
			);
		}
	}

	return true;
}

bool ASTVisitor::VisitVarDecl(clang::VarDecl* declaration)
{
	// Abort on local variables and parameters.
	if (declaration->isFunctionOrMethodVarDecl() || clang::isa<clang::ParmVarDecl>(declaration))
	{
		return true;
	}

	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		clang::AccessSpecifier access = declaration->getAccess();

		if (access == clang::AS_none)
		{
			m_client->onGlobalVariableParsed(getParseLocation(location), getParseVariable(declaration));
		}
		else
		{
			m_client->onFieldParsed(
				getParseLocation(location),
				getParseVariable(declaration),
				convertAccessType(declaration->getAccess())
			);
		}
	}

	return true;
}

bool ASTVisitor::VisitFieldDecl(clang::FieldDecl* declaration)
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		m_client->onFieldParsed(
			getParseLocation(location),
			getParseVariable(declaration),
			convertAccessType(declaration->getAccess())
		);
	}

	return true;
}

bool ASTVisitor::VisitFunctionDecl(clang::FunctionDecl* declaration)
{
	// Abort for CXXMethodDecls to avoid duplicate call.
	if (clang::isa<clang::CXXMethodDecl>(declaration))
	{
		return true;
	}

	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		m_client->onFunctionParsed(
			getParseLocation(location),
			declaration->getQualifiedNameAsString(),
			getTypeName(declaration->getReturnType()),
			getParameters(declaration)
		);
	}

	return true;
}

bool ASTVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* declaration)
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		ParserClient::AbstractionType abstraction = ParserClient::ABSTRACTION_NONE;
		if (declaration->isPure())
		{
			abstraction = ParserClient::ABSTRACTION_PURE_VIRTUAL;
		}
		else if (declaration->isVirtual())
		{
			abstraction = ParserClient::ABSTRACTION_VIRTUAL;
		}

		m_client->onMethodParsed(
			getParseLocation(location),
			declaration->getQualifiedNameAsString(),
			getTypeName(declaration->getReturnType()),
			getParameters(declaration),
			convertAccessType(declaration->getAccess()),
			abstraction,
			declaration->isConst(),
			declaration->isStatic()
		);
	}

	return true;
}

bool ASTVisitor::VisitNamespaceDecl(clang::NamespaceDecl* declaration)
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		m_client->onNamespaceParsed(getParseLocation(location), declaration->getQualifiedNameAsString());
	}

	return true;
}

bool ASTVisitor::VisitEnumDecl(clang::EnumDecl* declaration)
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		m_client->onEnumParsed(
			getParseLocation(location),
			declaration->getQualifiedNameAsString(),
			convertAccessType(declaration->getAccess())
		);
	}

	return true;
}

bool ASTVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl* declaration)
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (isValidLocation(location))
	{
		m_client->onEnumFieldParsed(getParseLocation(location), declaration->getQualifiedNameAsString());
	}

	return true;
}

bool ASTVisitor::isValidLocation(const clang::SourceLocation& location) const
{
	return location.isValid() && m_context->getSourceManager().isWrittenInMainFile(location);
}

ParseLocation ASTVisitor::getParseLocation(const clang::SourceLocation& location) const
{
	clang::FullSourceLoc fullLocation = m_context->getFullLoc(location);
	return ParseLocation(
		m_context->getSourceManager().getFilename(location),
		fullLocation.getSpellingLineNumber(),
		fullLocation.getSpellingColumnNumber()
	);
}

ParseVariable ASTVisitor::getParseVariable(clang::ValueDecl* declaration) const
{
	clang::QualType type = declaration->getType();

	bool isStatic = false;
	if (clang::isa<clang::VarDecl>(declaration))
	{
		isStatic = static_cast<clang::VarDecl*>(declaration)->isStaticDataMember();
	}

	return ParseVariable(
		getTypeName(type),
		declaration->getQualifiedNameAsString(),
		type.isConstQualified(),
		isStatic
	);
}

std::vector<ParseVariable> ASTVisitor::getParameters(clang::FunctionDecl* declaration) const
{
	std::vector<ParseVariable> parameters;

	for (unsigned i = 0; i < declaration->getNumParams(); i++)
	{
		parameters.push_back(getParseVariable(declaration->getParamDecl(i)));
	}

	return parameters;
}

std::string ASTVisitor::getTypeName(const clang::QualType& type) const
{
	std::string typeName = type.getUnqualifiedType().getAsString();

	// Remove keywords from type.
	std::string keyword;
	size_t pos = typeName.find_first_of(' ');
	if (pos != std::string::npos)
	{
		keyword = typeName.substr(0, pos);
	}

	if (keyword == "class" || keyword == "struct" || keyword == "enum")
	{
		typeName = typeName.substr(pos + 1);
	}

	return typeName;
}

ParserClient::AccessType ASTVisitor::convertAccessType(clang::AccessSpecifier access) const
{
	switch (access)
	{
	case clang::AS_public:
		return ParserClient::ACCESS_PUBLIC;
	case clang::AS_protected:
		return ParserClient::ACCESS_PROTECTED;
	case clang::AS_private:
		return ParserClient::ACCESS_PRIVATE;
	case clang::AS_none:
		return ParserClient::ACCESS_NONE;
	}
}
