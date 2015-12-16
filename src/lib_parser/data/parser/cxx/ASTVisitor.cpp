#include "data/parser/cxx/ASTVisitor.h"

#include <clang/AST/ParentMap.h>
#include <clang/Lex/Lexer.h>

#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

#include "data/parser/cxx/ASTBodyVisitor.h"
#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "data/parser/cxx/utilityCxx.h"
#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"
#include "data/type/NamedDataType.h"

ASTVisitor::ASTVisitor(clang::ASTContext* context, ParserClient* client, FileRegister* fileRegister)
	: m_context(context)
	, m_client(client)
	, m_fileRegister(fileRegister)
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
	if (isLocatedInUnparsedProjectFile(declaration))
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
	if (isLocatedInUnparsedProjectFile(declaration))
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

		if ((declaration->isClass() || declaration->isStruct()) &&
			declaration->hasDefinition() && declaration->getDefinition() == declaration && declaration->getNumBases())
		{
			for (const clang::CXXBaseSpecifier& it : declaration->bases())
			{
				m_client->onInheritanceParsed(
					getParseLocation(it.getSourceRange()),
					utility::getDeclNameHierarchy(declaration),
					utility::qualTypeToDataType(it.getType())->getTypeNameHierarchy(),
					convertAccessType(it.getAccessSpecifier())
				);

				// TODO: check for template class and add arguments!
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

	if (isLocatedInUnparsedProjectFile(declaration))
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
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		if (declaration->hasInClassInitializer())
		{
			ASTBodyVisitor bodyVisitor(this, declaration);
			bodyVisitor.Visit(declaration->getInClassInitializer());
		}
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

	if (isLocatedInUnparsedProjectFile(declaration))
	{
		processFunctionDecl(declaration);
	}

	return true;
}

bool ASTVisitor::VisitLambdaExpr(clang::LambdaExpr* expr)
{
	if (isLocatedInUnparsedProjectFile(expr->getCallOperator()))
	{
		processFunctionDecl(expr->getCallOperator());
	}
	return true;
}

bool ASTVisitor::VisitParmVarDecl(clang::ParmVarDecl* declaration)
{
	// todo: handle parameters here!
	// maybe handle template args in visitvardecl to account for local variables and stuff...
	clang::TypeLoc loc = declaration->getTypeSourceInfo()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}

	return true;
}

bool ASTVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		ParseFunction parseFunction = getParseFunction(declaration);
		ParseLocation location = getParseLocationForNamedDecl(declaration);

		m_client->onMethodParsed(
			location,
			parseFunction,
			convertAccessType(declaration->getAccess()),
			getAbstractionType(declaration),
			getParseLocationOfFunctionBody(declaration)
		);

		for (clang::CXXMethodDecl::method_iterator it = declaration->begin_overridden_methods();
			it != declaration->end_overridden_methods(); it++)
		{
			m_client->onMethodOverrideParsed(location, getParseFunction(*it), parseFunction);
		}

		if (declaration->hasBody() && declaration->getBody() != NULL && declaration->isThisDeclarationADefinition() &&
			!declaration->isDependentContext())
		{
			ASTBodyVisitor bodyVisitor(this, declaration);
			bodyVisitor.Visit(declaration->getBody());
		}
	}

	return true;
}

