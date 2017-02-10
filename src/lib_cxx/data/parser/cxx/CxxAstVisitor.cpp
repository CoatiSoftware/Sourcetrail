#include "data/parser/cxx/CxxAstVisitor.h"

#include <clang/AST/ASTContext.h>
#include <clang/Lex/Preprocessor.h>

#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"

#include "data/parser/cxx/CxxAstVisitorComponent.h"
#include "data/parser/cxx/CxxAstVisitorComponentContext.h"
#include "data/parser/cxx/CxxAstVisitorComponentDeclRefKind.h"
#include "data/parser/cxx/CxxAstVisitorComponentTypeRefKind.h"
#include "data/parser/cxx/CxxAstVisitorComponentIndexer.h"
#include "data/parser/cxx/utilityCxxAstVisitor.h"

#include "data/parser/ParseLocation.h"

CxxAstVisitor::CxxAstVisitor(clang::ASTContext* astContext, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister)
	: m_astContext(astContext)
	, m_preprocessor(preprocessor)
	, m_client(client)
	, m_fileRegister(fileRegister)
{
	m_declNameCache = std::make_shared<DeclNameCache>([](const clang::NamedDecl* decl) -> NameHierarchy
	{
		if (decl)
		{
			CxxDeclNameResolver resolver;
			if (std::shared_ptr<CxxDeclName> declName = resolver.getName(decl))
			{
				return declName->toNameHierarchy();
			}
		}
		return NameHierarchy("global");
	});
	m_typeNameCache = std::make_shared<TypeNameCache>([](const clang::Type* type) -> NameHierarchy
	{
		if (type)
		{
			CxxTypeNameResolver resolver;
			if (std::shared_ptr<CxxTypeName> typeName = resolver.getName(type))
			{
				return typeName->toNameHierarchy();
			}
		}
		return NameHierarchy("global");
	});

	m_contextComponent = std::make_shared<CxxAstVisitorComponentContext>(this);
	m_components.push_back(m_contextComponent);
	m_typeRefKindComponent = std::make_shared<CxxAstVisitorComponentTypeRefKind>(this);
	m_components.push_back(m_typeRefKindComponent);
	m_declRefKindComponent = std::make_shared<CxxAstVisitorComponentDeclRefKind>(this);
	m_components.push_back(m_declRefKindComponent);
	m_indexerComponent = std::make_shared<CxxAstVisitorComponentIndexer>(this, astContext, client, fileRegister);
	m_components.push_back(m_indexerComponent);
}

CxxAstVisitor::~CxxAstVisitor()
{
}

template <>
std::shared_ptr<CxxAstVisitorComponentContext> CxxAstVisitor::getComponent()
{
	return m_contextComponent;
}

template <>
std::shared_ptr<CxxAstVisitorComponentTypeRefKind> CxxAstVisitor::getComponent()
{
	return m_typeRefKindComponent;
}

template <>
std::shared_ptr<CxxAstVisitorComponentDeclRefKind> CxxAstVisitor::getComponent()
{
	return m_declRefKindComponent;
}

template <>
std::shared_ptr<CxxAstVisitorComponentIndexer> CxxAstVisitor::getComponent()
{
	return m_indexerComponent;
}

std::shared_ptr<DeclNameCache> CxxAstVisitor::getDeclNameCache()
{
	return m_declNameCache;
}

std::shared_ptr<TypeNameCache> CxxAstVisitor::getTypeNameCache()
{
	return m_typeNameCache;
}

void CxxAstVisitor::indexDecl(clang::Decl* d)
{
	this->TraverseDecl(d);
}

bool CxxAstVisitor::shouldVisitTemplateInstantiations() const
{
	return true;
}

bool CxxAstVisitor::shouldVisitImplicitCode() const
{
	return true;
}

bool CxxAstVisitor::checkIgnoresTypeLoc(const clang::TypeLoc& tl) const
{
	if ((!tl.getAs<clang::TagTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TypedefTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TemplateTypeParmTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TemplateSpecializationTypeLoc>().isNull()) ||
		(!tl.getAs<clang::InjectedClassNameTypeLoc>().isNull()) ||
		(!tl.getAs<clang::DependentNameTypeLoc>().isNull()) ||
		(!tl.getAs<clang::DependentTemplateSpecializationTypeLoc>().isNull()) ||
		(!tl.getAs<clang::BuiltinTypeLoc>().isNull()) ||
		(!tl.getAs<clang::AutoTypeLoc>().isNull())
		){
		return false;
	}
	return true;
}

