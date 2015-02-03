#include "data/parser/cxx/ASTVisitor.h"

#include <clang/AST/ParentMap.h>
#include <clang/Lex/Lexer.h>

#include "data/parser/cxx/ASTBodyVisitor.h"
#include "data/parser/cxx/utilityCxx.h"
#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

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
			utility::getDeclNameHierarchy(declaration),
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
				utility::getDeclNameHierarchy(declaration),
				convertAccessType(declaration->getAccess()),
				getParseLocationOfRecordBody(declaration)
			);
		}
		else if (declaration->isStruct())
		{
			m_client->onStructParsed(
				getParseLocationForNamedDecl(declaration),
				utility::getDeclNameHierarchy(declaration),
				convertAccessType(declaration->getAccess()),
				getParseLocationOfRecordBody(declaration)
			);
		}

		if (declaration->isClass() || declaration->isStruct())
		{
			if (declaration->hasDefinition() && declaration->getNumBases())
			{
				for (const clang::CXXBaseSpecifier& it : declaration->bases())
				{
					m_client->onInheritanceParsed(
						getParseLocation(it.getSourceRange()),
						utility::getDeclNameHierarchy(declaration),
						utility::qualTypeToDataType(it.getType()).getTypeNameHierarchy(),
						convertAccessType(it.getAccessSpecifier())
					);
				}
			}
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

		if (declaration->hasBody() && declaration->getBody() != NULL && declaration->isThisDeclarationADefinition())
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
						utility::getDeclNameHierarchy(init->getMember())
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
			utility::getDeclNameHierarchy(declaration),
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
			utility::getDeclNameHierarchy(declaration),
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
			utility::getDeclNameHierarchy(declaration)
		);
	}

	return true;
}

bool ASTVisitor::VisitClassTemplateDecl(clang::ClassTemplateDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		std::vector<std::string> templateRecordNameHierarchy = utility::getDeclNameHierarchy(declaration);
		clang::TemplateParameterList* parameterList = declaration->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			clang::NamedDecl* namedDecl = parameterList->getParam(i);
			if (hasValidLocation(namedDecl))
			{
				m_client->onTemplateRecordParameterTypeParsed(
					getParseLocationForNamedDecl(namedDecl),
					namedDecl->getNameAsString(),
					templateRecordNameHierarchy
				);
			}
		}
	}

	// for implicit template specializations we do not need a valid location of the original template class definition (since that file could be included)
	// handles explicit specializations and implicit specializations but no explicit partial specializations
	for (clang::ClassTemplateDecl::spec_iterator it = declaration->specializations().begin();
		it != declaration->specializations().end(); it++
	)
	{
		clang::ClassTemplateSpecializationDecl* specializationDecl = *it;

		std::vector<std::string> specializationParentNameHierarchy = utility::getTemplateSpecializationParentNameHierarchy(specializationDecl);

		ParserClient::RecordType specializedRecordType = specializationDecl->isStruct() ? ParserClient::RECORD_STRUCT : ParserClient::RECORD_CLASS;
		std::vector<std::string> specializedRecordNameHierarchy = utility::getDeclNameHierarchy(specializationDecl);
		m_client->onTemplateRecordSpecializationParsed(
			getParseLocationForNamedDecl(*it), specializedRecordNameHierarchy, specializedRecordType, specializationParentNameHierarchy
		);

		const clang::TemplateArgumentList &argList = specializationDecl->getTemplateArgs();
		for (int i = 0; i < argList.size(); i++)
		{
			std::vector<std::string> argumentNameHierarchy = utility::qualTypeToDataType(argList.get(i).getAsType()).getTypeNameHierarchy();
			m_client->onTemplateRecordArgumentTypeParsed(ParseLocation(), argumentNameHierarchy, specializedRecordNameHierarchy); // TODO: What about the ParseLocation
		}
	}
	return true;
}

bool ASTVisitor::VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* declaration)
{
	if (hasValidLocation(declaration))
	{
		std::vector<std::string> specializedRecordNameHierarchy = utility::getDeclNameHierarchy(declaration);
		std::vector<std::string> specializationParentNameHierarchy = utility::getTemplateSpecializationParentNameHierarchy(declaration);

		ParserClient::RecordType specializedRecordType = declaration->isStruct() ? ParserClient::RECORD_STRUCT : ParserClient::RECORD_CLASS;
		m_client->onTemplateRecordSpecializationParsed(
			getParseLocationForNamedDecl(declaration), specializedRecordNameHierarchy, specializedRecordType, specializationParentNameHierarchy
		);

		clang::TemplateParameterList* parameterList = declaration->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			clang::NamedDecl* namedDecl = parameterList->getParam(i);
			if (hasValidLocation(namedDecl))
			{
				m_client->onTemplateRecordParameterTypeParsed(
					getParseLocationForNamedDecl(namedDecl),
					namedDecl->getNameAsString(),
					specializedRecordNameHierarchy
				);
			}
		}

		const clang::ASTTemplateArgumentListInfo* argumentInfoList = declaration->getTemplateArgsAsWritten();
		for (int i = 0; i < argumentInfoList->NumTemplateArgs; i++)
		{
			const clang::TemplateArgumentLoc& argumentLoc = argumentInfoList->operator[](i);
			const clang::QualType argumentType = argumentLoc.getArgument().getAsType();

			m_client->onTemplateRecordArgumentTypeParsed(
				getParseLocation(argumentLoc.getSourceRange()),
				utility::qualTypeToDataType(argumentType).getTypeNameHierarchy(),
				specializedRecordNameHierarchy);
		}
	}
	return true;
}