bool ASTVisitor::VisitCXXConstructorDecl(clang::CXXConstructorDecl* declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		for (clang::CXXConstructorDecl::init_const_iterator it = declaration->init_begin(); it != declaration->init_end(); it++)
		{
			if ((*it)->getInit())
			{
				clang::CXXCtorInitializer* init = *it;

				if (init->isMemberInitializer())
				{
					m_client->onFieldUsageParsed(
						getParseLocationForTokenAtLocation(init->getMemberLocation()),
						getParseFunction(declaration),
						utility::getDeclNameHierarchy(init->getMember())
					);
				}
				else if (init->isBaseInitializer())
				{
					m_client->onTypeUsageParsed(
						getParseTypeUsage(init->getTypeSourceInfo()->getTypeLoc(), init->getTypeSourceInfo()->getType()), // TODO: rewrite this to old!
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
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		m_client->onNamespaceParsed(
			declaration->isAnonymousNamespace() ? ParseLocation() : getParseLocationForNamedDecl(declaration), // TODO: why no real parse loc for anonymous namespace?
			utility::getDeclNameHierarchy(declaration),
			getParseLocation(declaration->getSourceRange())
		);
	}
	return true;
}

bool ASTVisitor::VisitEnumDecl(clang::EnumDecl* declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration))
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
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		m_client->onEnumConstantParsed(
			getParseLocation(declaration->getSourceRange()),
			utility::getDeclNameHierarchy(declaration)
		);
	}
	return true;
}

bool ASTVisitor::VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl *declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration) && declaration->hasDefaultArgument())
	{
		m_client->onTemplateDefaultArgumentTypeParsed(
			getParseTypeUsage(declaration->getDefaultArgumentInfo()->getTypeLoc(), declaration->getDefaultArgument()),
			utility::getDeclNameHierarchy(declaration)
		);
	}
	return true;
}

bool ASTVisitor::VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl *declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration) && declaration->hasDefaultArgument())
	{
		const clang::TemplateArgumentLoc& defaultArgumentLoc = declaration->getDefaultArgument();
		clang::SourceRange sr = defaultArgumentLoc.getSourceRange();
		std::shared_ptr<DataType> defaultArgumentDataType = std::make_shared<NamedDataType>(
			utility::getDeclNameHierarchy(defaultArgumentLoc.getArgument().getAsTemplate().getAsTemplateDecl())
		);
		m_client->onTemplateDefaultArgumentTypeParsed(
			getParseTypeUsage(sr, defaultArgumentDataType),
			utility::getDeclNameHierarchy(declaration)
		);
	}
	return true;
}

bool ASTVisitor::VisitClassTemplateDecl(clang::ClassTemplateDecl* declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		NameHierarchy templateRecordNameHierarchy = utility::getDeclNameHierarchy(declaration);
		clang::TemplateParameterList* parameterList = declaration->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			clang::NamedDecl* namedDecl = parameterList->getParam(i);
			if (!namedDecl->getName().empty()) // do not create node for template param if the param has no name
			{
				m_client->onTemplateRecordParameterTypeParsed(
					getParseLocationForNamedDecl(namedDecl),
					utility::getDeclNameHierarchy(namedDecl),
					templateRecordNameHierarchy
				);
			}
		}
	}

	// handles explicit specializations and implicit specializations but no explicit partial specializations
	for (clang::ClassTemplateDecl::spec_iterator it = declaration->specializations().begin();
		it != declaration->specializations().end(); it++
	)
	{
		clang::ClassTemplateSpecializationDecl* specializationDecl = *it;
		NameHierarchy specializationNameHierarchy = utility::getDeclNameHierarchy(specializationDecl);

		ParseLocation specializationLocation = getParseLocationForNamedDecl(specializationDecl);
		if (specializationDecl->getSpecializationKind() == clang::TSK_ImplicitInstantiation)
		{
			specializationLocation = getParseLocation(specializationDecl->getPointOfInstantiation());
		}

		// handling template arguments
		if (isLocatedInUnparsedProjectFile(specializationDecl) && specializationDecl->isExplicitSpecialization())
		{
			processTemplateArgumentsOfExplicitSpecialization(specializationDecl);
		}

		if (isLocatedInProjectFile(declaration))
		{
			m_client->onTemplateRecordSpecializationParsed(
				specializationLocation,
				specializationNameHierarchy,
				specializationDecl->isStruct() ? ParserClient::RECORD_STRUCT : ParserClient::RECORD_CLASS,
				utility::getTemplateSpecializationParentNameHierarchy(specializationDecl)
			);

			// template member specializations
			if (specializationDecl->getSpecializationKind() == clang::TSK_ImplicitInstantiation)
			{
				for (clang::CXXRecordDecl::method_iterator methodIt = specializationDecl->method_begin(); methodIt != specializationDecl->method_end(); methodIt++)
				{
					clang::CXXMethodDecl* methodDecl = (*methodIt);
					if (methodDecl->getTemplatedKind() == clang::FunctionDecl::TK_MemberSpecialization)
					{
						m_client->onMethodParsed(
							getParseLocation(methodDecl->getMemberSpecializationInfo()->getPointOfInstantiation()),
							getParseFunction(methodDecl),
							convertAccessType(methodDecl->getAccess()),
							getAbstractionType(methodDecl),
							ParseLocation()
						);

						clang::NamedDecl* specializedNamedDecel = methodDecl->getMemberSpecializationInfo()->getInstantiatedFrom();
						if (clang::isa<clang::FunctionDecl>(specializedNamedDecel))
						{
							m_client->onTemplateMemberFunctionSpecializationParsed(
								getParseLocation(methodDecl->getMemberSpecializationInfo()->getPointOfInstantiation()),
								getParseFunction(methodDecl),
								getParseFunction(clang::dyn_cast<clang::FunctionDecl>(specializedNamedDecel))
							);
						}
					}
				}
			}
		}
	}
	return true;
}

