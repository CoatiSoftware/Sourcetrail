#include "data/parser/cxx/ASTVisitor.h"

#include <clang/Lex/Lexer.h>

#include "data/parser/cxx/ASTBodyVisitor.h"
#include "data/parser/cxx/utilityCxx.h"
#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"

ASTVisitor::ASTVisitor(clang::ASTContext* context, ParserClient* client)
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

bool ASTVisitor::VisitTypedefDecl(clang::TypedefDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onTypedefParsed(
			getParseLocationForNamedDecl(declaration),
			declaration->getQualifiedNameAsString(),
			getParseTypeUsage(declaration->getTypeSourceInfo()->getTypeLoc(), declaration->getUnderlyingType()),
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
				getParseLocationForNamedDecl(declaration),
				declaration->getQualifiedNameAsString(),
				convertAccessType(declaration->getAccess()),
				getParseLocationOfRecordBody(declaration)
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
				getParseLocationForNamedDecl(declaration),
				declaration->getQualifiedNameAsString(),
				convertAccessType(declaration->getAccess()),
				getParseLocationOfRecordBody(declaration)
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
				getParseLocationForNamedDecl(declaration),
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
				getParseLocationForNamedDecl(declaration),
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
			getParseLocationForNamedDecl(declaration),
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
			getParseLocationForNamedDecl(declaration),
			getParseFunction(declaration),
			getParseLocationOfFunctionBody(declaration)
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
			getParseLocationForNamedDecl(declaration),
			getParseFunction(declaration),
			convertAccessType(declaration->getAccess()),
			abstraction,
			getParseLocationOfFunctionBody(declaration)
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
				clang::CXXCtorInitializer* init = *it;

				if (init->isMemberInitializer())
				{
					m_client->onFieldUsageParsed(
						getParseLocationForNamedDecl(init->getMember(), init->getMemberLocation()),
						getParseFunction(declaration),
						init->getMember()->getQualifiedNameAsString()
					);
				}
				else if (init->isBaseInitializer())
				{
					m_client->onTypeUsageParsed(
						getParseTypeUsage(init->getTypeSourceInfo()->getTypeLoc(), init->getTypeSourceInfo()->getType()),
						getParseFunction(declaration)
					);
				}

				ASTBodyVisitor bodyVisitor(this, declaration);
				bodyVisitor.Visit(init->getInit());
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
			declaration->isAnonymousNamespace() ? ParseLocation() : getParseLocationForNamedDecl(declaration),
			declaration->getQualifiedNameAsString(),
			getParseLocation(declaration->getSourceRange())
		);
	}

	return true;
}

bool ASTVisitor::VisitEnumDecl(clang::EnumDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		m_client->onEnumParsed(
			getParseLocationForNamedDecl(declaration),
			declaration->getQualifiedNameAsString(),
			convertAccessType(declaration->getAccess()),
			getParseLocation(declaration->getSourceRange())
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

void ASTVisitor::VisitCallExprInDeclBody(clang::FunctionDecl* decl, clang::CallExpr* expr)
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

	if (!expr->getDirectCallee())
	{
		// TODO: Save error at location.
		return;
	}

	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		getParseFunction(decl),
		getParseFunction(expr->getDirectCallee())
	);
}

void ASTVisitor::VisitCallExprInDeclBody(clang::VarDecl* decl, clang::CallExpr* expr)
{
	if (!expr->getDirectCallee())
	{
		// TODO: Save error at location.
		return;
	}

	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		getParseVariable(decl),
		getParseFunction(expr->getDirectCallee())
	);
}

void ASTVisitor::VisitCXXConstructExprInDeclBody(clang::FunctionDecl* decl, clang::CXXConstructExpr* expr)
{
	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		getParseFunction(decl),
		getParseFunction(expr->getConstructor())
	);
}

void ASTVisitor::VisitCXXConstructExprInDeclBody(clang::VarDecl* decl, clang::CXXConstructExpr* expr)
{
	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		getParseVariable(decl),
		getParseFunction(expr->getConstructor())
	);
}

void ASTVisitor::VisitMemberExprInDeclBody(clang::FunctionDecl* decl, clang::MemberExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getMemberNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onFieldUsageParsed(
		parseLocation,
		getParseFunction(decl),
		expr->getMemberDecl()->getQualifiedNameAsString()
	);
}

void ASTVisitor::VisitDeclRefExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onGlobalVariableUsageParsed(
		parseLocation,
		getParseFunction(decl),
		expr->getDecl()->getQualifiedNameAsString()
	);
}

void ASTVisitor::VisitVarDeclInDeclBody(clang::FunctionDecl* decl, clang::VarDecl* varDecl)
{
	m_client->onTypeUsageParsed(
		getParseTypeUsage(varDecl->getTypeSourceInfo()->getTypeLoc(), varDecl->getType()),
		getParseFunction(decl)
	);
}