#define DEF_TRAVERSE_CUSTOM_TYPE_PTR(__NAME_TYPE__, __PARAM_TYPE__, CODE_BEFORE, CODE_AFTER)	\
	bool CxxAstVisitor::Traverse##__NAME_TYPE__(clang::__PARAM_TYPE__* v)						\
	{																							\
		for (auto it = m_components.begin(); it != m_components.end(); it++)					\
		{																						\
			(*it)->beginTraverse##__NAME_TYPE__(v);												\
		}																						\
		bool ret = true;																		\
		{ CODE_BEFORE; }																		\
		Base::Traverse##__NAME_TYPE__(v);														\
		{ CODE_AFTER; }																			\
		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)					\
		{																						\
			(*it)->endTraverse##__NAME_TYPE__(v);												\
		}																						\
		return ret;																				\
	}

#define DEF_TRAVERSE_CUSTOM_TYPE(__NAME_TYPE__, __PARAM_TYPE__, CODE_BEFORE, CODE_AFTER)		\
	bool CxxAstVisitor::Traverse##__NAME_TYPE__(clang::__PARAM_TYPE__ v)						\
	{																							\
		for (auto it = m_components.begin(); it != m_components.end(); it++)					\
		{																						\
			(*it)->beginTraverse##__NAME_TYPE__(v);												\
		}																						\
		bool ret = true;																		\
		{ CODE_BEFORE; }																		\
		Base::Traverse##__NAME_TYPE__(v);														\
		{ CODE_AFTER; }																			\
		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)					\
		{																						\
			(*it)->endTraverse##__NAME_TYPE__(v);												\
		}																						\
		return ret;																				\
	}

#define DEF_TRAVERSE_TYPE_PTR(__TYPE__, CODE_BEFORE, CODE_AFTER)								\
	DEF_TRAVERSE_CUSTOM_TYPE_PTR(__TYPE__, __TYPE__, CODE_BEFORE, CODE_AFTER)

#define DEF_TRAVERSE_TYPE(__TYPE__, CODE_BEFORE, CODE_AFTER)									\
	DEF_TRAVERSE_CUSTOM_TYPE(__TYPE__, __TYPE__, CODE_BEFORE, CODE_AFTER)

DEF_TRAVERSE_TYPE_PTR(Decl, {}, {})

DEF_TRAVERSE_TYPE_PTR(Stmt, {}, {})

DEF_TRAVERSE_CUSTOM_TYPE(Type, QualType, {}, {})

// same as Base::TraverseQualifiedTypeLoc(..) but we need to make sure to call this.TraverseTypeLoc(..)
bool CxxAstVisitor::TraverseQualifiedTypeLoc(clang::QualifiedTypeLoc tl)
{
	return TraverseTypeLoc(tl.getUnqualifiedLoc());
}

DEF_TRAVERSE_TYPE(TypeLoc, {}, {})

// same as Base::TraverseCXXRecordDecl(..) but we need to integrate the setter for the context info.
// additionally: skip implicit CXXRecordDecls (this does not skip template specializations).
bool CxxAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl *d)
{
	if (utility::isImplicit(d))
	{
		return true;
	}

	WalkUpFromCXXRecordDecl(d);

	TraverseNestedNameSpecifierLoc(d->getQualifierLoc());

	if (d->isCompleteDefinition())
	{
		for (const auto& base : d->bases())
		{
			if (!traverseCXXBaseSpecifier(base))
			{
				return false;
			}
		}
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

bool CxxAstVisitor::traverseCXXBaseSpecifier(const clang::CXXBaseSpecifier& d)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseCXXBaseSpecifier();
	}
	bool ret = TraverseTypeLoc(d.getTypeSourceInfo()->getTypeLoc());
	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseCXXBaseSpecifier();
	}
	return ret;
}