bool ASTVisitor::VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* declaration)
{
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		NameHierarchy specializedRecordNameHierarchy = utility::getDeclNameHierarchy(declaration);
		NameHierarchy specializationParentNameHierarchy = utility::getTemplateSpecializationParentNameHierarchy(declaration);

		ParserClient::RecordType specializedRecordType = declaration->isStruct() ? ParserClient::RECORD_STRUCT : ParserClient::RECORD_CLASS;
		m_client->onTemplateRecordSpecializationParsed(
			getParseLocationForNamedDecl(declaration), specializedRecordNameHierarchy, specializedRecordType, specializationParentNameHierarchy
		);

		clang::TemplateParameterList* parameterList = declaration->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			clang::NamedDecl* namedDecl = parameterList->getParam(i);
			if (!namedDecl->getName().empty()) // do not create node for template param if the param has no name
			{
				m_client->onTemplateRecordParameterTypeParsed(
					getParseLocationForNamedDecl(namedDecl),
					utility::getDeclNameHierarchy(namedDecl),
					specializedRecordNameHierarchy
				);
			}
		}

		processTemplateArgumentsOfExplicitSpecialization(declaration);
	}
	return true;
}

bool ASTVisitor::VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *declaration)
{
	const ParseFunction templateFunction = getParseFunction(declaration);
	if (isLocatedInUnparsedProjectFile(declaration))
	{
		clang::TemplateParameterList* parameterList = declaration->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			clang::NamedDecl* namedDecl = parameterList->getParam(i);
			if (!namedDecl->getName().empty()) // do not create node for template param if the param has no name
			{
				m_client->onTemplateFunctionParameterTypeParsed(
					getParseLocationForNamedDecl(namedDecl),
					utility::getDeclNameHierarchy(namedDecl),
					templateFunction
				);
			}
		}
	}

	for (clang::FunctionTemplateDecl::spec_iterator it = declaration->specializations().begin(); it != declaration->specializations().end(); it++)
	{
		clang::FunctionDecl* specializationDecl = *it;
		bool needsToProcessSpecialization = false;

		if (specializationDecl->getTemplateSpecializationKind() == clang::TSK_ExplicitSpecialization)
		{
			if (isLocatedInUnparsedProjectFile(declaration))
			{
				processTemplateArgumentsOfExplicitSpecialization(specializationDecl);
				needsToProcessSpecialization = true;
			}
		}
		else if (isLocatedInProjectFile(declaration))
		{
			needsToProcessSpecialization = true;
		}

		if (needsToProcessSpecialization)
		{
			processFunctionDecl(specializationDecl);

			m_client->onTemplateFunctionSpecializationParsed(
				getParseLocationForNamedDecl(specializationDecl),
				getParseFunction(specializationDecl),
				templateFunction
			);
		}

	}
	return true;
}