bool ASTVisitor::hasValidLocation(const clang::Decl* declaration) const
{
	const clang::SourceLocation& location = declaration->getLocStart();
	return location.isValid() && m_context->getSourceManager().isWrittenInMainFile(location);
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

ParseLocation ASTVisitor::getParseLocation(const clang::SourceRange& sourceRange) const
{
	if (sourceRange.isInvalid())
	{
		return ParseLocation();
	}

	const clang::SourceManager& sourceManager = m_context->getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
	const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

	return ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn()
	);
}

ParseLocation ASTVisitor::getParseLocationForNamedDecl(clang::NamedDecl* decl, const clang::SourceLocation& loc) const
{
	const clang::SourceManager& sourceManager = m_context->getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(loc);

	return ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedBegin.getLine(),
		presumedBegin.getColumn() + decl->getNameAsString().size() - 1
	);
}

ParseLocation ASTVisitor::getParseLocationForNamedDecl(clang::NamedDecl* decl) const
{
	return getParseLocationForNamedDecl(decl, decl->getLocation());
}

ParseLocation ASTVisitor::getParseLocationOfFunctionBody(clang::FunctionDecl* decl) const
{
	if (decl->hasBody() && decl->isThisDeclarationADefinition())
	{
		return getParseLocation(decl->getSourceRange());
	}

	return ParseLocation();
}

ParseLocation ASTVisitor::getParseLocationOfRecordBody(clang::CXXRecordDecl* decl) const
{
	if (decl->hasDefinition() && decl->isThisDeclarationADefinition())
	{
		return getParseLocation(decl->getDefinition()->getSourceRange());
	}

	return ParseLocation();
}

ParseTypeUsage ASTVisitor::getParseTypeUsage(clang::TypeLoc typeLoc, const clang::QualType& type) const
{
	DataType dataType = utility::qualTypeToDataType(type);
	ParseLocation parseLocation;

	if (!typeLoc.isNull())
	{
		clang::SourceRange sr(
			typeLoc.getLocStart(),
			clang::Lexer::getLocForEndOfToken(typeLoc.getLocEnd(), 0, m_context->getSourceManager(), clang::LangOptions())
		);

		parseLocation = getParseLocation(sr);
		parseLocation.endColumnNumber -= 1;
	}

	return ParseTypeUsage(parseLocation, dataType);
}

ParseTypeUsage ASTVisitor::getParseTypeUsageOfReturnType(clang::FunctionDecl* declaration) const
{
	// TODO: use FunctionDecl::getReturnTypeSourceRange() in newer clang version
	clang::TypeLoc typeLoc;

	const clang::TypeSourceInfo *TSI = declaration->getTypeSourceInfo();
	if (TSI)
	{
		const clang::FunctionTypeLoc FTL = TSI->getTypeLoc().IgnoreParens().getAs<clang::FunctionTypeLoc>();
		typeLoc = FTL.getReturnLoc();
	}

	return getParseTypeUsage(typeLoc, declaration->getReturnType());
}

std::vector<ParseTypeUsage> ASTVisitor::getParameters(clang::FunctionDecl* declaration) const
{
	std::vector<ParseTypeUsage> parameters;

	for (unsigned i = 0; i < declaration->getNumParams(); i++)
	{
		clang::ParmVarDecl* paramDecl = declaration->getParamDecl(i);
		if (paramDecl->getTypeSourceInfo())
		{
			parameters.push_back(getParseTypeUsage(paramDecl->getTypeSourceInfo()->getTypeLoc(), paramDecl->getType()));
		}
	}

	return parameters;
}

ParseVariable ASTVisitor::getParseVariable(clang::DeclaratorDecl* declaration) const
{
	bool isStatic = false;
	if (clang::isa<clang::VarDecl>(declaration))
	{
		clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declaration);
		isStatic = varDecl->isStaticDataMember() || varDecl->getStorageClass() == clang::SC_Static;
	}

	return ParseVariable(
		getParseTypeUsage(declaration->getTypeSourceInfo()->getTypeLoc(), declaration->getType()),
		declaration->getQualifiedNameAsString(),
		isStatic
	);
}

ParseFunction ASTVisitor::getParseFunction(clang::FunctionDecl* declaration) const
{
	bool isStatic = false;
	bool isConst = false;
	if (clang::isa<clang::CXXMethodDecl>(declaration))
	{
		clang::CXXMethodDecl* methodDecl = clang::dyn_cast<clang::CXXMethodDecl>(declaration);
		isStatic = methodDecl->isStatic();
		isConst = methodDecl->isConst();
	}
	else
	{
		isStatic = declaration->getStorageClass() == clang::SC_Static;
	}

	return ParseFunction(
		getParseTypeUsageOfReturnType(declaration),
		declaration->getQualifiedNameAsString(),
		getParameters(declaration),
		isStatic,
		isConst
	);
}
