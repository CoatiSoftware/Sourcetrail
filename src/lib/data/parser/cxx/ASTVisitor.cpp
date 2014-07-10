#include "data/parser/cxx/ASTVisitor.h"

#include "data/parser/cxx/ASTBodyVisitor.h"
#include "data/parser/cxx/utilityCxx.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"

ASTVisitor::ASTVisitor(clang::ASTContext* context, std::shared_ptr<ParserClient> client)
	: m_context(context)
	, m_client(client)
{
}

ASTVisitor::~ASTVisitor()
{
}

bool ASTVisitor::VisitStmt(const clang::Stmt* statement)
{
	return true;
}

bool ASTVisitor::VisitTypedefDecl(const clang::TypedefDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onTypedefParsed(
			getParseLocation(declaration->getSourceRange()),
			declaration->getQualifiedNameAsString(),
			utility::qualTypeToDataType(declaration->getUnderlyingType()),
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

			if (declaration->getInit())
			{
				ASTBodyVisitor bodyVisitor(this, declaration);
				bodyVisitor.Visit(declaration->getInit());
			}
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
			utility::qualTypeToDataType(declaration->getReturnType()),
			getParameters(declaration)
		);

		if (declaration->hasBody() && declaration->isThisDeclarationADefinition())
		{
			ASTBodyVisitor bodyVisitor(this, declaration);
			bodyVisitor.Visit(declaration->getBody());
		}
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
			utility::qualTypeToDataType(declaration->getReturnType()),
			getParameters(declaration),
			convertAccessType(declaration->getAccess()),
			abstraction,
			declaration->isConst(),
			declaration->isStatic()
		);

		if (declaration->hasBody() && declaration->isThisDeclarationADefinition())
		{
			ASTBodyVisitor bodyVisitor(this, declaration);
			bodyVisitor.Visit(declaration->getBody());
		}
	}

	return true;
}

bool ASTVisitor::VisitCXXConstructorDecl(clang::CXXConstructorDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		for (clang::CXXConstructorDecl::init_const_iterator it = declaration->init_begin(); it != declaration->init_end(); it++)
		{
			if ((*it)->getInit())
			{
				ASTBodyVisitor bodyVisitor(this, declaration);
				bodyVisitor.Visit((*it)->getInit());
			}
		}
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

void ASTVisitor::VisitCallExprInDeclBody(clang::NamedDecl* decl, clang::CallExpr* expr)
{
	// if (clang::FunctionDecl *CalleeDecl = CE->getDirectCallee())
	// {
	// 	return CalleeDecl;
	// }

	// clang::Expr *CEE = CE->getCallee()->IgnoreParenImpCasts();
	// if (clang::BlockExpr *Block = clang::dyn_cast<clang::BlockExpr>(CEE))
	// {
	// 	NumBlockCallEdges++;
	// 	return Block->getBlockDecl();
	// }

	// return nullptr;

	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		decl->getQualifiedNameAsString(),
		expr->getDirectCallee()->getQualifiedNameAsString()
	);
}

void ASTVisitor::VisitCXXConstructExprInDeclBody(clang::NamedDecl* decl, clang::CXXConstructExpr* expr)
{
	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		decl->getQualifiedNameAsString(),
		expr->getConstructor()->getQualifiedNameAsString()
	);
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
	clang::QualType qualType = declaration->getType();

	bool isStatic = false;
	if (clang::isa<clang::VarDecl>(declaration))
	{
		clang::VarDecl* varDecl = static_cast<clang::VarDecl*>(declaration);
		isStatic = varDecl->isStaticDataMember() || varDecl->getStorageClass() == clang::SC_Static;
	}

	return ParseVariable(
		utility::qualTypeToDataType(qualType),
		declaration->getQualifiedNameAsString(),
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

std::string ASTVisitor::getTypeName(const clang::QualType& qualType) const
{
	DataType dataType = utility::qualTypeToDataType(qualType);
	return dataType.getRawTypeName();
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