void ASTVisitor::VisitCallExprInDeclBody(clang::FunctionDecl* decl, clang::CallExpr* expr)
{
	clang::FunctionDecl* calleeFunctionDecl = expr->getDirectCallee();
	if (!calleeFunctionDecl)
	{
		// TODO: Save error at location.
		return;
	}

	m_client->onCallParsed(
		getParseLocation(expr->getSourceRange()),
		getParseFunction(decl),
		getParseFunction(calleeFunctionDecl)
	);
}

void ASTVisitor::VisitCallExprInDeclBody(clang::DeclaratorDecl* decl, clang::CallExpr* expr)
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


void ASTVisitor::VisitDeclRefExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr)
{
	processTemplateArguments(expr);
}

void ASTVisitor::VisitDeclRefExprInDeclBody(clang::DeclaratorDecl* decl, clang::DeclRefExpr* expr)
{
	processTemplateArguments(expr);
}

void ASTVisitor::VisitCXXConstructExprInDeclBody(clang::FunctionDecl* decl, clang::CXXConstructExpr* expr)
{
	m_client->onCallParsed(
		// getParseLocationForTokensInRange(expr->getSourceRange()),
		getParseLocation(expr->getSourceRange()),
		getParseFunction(decl),
		getParseFunction(expr->getConstructor())
	);
}

void ASTVisitor::VisitCXXConstructExprInDeclBody(clang::DeclaratorDecl* decl, clang::CXXConstructExpr* expr)
{
	m_client->onCallParsed(
		// getParseLocationForTokensInRange(expr->getSourceRange()),
		getParseLocation(expr->getSourceRange()),
		getParseVariable(decl),
		getParseFunction(expr->getConstructor())
	);
}

void ASTVisitor::VisitExplicitCastExprInDeclBody(clang::FunctionDecl* decl, clang::ExplicitCastExpr* expr)
{
	clang::TypeLoc loc = expr->getTypeInfoAsWritten()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
}

void ASTVisitor::VisitExplicitCastExprInDeclBody(clang::DeclaratorDecl* decl, clang::ExplicitCastExpr* expr)
{
	clang::TypeLoc loc = expr->getTypeInfoAsWritten()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
}

void ASTVisitor::VisitCXXTemporaryObjectExprInDeclBody(clang::FunctionDecl* decl, clang::CXXTemporaryObjectExpr* expr)
{
	clang::TypeLoc loc = expr->getTypeSourceInfo()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
}

void ASTVisitor::VisitCXXTemporaryObjectExprInDeclBody(clang::DeclaratorDecl* decl, clang::CXXTemporaryObjectExpr* expr)
{
	clang::TypeLoc loc = expr->getTypeSourceInfo()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
}

void ASTVisitor::VisitCXXNewExprInDeclBody(clang::FunctionDecl* decl, clang::CXXNewExpr* expr)
{
	m_client->onTypeUsageParsed(
		getParseTypeUsage(expr->getAllocatedTypeSourceInfo()->getTypeLoc(), expr->getAllocatedType()),
		getParseFunction(decl)
	);

	clang::TypeLoc loc = expr->getAllocatedTypeSourceInfo()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
}