// same as Base::TraverseTemplateTypeParmDecl(..) but we need to integrate the setter for the context info.
bool CxxAstVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	WalkUpFromTemplateTypeParmDecl(d);

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->beginTraverseTemplateDefaultArgumentLoc();
		}
		TraverseTypeLoc(d->getDefaultArgumentInfo()->getTypeLoc());
		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
		{
			(*it)->endTraverseTemplateDefaultArgumentLoc();
		}
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

// same as Base::TraverseTemplateTemplateParmDecl(..) but we need to integrate the setter for the context info.
bool CxxAstVisitor::TraverseTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	WalkUpFromTemplateTemplateParmDecl(d);

	TraverseDecl(d->getTemplatedDecl());

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->beginTraverseTemplateDefaultArgumentLoc();
		}
		TraverseTemplateArgumentLoc(d->getDefaultArgument());
		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
		{
			(*it)->endTraverseTemplateDefaultArgumentLoc();
		}
	}

	clang::TemplateParameterList* TPL = d->getTemplateParameters();
	if (TPL)
	{
		for (clang::TemplateParameterList::iterator I = TPL->begin(), E = TPL->end(); I != E; ++I)
		{
			TraverseDecl(*I);
		}
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

bool CxxAstVisitor::TraverseNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc loc)
{
	bool ret = true;
	if (loc)
	{
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->beginTraverseNestedNameSpecifierLoc(loc);
		}

		//todo: call method of base class...
		if (clang::NestedNameSpecifierLoc prefix = loc.getPrefix())
		{
			ret = TraverseNestedNameSpecifierLoc(prefix);
		}

		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
		{
			(*it)->endTraverseNestedNameSpecifierLoc(loc);
		}
	}
	return ret;
}

bool CxxAstVisitor::TraverseConstructorInitializer(clang::CXXCtorInitializer* init)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseConstructorInitializer(init);
	}

	if (!VisitConstructorInitializer(init))
	{
		return false;
	}
	bool ret = Base::TraverseConstructorInitializer(init);

	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseConstructorInitializer(init);
	}

	return ret;
}

bool CxxAstVisitor::TraverseCallExpr(clang::CallExpr* s)
{
	return TraverseCallCommon(s);
}

bool CxxAstVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* s)
{
	return TraverseCallCommon(s);
}

bool CxxAstVisitor::TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr* s)
{
	return TraverseCallCommon(s);
}

bool CxxAstVisitor::TraverseCXXConstructExpr(clang::CXXConstructExpr* s)
{
	{
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->beginTraverseCallCommonCallee();
		}
		WalkUpFromCXXConstructExpr(s);
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->endTraverseCallCommonCallee();
		}
	}
	for (unsigned int i = 0; i < s->getNumArgs(); ++i)
	{
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->beginTraverseCallCommonArgument();
		}
		TraverseStmt(s->getArg(i));
		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
		{
			(*it)->endTraverseCallCommonArgument();
		}
	}
	return true;
}

DEF_TRAVERSE_TYPE_PTR(CXXTemporaryObjectExpr, {}, {})
DEF_TRAVERSE_TYPE_PTR(LambdaExpr, {}, {})
DEF_TRAVERSE_TYPE_PTR(FunctionDecl, {}, {})
DEF_TRAVERSE_TYPE_PTR(ClassTemplateSpecializationDecl, {}, {})
DEF_TRAVERSE_TYPE_PTR(ClassTemplatePartialSpecializationDecl, {}, {})
DEF_TRAVERSE_TYPE_PTR(DeclRefExpr, {}, {})
DEF_TRAVERSE_TYPE(TemplateSpecializationTypeLoc, {}, {})
DEF_TRAVERSE_TYPE_PTR(UnresolvedLookupExpr, {}, {})

bool CxxAstVisitor::TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseTemplateArgumentLoc(loc);
	}

	bool ret = Base::TraverseTemplateArgumentLoc(loc);

	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseTemplateArgumentLoc(loc);
	}
	return ret;
}

bool CxxAstVisitor::TraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseLambdaCapture(lambdaExpr, capture);
	}

	bool ret = true;

	if (lambdaExpr->isInitCapture(capture))
	{
		ret = TraverseDecl(capture->getCapturedVar());
	}

	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseLambdaCapture(lambdaExpr, capture);
	}
	return ret;
}