bool ASTVisitor::VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *declaration)
{
	if (hasValidLocation(declaration))
	{
		const ParseFunction templateFunction = getParseFunction(declaration->getTemplatedDecl());
		for (clang::FunctionTemplateDecl::spec_iterator it = declaration->specializations().begin(); it != declaration->specializations().end(); it++)
		{
			ParseLocation specializedFunctionLocation = getParseLocationForNamedDecl(*(it));
			ParseFunction specializedFunction = getParseFunction(*(it));
			m_client->onTemplateFunctionSpecializationParsed(
				specializedFunctionLocation,
				specializedFunction,
				templateFunction);

			m_client->onFunctionParsed(specializedFunctionLocation, specializedFunction, getParseLocationOfFunctionBody(*(it)));
		}

		clang::TemplateParameterList* parameterList = declaration->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			clang::NamedDecl* namedDecl = parameterList->getParam(i);

			if (hasValidLocation(namedDecl))
			{
				std::string templateParameterTypeName = namedDecl->getNameAsString();

				m_client->onTemplateFunctionParameterTypeParsed(
					getParseLocationForNamedDecl(namedDecl),
					templateParameterTypeName,
					templateFunction
				);
			}
		}
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
	std::string caller = decl->getNameAsString();
	std::string callee = expr->getConstructor()->getNameAsString();
	clang::SourceRange sourceRange = expr->getSourceRange();
//	expr->getParenOrBraceRange(); // is null when no parens found (for implicit constructor calls; maybe we will have to use this in the future)
//	expr->getNumArgs(); // and maybe we will need this one, too.. for same reasons as above.

	const clang::SourceManager& sourceManager = m_context->getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
	const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);
	int endLocationOffset = 0;

	bool oneCharacterLocation = (
		presumedBegin.getLine() == presumedEnd.getLine() && presumedBegin.getColumn() == presumedEnd.getColumn()
	);
	if  (oneCharacterLocation) // get the exact range of name of declard variable
	{
		clang::ParentMap pm(decl->getBody());
		clang::Stmt* parentStmt = pm.getParent(expr);
		if (parentStmt && parentStmt->getStmtClass() == clang::Stmt::DeclStmtClass)
		{
			clang::DeclStmt* declStmt = clang::dyn_cast<clang::DeclStmt>(parentStmt);
			clang::Decl* decl =  declStmt->getSingleDecl();
			if (clang::isa<clang::NamedDecl>(decl))
			{
				clang::NamedDecl* namedDecl = clang::dyn_cast<clang::NamedDecl>(decl);
				int variableNameLength = namedDecl->getName().size();
				endLocationOffset = variableNameLength - 1;
			}
		}
	}

	ParseLocation location(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn() + endLocationOffset
	);

	m_client->onCallParsed(
		location,
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

void ASTVisitor::VisitCXXNewExprInDeclBody(clang::FunctionDecl* decl, clang::CXXNewExpr* expr)
{
	m_client->onTypeUsageParsed(
		getParseTypeUsage(expr->getAllocatedTypeSourceInfo()->getTypeLoc(), expr->getAllocatedType()),
		getParseFunction(decl)
	);
}

void ASTVisitor::VisitCXXNewExprInDeclBody(clang::VarDecl* decl, clang::CXXNewExpr* expr)
{
	m_client->onTypeUsageParsed(
		getParseTypeUsage(expr->getAllocatedTypeSourceInfo()->getTypeLoc(), expr->getAllocatedType()),
		getParseVariable(decl)
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
		utility::getDeclNameHierarchy(expr->getMemberDecl())
	);
}

void ASTVisitor::VisitGlobalVariableExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onGlobalVariableUsageParsed(
		parseLocation,
		getParseFunction(decl),
		utility::getDeclNameHierarchy(expr->getDecl())
	);
}

void ASTVisitor::VisitEnumExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onEnumFieldUsageParsed(
		parseLocation,
		getParseFunction(decl),
		utility::getDeclNameHierarchy(expr->getDecl())
	);
}

void ASTVisitor::VisitEnumExprInDeclBody(clang::VarDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onEnumFieldUsageParsed(
		parseLocation,
		getParseVariable(decl),
		utility::getDeclNameHierarchy(expr->getDecl())
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
	clang::TypeLoc typeLoc;

	const clang::TypeSourceInfo *TSI = declaration->getTypeSourceInfo();
	if (TSI)
	{
		const clang::FunctionTypeLoc FTL = TSI->getTypeLoc().IgnoreParens().getAs<clang::FunctionTypeLoc>();
		if (FTL)
		{
			typeLoc = FTL.getReturnLoc();
		}
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
	std::vector<std::string> hameHierarchy = utility::getDeclNameHierarchy(declaration);
	if (clang::isa<clang::VarDecl>(declaration))
	{
		clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declaration);
		isStatic = varDecl->isStaticDataMember() || varDecl->getStorageClass() == clang::SC_Static;
	}
	else if (clang::isa<clang::FieldDecl>(declaration))
	{
		isStatic = false; // fieldDecls cannot be static. If they are, they are treated as VarDecls
	}

	return ParseVariable(
		getParseTypeUsage(declaration->getTypeSourceInfo()->getTypeLoc(), declaration->getType()),
		hameHierarchy,
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
		utility::getDeclNameHierarchy(declaration),
		getParameters(declaration),
		isStatic,
		isConst
	);
}
