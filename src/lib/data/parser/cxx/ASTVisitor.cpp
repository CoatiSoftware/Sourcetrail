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

bool ASTVisitor::VisitTypedefDecl(const clang::TypedefDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onTypedefParsed(
			getParseLocation(declaration->getSourceRange()),
			declaration->getQualifiedNameAsString(),
			declaration->getUnderlyingType().getAsString(),
			convertAccessType(declaration->getAccess())
		);
	}

	return true;
}

bool ASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		if (declaration->isClass())
		{
			m_client->onClassParsed(
				getParseLocation(declaration->getSourceRange()),
				declaration->getQualifiedNameAsString(),
				convertAccessType(declaration->getAccess())
			);

			if (declaration->hasDefinition() && declaration->getNumBases())
			{
				for (const auto& it : declaration->bases())
				{
					m_client->onInheritanceParsed(
						getParseLocation(it.getSourceRange()),
						declaration->getQualifiedNameAsString(),
						getTypeName(it.getType()),
						convertAccessType(it.getAccessSpecifier())
					);
				}
			}
		}
		else if (declaration->isStruct())
		{
			m_client->onStructParsed(
				getParseLocation(declaration->getSourceRange()),
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

	if (hasValidLocation(declaration))
	{
		clang::AccessSpecifier access = declaration->getAccess();

		if (access == clang::AS_none)
		{
			m_client->onGlobalVariableParsed(
				getParseLocation(declaration->getSourceRange()),
				getParseVariable(declaration)
			);
		}
		else
		{
			m_client->onFieldParsed(
				getParseLocation(declaration->getSourceRange()),
				getParseVariable(declaration),
				convertAccessType(declaration->getAccess())
			);
		}
	}

	return true;
}

bool ASTVisitor::VisitFieldDecl(clang::FieldDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onFieldParsed(
			getParseLocation(declaration->getSourceRange()),
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

	if (hasValidLocation(declaration))
	{
		m_client->onFunctionParsed(
			getParseLocation(declaration->getSourceRange()),
			declaration->getQualifiedNameAsString(),
			getTypeName(declaration->getReturnType()),
			getParameters(declaration)
		);
	}

	return true;
}

bool ASTVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* declaration)
{
	if (hasValidLocation(declaration))
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
			getParseLocation(declaration->getSourceRange()),
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
	if (hasValidLocation(declaration))
	{
		m_client->onNamespaceParsed(
			getParseLocation(declaration->getSourceRange()),
			declaration->getQualifiedNameAsString()
		);
	}

	return true;
}

bool ASTVisitor::VisitEnumDecl(clang::EnumDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onEnumParsed(
			getParseLocation(declaration->getSourceRange()),
			declaration->getQualifiedNameAsString(),
			convertAccessType(declaration->getAccess())
		);
	}

	return true;
}

bool ASTVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onEnumFieldParsed(
			getParseLocation(declaration->getSourceRange()),
			declaration->getQualifiedNameAsString()
		);
	}

	return true;
}

bool ASTVisitor::hasValidLocation(const clang::Decl* declaration) const
{
	const clang::SourceLocation& location = declaration->getLocStart();
	return location.isValid() && m_context->getSourceManager().isWrittenInMainFile(location);
}

ParseLocation ASTVisitor::getParseLocation(const clang::SourceRange& sourceRange) const
{
	const clang::SourceManager& sourceManager = m_context->getSourceManager();

	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin());
	const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd());

	return ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn()
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