bool CxxAstVisitor::TraverseBinComma(clang::BinaryOperator* s)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseBinCommaLhs();
	}
	TraverseStmt(s->getLHS());
	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseBinCommaLhs();
	}

	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseBinCommaRhs();
	}
	TraverseStmt(s->getRHS());
	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseBinCommaRhs();
	}
	return true;
}

void CxxAstVisitor::traverseDeclContextHelper(clang::DeclContext* d)
{
	if (!d)
	{
		return;
	}

	// Traverse children.
	for (clang::DeclContext::decl_iterator it = d->decls_begin(),
		itEnd = d->decls_end(); it != itEnd; ++it)
	{
		// BlockDecls are traversed through BlockExprs.
		if (!llvm::isa<clang::BlockDecl>(*it))
		{
			TraverseDecl(*it);
		}
	}
}

bool CxxAstVisitor::TraverseCallCommon(clang::CallExpr* s)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseCallCommonCallee();
	}
	TraverseStmt(s->getCallee());
	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseCallCommonCallee();
	}

	for (unsigned int i = 0; i < s->getNumArgs(); ++i)
	{
		for (auto it = m_components.begin(); it != m_components.end(); it++)
		{
			(*it)->beginTraverseCallCommonArgument();
		}
		TraverseStmt(s->getArg(i));
		for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
		{
			(*it)->endTraverseCallCommonArgument();
		}
	}
	return true;
}

bool CxxAstVisitor::TraverseAssignCommon(clang::BinaryOperator* s)
{
	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseAssignCommonLhs();
	}
	TraverseStmt(s->getLHS());
	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseAssignCommonLhs();
	}

	for (auto it = m_components.begin(); it != m_components.end(); it++)
	{
		(*it)->beginTraverseAssignCommonRhs();
	}
	TraverseStmt(s->getRHS());
	for (auto it = m_components.rbegin(); it != m_components.rend(); it++)
	{
		(*it)->endTraverseAssignCommonRhs();
	}
	return true;
}

#undef DEF_TRAVERSE_CUSTOM_TYPE_PTR
#undef DEF_TRAVERSE_CUSTOM_TYPE
#undef DEF_TRAVERSE_TYPE_PTR
#undef DEF_TRAVERSE_TYPE

#define DEF_VISIT_CUSTOM_TYPE_PTR(__NAME_TYPE__, __PARAM_TYPE__)				\
	bool CxxAstVisitor::Visit##__NAME_TYPE__(clang::__PARAM_TYPE__* v)			\
	{																			\
		for (auto it = m_components.begin(); it != m_components.end(); it++)	\
		{																		\
			(*it)->visit##__NAME_TYPE__(v);										\
		}																		\
		return true;															\
	}

#define DEF_VISIT_CUSTOM_TYPE(__NAME_TYPE__, __PARAM_TYPE__)					\
	bool CxxAstVisitor::Visit##__NAME_TYPE__(clang::__PARAM_TYPE__ v)			\
	{																			\
		for (auto it = m_components.begin(); it != m_components.end(); it++)	\
		{																		\
			(*it)->visit##__NAME_TYPE__(v);										\
		}																		\
		return true;															\
	}

#define DEF_VISIT_TYPE_PTR(__TYPE__)											\
	DEF_VISIT_CUSTOM_TYPE_PTR(__TYPE__, __TYPE__)

#define DEF_VISIT_TYPE(__TYPE__)												\
	DEF_VISIT_CUSTOM_TYPE(__TYPE__, __TYPE__)