void ASTVisitor::VisitCXXNewExprInDeclBody(clang::DeclaratorDecl* decl, clang::CXXNewExpr* expr)
{
	m_client->onTypeUsageParsed(
		getParseTypeUsage(expr->getAllocatedTypeSourceInfo()->getTypeLoc(), expr->getAllocatedType()),
		getParseVariable(decl)
	);

	clang::TypeLoc loc = expr->getAllocatedTypeSourceInfo()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
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

void ASTVisitor::VisitMemberExprInDeclBody(clang::DeclaratorDecl* decl, clang::MemberExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getMemberNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onFieldUsageParsed(
		parseLocation,
		getParseVariable(decl),
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

void ASTVisitor::VisitGlobalVariableExprInDeclBody(clang::DeclaratorDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onGlobalVariableUsageParsed(
		parseLocation,
		getParseVariable(decl),
		utility::getDeclNameHierarchy(expr->getDecl())
	);
}

void ASTVisitor::VisitEnumExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onEnumConstantUsageParsed(
		parseLocation,
		getParseFunction(decl),
		utility::getDeclNameHierarchy(expr->getDecl())
	);
}

void ASTVisitor::VisitEnumExprInDeclBody(clang::DeclaratorDecl* decl, clang::DeclRefExpr* expr)
{
	ParseLocation parseLocation = getParseLocation(expr->getSourceRange());

	const std::string exprName = expr->getNameInfo().getAsString();
	parseLocation.endColumnNumber += exprName.size() - 1;

	m_client->onEnumConstantUsageParsed(
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

	clang::TypeLoc loc = varDecl->getTypeSourceInfo()->getTypeLoc();
	if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
	{
		processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
	}
}

void ASTVisitor::processFunctionDecl(clang::FunctionDecl* declaration)
{
	m_client->onFunctionParsed(
		getParseLocationForNamedDecl(declaration),
		getParseFunction(declaration),
		getParseLocationOfFunctionBody(declaration)
	);

	// handle template arguments of return type.
	clang::TypeLoc functionLoc = declaration->getTypeSourceInfo()->getTypeLoc();
	if (functionLoc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::FunctionProto)
	{
		clang::TypeLoc returnLoc = functionLoc.getAs<clang::FunctionProtoTypeLoc>().getReturnLoc();
		if (returnLoc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
		{
			processTemplateArguments(returnLoc.getAs<clang::TemplateSpecializationTypeLoc>());
		}
	}


	if (declaration->hasBody() &&
		declaration->isThisDeclarationADefinition() &&
		!declaration->isDependentContext()
	)
	{
		ASTBodyVisitor bodyVisitor(this, declaration);
		bodyVisitor.Visit(declaration->getBody());
	}
}

void ASTVisitor::processTemplateArgumentsOfExplicitSpecialization(clang::FunctionDecl* specializationDecl)
{
	ParseFunction specializationFunction = getParseFunction(specializationDecl);

	if (specializationDecl->getTemplateSpecializationArgsAsWritten())
	{
		const clang::ASTTemplateArgumentListInfo* argumentInfoList = specializationDecl->getTemplateSpecializationArgsAsWritten();
		for (size_t i = 0; i < argumentInfoList->NumTemplateArgs; i++)
		{
			const clang::TemplateArgumentLoc& argumentLoc = argumentInfoList->operator[](i);

			const clang::TemplateArgument& argument = argumentLoc.getArgument();
			NameHierarchy argumentNameHierarchy = utility::templateArgumentToDataType(argument)->getTypeNameHierarchy();
			if (argumentNameHierarchy.size()) // FIXME: Some TemplateArgument kinds are not handled yet.
			{
				m_client->onTemplateArgumentTypeOfTemplateFunctionParsed(
					getParseLocation(argumentLoc.getSourceRange()),
					argumentNameHierarchy,
					specializationFunction
					);
			}
		}
	}
}

void ASTVisitor::processTemplateArgumentsOfExplicitSpecialization(clang::ClassTemplateSpecializationDecl* specializationDecl)
{
	NameHierarchy specializedRecordNameHierarchy = utility::getDeclNameHierarchy(specializationDecl);
	if (clang::ClassTemplatePartialSpecializationDecl* partialSpecializationDecl =
		clang::dyn_cast_or_null<clang::ClassTemplatePartialSpecializationDecl>(specializationDecl))
	{
		const clang::ASTTemplateArgumentListInfo* argumentInfoList = partialSpecializationDecl->getTemplateArgsAsWritten();
		for (size_t i = 0; i < argumentInfoList->NumTemplateArgs; i++)
		{
			const clang::TemplateArgumentLoc& argumentLoc = argumentInfoList->operator[](i);
			const clang::TemplateArgument& argument = argumentLoc.getArgument();

			NameHierarchy argumentNameHierarchy = utility::templateArgumentToDataType(argument)->getTypeNameHierarchy();
			if (argumentNameHierarchy.size()) // FIXME: Some TemplateArgument kinds are not handled yet.
			{
				m_client->onTemplateArgumentTypeOfTemplateRecordParsed(
					getParseLocationForTokensInRange(argumentLoc.getSourceRange()),
					argumentNameHierarchy,
					specializedRecordNameHierarchy
				);
			}
		}
	}
	else
	{
		if (clang::TypeSourceInfo* typeSourceInfo = specializationDecl->getTypeAsWritten())
		{
			clang::TypeLoc loc = typeSourceInfo->getTypeLoc();
			if (loc.getTypeLocClass() == clang::TypeLoc::TypeLocClass::TemplateSpecialization)
			{
				processTemplateArguments(loc.getAs<clang::TemplateSpecializationTypeLoc>());
			}
		}
		else
		{
			LOG_WARNING("Could not locate template arguments of explicit template specialization: " + specializedRecordNameHierarchy.getFullName());
		}
	}
}

void ASTVisitor::processTemplateArguments(clang::DeclRefExpr* expr)
{
	if (!isLocatedInProjectFile(getParseLocation(expr->getSourceRange())))
	{
		return;
	}
	clang::FunctionDecl* specializationDecl = clang::dyn_cast_or_null<clang::FunctionDecl>(expr->getDecl());
	if (!specializationDecl)
	{
		return;
	}
	clang::FunctionTemplateDecl* specializedDecl = specializationDecl->getPrimaryTemplate(); // just to check if it is really a template.. can be done differently
	if (!specializedDecl)
	{
		return;
	}

	ParseFunction specializationFunction = getParseFunction(specializationDecl);

	if (expr->hasExplicitTemplateArgs()) // handle explicit template args
	{
		const clang::TemplateArgumentLoc* arguments = expr->getTemplateArgs();

		for (unsigned int i = 0; i < expr->getNumTemplateArgs(); i++)
		{
			clang::TemplateArgument argument = arguments[i].getArgument();
			NameHierarchy argumentNameHierarchy = utility::templateArgumentToDataType(argument)->getTypeNameHierarchy();
			if (argumentNameHierarchy.size()) // FIXME: Some TemplateArgument kinds are not handled yet.
			{
				m_client->onTemplateArgumentTypeOfTemplateFunctionParsed(
					getParseLocation(arguments[i].getSourceRange()),
					argumentNameHierarchy,
					specializationFunction
				);
			}
		}
	}
	else // handle implicit template args
	{
		const clang::TemplateArgumentList* argumentList = specializationDecl->getTemplateSpecializationArgs();
		for (size_t i = 0; i < argumentList->size(); ++i)
		{
			const clang::TemplateArgument& argument = argumentList->get(i);
			NameHierarchy argumentNameHierarchy = utility::templateArgumentToDataType(argument)->getTypeNameHierarchy();
			if (argumentNameHierarchy.size()) // FIXME: Some TemplateArgument kinds are not handled yet.
			{
				const clang::SourceLocation argumentSourceLoc = clang::Lexer::getLocForEndOfToken(
					expr->getLocEnd(), 1, m_context->getSourceManager(), clang::LangOptions()
				);
				m_client->onTemplateArgumentTypeOfTemplateFunctionParsed(
					getParseLocation(clang::SourceRange(argumentSourceLoc, argumentSourceLoc)),
					argumentNameHierarchy,
					specializationFunction
				);
			}
		}
	}
}

void ASTVisitor::processTemplateArguments(clang::TemplateSpecializationTypeLoc loc)
{
	NameHierarchy specializationNameHierarchy = utility::qualTypeToDataType(loc.getType())->getTypeNameHierarchy();

	if (!isLocatedInProjectFile(getParseLocation(loc.getSourceRange())))
	{
		return;
	}

	for (unsigned int i = 0; i < loc.getNumArgs(); i++)
	{
		const clang::TemplateArgument& argument = loc.getArgLoc(i).getArgument();
		NameHierarchy argumentNameHierarchy = utility::templateArgumentToDataType(argument)->getTypeNameHierarchy();
		if (argumentNameHierarchy.size()) // FIXME: Some TemplateArgument kinds are not handled yet.
		{
			m_client->onTemplateArgumentTypeOfTemplateRecordParsed(
				getParseLocationForTokensInRange(loc.getArgLoc(i).getSourceRange()),
				argumentNameHierarchy,
				specializationNameHierarchy
				);
		}
	}
}

bool ASTVisitor::isLocatedInUnparsedProjectFile(const clang::Decl* declaration) const
{
	const clang::SourceLocation& location = declaration->getLocStart();

	if (!location.isValid())
	{
		return false;
	}

	if (m_context->getSourceManager().isWrittenInMainFile(location))
	{
		return true;
	}

	return m_fileRegister->includeFileIsParsing(FilePath(m_context->getSourceManager().getFilename(location)));
}

bool ASTVisitor::isLocatedInProjectFile(const ParseLocation& location) const
{
	if (location.isValid())
	{
		return m_fileRegister->getFileManager()->hasFilePath(location.filePath);
	}
	return false;
}

bool ASTVisitor::isLocatedInProjectFile(const clang::Decl* declaration) const
{
	const clang::SourceLocation& location = declaration->getLocStart();
	if (location.isValid())
	{
		const clang::SourceManager& sourceManager = m_context->getSourceManager();
		std::string filePath = FileSystem::absoluteFilePath(sourceManager.getFilename(location));
		return m_fileRegister->getFileManager()->hasFilePath(filePath);
	}
	return false;
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

ParserClient::AbstractionType ASTVisitor::getAbstractionType(const clang::CXXMethodDecl* methodDecl) const
{
	ParserClient::AbstractionType abstraction = ParserClient::ABSTRACTION_NONE;
	if (methodDecl->isPure())
	{
		abstraction = ParserClient::ABSTRACTION_PURE_VIRTUAL;
	}
	else if (methodDecl->isVirtual())
	{
		abstraction = ParserClient::ABSTRACTION_VIRTUAL;
	}
	return abstraction;
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

ParseLocation ASTVisitor::getParseLocationForTokenAtLocation(const clang::SourceLocation& loc) const
{
	return getParseLocationForTokensInRange(clang::SourceRange(loc, loc));
}

ParseLocation ASTVisitor::getParseLocationForTokensInRange(const clang::SourceRange& range) const
{
	if (range.isInvalid())
	{
		return ParseLocation();
	}

	const clang::SourceManager& sourceManager = m_context->getSourceManager();

	clang::SourceLocation startLoc = clang::Lexer::GetBeginningOfToken(range.getBegin(), sourceManager, clang::LangOptions());
	clang::SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(range.getEnd(), 1, sourceManager, clang::LangOptions());

	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(startLoc);
	const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(endLoc);

	return ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn()
	);
}

ParseLocation ASTVisitor::getParseLocationForNamedDecl(const clang::NamedDecl* decl) const
{
	return getParseLocationForTokenAtLocation(decl->getLocation());
}

ParseLocation ASTVisitor::getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const
{
	if (decl->hasBody() && decl->isThisDeclarationADefinition())
	{
		clang::SourceRange range;
		clang::FunctionTemplateDecl* templateDecl = decl->getDescribedFunctionTemplate();
		if (templateDecl)
		{
			range = templateDecl->getSourceRange();
		}
		else
		{
			range = decl->getSourceRange();
		}

		return getParseLocation(range);
	}

	return ParseLocation();
}

ParseLocation ASTVisitor::getParseLocationOfRecordBody(clang::CXXRecordDecl* decl) const
{
	if (decl->hasDefinition() && decl->isThisDeclarationADefinition())
	{
		clang::SourceRange range;
		clang::ClassTemplateDecl* templateDecl = decl->getDescribedClassTemplate();
		if (templateDecl)
		{
			range = templateDecl->getSourceRange();
		}
		else
		{
			range = decl->getDefinition()->getSourceRange();
		}

		return getParseLocation(range);
	}

	return ParseLocation();
}

ParseTypeUsage ASTVisitor::getParseTypeUsage(const clang::TypeLoc& typeLoc, const clang::QualType& type) const
{
	std::shared_ptr<DataType> dataType = utility::qualTypeToDataType(type);
	return getParseTypeUsage(typeLoc, dataType);
}

ParseTypeUsage ASTVisitor::getParseTypeUsage(const clang::TypeLoc& typeLoc, const std::shared_ptr<DataType> type) const
{
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

	return ParseTypeUsage(parseLocation, type);
}

ParseTypeUsage ASTVisitor::getParseTypeUsage(const clang::SourceRange& sourceRange, const std::shared_ptr<DataType> type) const
{
	ParseLocation parseLocation;

	if (sourceRange.isValid())
	{
		clang::SourceRange sr(
			sourceRange.getBegin(),
			clang::Lexer::getLocForEndOfToken(sourceRange.getEnd(), 0, m_context->getSourceManager(), clang::LangOptions())
		);

		parseLocation = getParseLocation(sr);
		parseLocation.endColumnNumber -= 1;
	}

	return ParseTypeUsage(parseLocation, type);
}

ParseTypeUsage ASTVisitor::getParseTypeUsageOfReturnType(const clang::FunctionDecl* declaration) const
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

std::vector<ParseTypeUsage> ASTVisitor::getParameters(const clang::FunctionDecl* declaration) const
{
	std::vector<ParseTypeUsage> parameters;

	for (unsigned i = 0; i < declaration->getNumParams(); i++)
	{
		const clang::ParmVarDecl* paramDecl = declaration->getParamDecl(i);
		if (paramDecl->getTypeSourceInfo())
		{
			parameters.push_back(getParseTypeUsage(paramDecl->getTypeSourceInfo()->getTypeLoc(), paramDecl->getType()));
		}
	}

	return parameters;
}

ParseVariable ASTVisitor::getParseVariable(const clang::DeclaratorDecl* declaration) const
{
	bool isStatic = false;
	NameHierarchy hameHierarchy = utility::getDeclNameHierarchy(declaration);
	if (clang::isa<clang::VarDecl>(declaration))
	{
		const clang::VarDecl* varDecl = clang::dyn_cast<const clang::VarDecl>(declaration);
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

ParseFunction ASTVisitor::getParseFunction(const clang::FunctionDecl* declaration) const
{
	bool isStatic = false;
	bool isConst = false;

	if (clang::isa<clang::CXXMethodDecl>(declaration))
	{
		const clang::CXXMethodDecl* methodDecl = clang::dyn_cast<const clang::CXXMethodDecl>(declaration);
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

ParseFunction ASTVisitor::getParseFunction(const clang::FunctionTemplateDecl* declaration) const
{
	return getParseFunction(declaration->getTemplatedDecl());
}