DEF_VISIT_TYPE_PTR(CastExpr)
DEF_VISIT_CUSTOM_TYPE_PTR(UnaryAddrOf, UnaryOperator)
DEF_VISIT_CUSTOM_TYPE_PTR(UnaryDeref, UnaryOperator)
DEF_VISIT_TYPE_PTR(DeclStmt)
DEF_VISIT_TYPE_PTR(ReturnStmt)
DEF_VISIT_TYPE_PTR(InitListExpr)
DEF_VISIT_TYPE_PTR(TagDecl)
DEF_VISIT_TYPE_PTR(ClassTemplateSpecializationDecl)
DEF_VISIT_TYPE_PTR(FunctionDecl)
DEF_VISIT_TYPE_PTR(CXXMethodDecl)
DEF_VISIT_TYPE_PTR(VarDecl)
DEF_VISIT_TYPE_PTR(FieldDecl)
DEF_VISIT_TYPE_PTR(TypedefDecl)
DEF_VISIT_TYPE_PTR(TypeAliasDecl)
DEF_VISIT_TYPE_PTR(NamespaceDecl)
DEF_VISIT_TYPE_PTR(NamespaceAliasDecl)
DEF_VISIT_TYPE_PTR(EnumConstantDecl)
DEF_VISIT_TYPE_PTR(UsingDirectiveDecl)
DEF_VISIT_TYPE_PTR(UsingDecl)
DEF_VISIT_TYPE_PTR(NonTypeTemplateParmDecl)
DEF_VISIT_TYPE_PTR(TemplateTypeParmDecl)
DEF_VISIT_TYPE_PTR(TemplateTemplateParmDecl)
DEF_VISIT_TYPE(TypeLoc)
DEF_VISIT_TYPE_PTR(DeclRefExpr)
DEF_VISIT_TYPE_PTR(MemberExpr)
DEF_VISIT_TYPE_PTR(CXXConstructExpr)
DEF_VISIT_TYPE_PTR(LambdaExpr)
DEF_VISIT_CUSTOM_TYPE_PTR(ConstructorInitializer, CXXCtorInitializer)

#undef DEF_VISIT_CUSTOM_TYPE_PTR
#undef DEF_VISIT_CUSTOM_TYPE
#undef DEF_VISIT_TYPE_PTR
#undef DEF_VISIT_TYPE

ParseLocation CxxAstVisitor::getParseLocationOfTagDeclBody(clang::TagDecl* decl) const
{
	if (decl->isThisDeclarationADefinition())
	{
		clang::SourceRange range;
		if (clang::CXXRecordDecl* cxxDecl = clang::dyn_cast_or_null<clang::CXXRecordDecl>(decl))
		{
			clang::ClassTemplateDecl* templateDecl = cxxDecl->getDescribedClassTemplate();
			if (templateDecl)
			{
				range = templateDecl->getSourceRange();
			}
		}
		if (range.isInvalid())
		{
			range = decl->getDefinition()->getSourceRange();
		}
		return getParseLocation(range);
	}
	return ParseLocation();
}

ParseLocation CxxAstVisitor::getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const
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

ParseLocation CxxAstVisitor::getParseLocation(const clang::SourceLocation& loc) const
{
	ParseLocation parseLocation;
	if (loc.isValid())
	{
		clang::SourceManager& sourceManager = m_astContext->getSourceManager();
		clang::SourceLocation startLoc = sourceManager.getSpellingLoc(loc);
		clang::FileID fileId = sourceManager.getFileID(startLoc);

		// find the location file
		if (!fileId.isInvalid())
		{
			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
			if (fileEntry != NULL)
			{
				parseLocation.filePath = FilePath(fileEntry->getName()).canonical();
			}
		}

		// find the start location
		{
			unsigned int offset = sourceManager.getFileOffset(startLoc);
			parseLocation.startLineNumber = sourceManager.getLineNumber(fileId, offset);
			parseLocation.startColumnNumber = sourceManager.getColumnNumber(fileId, offset);
		}

		// General case -- find the end of the token starting at loc.
		{
			clang::SourceLocation endSloc = m_preprocessor->getLocForEndOfToken(startLoc);
			unsigned int offset = sourceManager.getFileOffset(endSloc);
			parseLocation.endLineNumber = sourceManager.getLineNumber(fileId, offset);
			parseLocation.endColumnNumber = sourceManager.getColumnNumber(fileId, offset) - 1;
		}
	}

	return parseLocation;
}

ParseLocation CxxAstVisitor::getParseLocation(const clang::SourceRange& sourceRange) const
{
	ParseLocation parseLocation;
	if (sourceRange.isValid())
	{
		const clang::SourceManager& sourceManager = m_astContext->getSourceManager();

		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
		const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

		parseLocation = ParseLocation(
			presumedBegin.getFilename(),
			presumedBegin.getLine(),
			presumedBegin.getColumn(),
			presumedEnd.getLine(),
			presumedEnd.getColumn()
		);
	}
	return parseLocation;
}
