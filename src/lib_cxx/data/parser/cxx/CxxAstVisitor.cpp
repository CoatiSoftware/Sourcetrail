#include "CxxAstVisitor.h"

#include <clang/AST/ASTContext.h>
#include <clang/Lex/Preprocessor.h>

#include "CanonicalFilePathCache.h"
#include "CxxDeclNameResolver.h"
#include "CxxTypeNameResolver.h"
#include "IndexerStateInfo.h"
#include "ParseLocation.h"
#include "ParserClient.h"
#include "logging.h"
#include "utilityClang.h"
#include "utilityString.h"

CxxAstVisitor::CxxAstVisitor(
	clang::ASTContext* astContext,
	clang::Preprocessor* preprocessor,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	std::shared_ptr<IndexerStateInfo> indexerStateInfo)
	: m_astContext(astContext)
	, m_preprocessor(preprocessor)
	, m_client(client)
	, m_indexerStateInfo(indexerStateInfo)
	, m_canonicalFilePathCache(canonicalFilePathCache)
	, m_contextComponent(this)
	, m_declRefKindComponent(this)
	, m_typeRefKindComponent(this)
	, m_implicitCodeComponent(this)
	, m_indexerComponent(this, astContext, client)
	, m_braceRecorderComponent(this, astContext, client)
{
}

template <>
CxxAstVisitorComponentContext* CxxAstVisitor::getComponent()
{
	return &m_contextComponent;
}

template <>
CxxAstVisitorComponentTypeRefKind* CxxAstVisitor::getComponent()
{
	return &m_typeRefKindComponent;
}

template <>
CxxAstVisitorComponentDeclRefKind* CxxAstVisitor::getComponent()
{
	return &m_declRefKindComponent;
}

template <>
CxxAstVisitorComponentIndexer* CxxAstVisitor::getComponent()
{
	return &m_indexerComponent;
}

CanonicalFilePathCache* CxxAstVisitor::getCanonicalFilePathCache() const
{
	return m_canonicalFilePathCache.get();
}

void CxxAstVisitor::indexDecl(clang::Decl* d)
{
	LOG_INFO("starting AST traversal");
	this->TraverseDecl(d);
}

bool CxxAstVisitor::shouldVisitTemplateInstantiations() const
{
	return true;
}

bool CxxAstVisitor::shouldVisitImplicitCode() const
{
	return m_implicitCodeComponent.shouldVisitImplicitCode();
}

bool CxxAstVisitor::checkIgnoresTypeLoc(const clang::TypeLoc& tl) const
{
	if ((!tl.getAs<clang::TagTypeLoc>().isNull()) || (!tl.getAs<clang::TypedefTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TemplateTypeParmTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TemplateSpecializationTypeLoc>().isNull()) ||
		(!tl.getAs<clang::InjectedClassNameTypeLoc>().isNull()) ||
		(!tl.getAs<clang::DependentNameTypeLoc>().isNull()) ||
		(!tl.getAs<clang::DependentTemplateSpecializationTypeLoc>().isNull()) ||
		(!tl.getAs<clang::SubstTemplateTypeParmTypeLoc>().isNull()) ||
		(!tl.getAs<clang::BuiltinTypeLoc>().isNull()) || (!tl.getAs<clang::AutoTypeLoc>().isNull()))
	{
		return false;
	}
	return true;
}

#define FOREACH_COMPONENT(__METHOD_CALL__)                                                         \
	{                                                                                              \
		m_contextComponent.__METHOD_CALL__;                                                        \
		m_typeRefKindComponent.__METHOD_CALL__;                                                    \
		m_declRefKindComponent.__METHOD_CALL__;                                                    \
		m_implicitCodeComponent.__METHOD_CALL__;                                                   \
		m_indexerComponent.__METHOD_CALL__;                                                        \
		m_braceRecorderComponent.__METHOD_CALL__;                                                  \
	}

#define DEF_TRAVERSE_CUSTOM_TYPE_PTR(__NAME_TYPE__, __PARAM_TYPE__, CODE_BEFORE, CODE_AFTER)       \
	bool CxxAstVisitor::Traverse##__NAME_TYPE__(clang::__PARAM_TYPE__* v)                          \
	{                                                                                              \
		FOREACH_COMPONENT(beginTraverse##__NAME_TYPE__(v));                                        \
		bool ret = true;                                                                           \
		{                                                                                          \
			CODE_BEFORE;                                                                           \
		}                                                                                          \
		Base::Traverse##__NAME_TYPE__(v);                                                          \
		{                                                                                          \
			CODE_AFTER;                                                                            \
		}                                                                                          \
		FOREACH_COMPONENT(endTraverse##__NAME_TYPE__(v));                                          \
		return ret;                                                                                \
	}

#define DEF_TRAVERSE_CUSTOM_TYPE(__NAME_TYPE__, __PARAM_TYPE__, CODE_BEFORE, CODE_AFTER)           \
	bool CxxAstVisitor::Traverse##__NAME_TYPE__(clang::__PARAM_TYPE__ v)                           \
	{                                                                                              \
		FOREACH_COMPONENT(beginTraverse##__NAME_TYPE__(v));                                        \
		bool ret = true;                                                                           \
		{                                                                                          \
			CODE_BEFORE;                                                                           \
		}                                                                                          \
		Base::Traverse##__NAME_TYPE__(v);                                                          \
		{                                                                                          \
			CODE_AFTER;                                                                            \
		}                                                                                          \
		FOREACH_COMPONENT(endTraverse##__NAME_TYPE__(v));                                          \
		return ret;                                                                                \
	}

#define DEF_TRAVERSE_TYPE_PTR(__TYPE__, CODE_BEFORE, CODE_AFTER)                                   \
	DEF_TRAVERSE_CUSTOM_TYPE_PTR(__TYPE__, __TYPE__, CODE_BEFORE, CODE_AFTER)

#define DEF_TRAVERSE_TYPE(__TYPE__, CODE_BEFORE, CODE_AFTER)                                       \
	DEF_TRAVERSE_CUSTOM_TYPE(__TYPE__, __TYPE__, CODE_BEFORE, CODE_AFTER)

bool CxxAstVisitor::TraverseDecl(clang::Decl* decl)
{
	bool traverse = true;
	if (decl)
	{
		const clang::SourceManager& sourceManager = m_astContext->getSourceManager();
		clang::SourceLocation loc = sourceManager.getExpansionLoc(decl->getLocation());

		if (loc.isInvalid())
		{
			loc = decl->getLocation();
		}

		if (loc.isValid())
		{
			// record files not handled in preprocessor callbacks, e.g. files within precompiled header
			const clang::FileID fileId = sourceManager.getFileID(loc);
			if (fileId.isValid() && m_canonicalFilePathCache->getFileSymbolId(fileId) == 0)
			{
				const FilePath filePath = m_canonicalFilePathCache->getCanonicalFilePath(
					fileId, sourceManager);
				const bool pathIsProjectFile = m_canonicalFilePathCache->isProjectFile(
					fileId, sourceManager);
				const Id symbolId = m_client->recordFile(filePath, pathIsProjectFile);
				m_client->recordFileLanguage(symbolId, L"cpp");
				m_canonicalFilePathCache->addFileSymbolId(fileId, filePath, symbolId);
			}

			traverse = isLocatedInProjectFile(loc);
		}
	}

	if (traverse)
	{
		FOREACH_COMPONENT(beginTraverseDecl(decl));
		Base::TraverseDecl(decl);
		FOREACH_COMPONENT(endTraverseDecl(decl));
	}

	if (m_indexerStateInfo && m_indexerStateInfo->indexingInterrupted)
	{
		LOG_INFO("interrupting AST traversal");
		return false;
	}
	return true;
}

// same as Base::TraverseQualifiedTypeLoc(..) but we need to make sure to call this.TraverseTypeLoc(..)
bool CxxAstVisitor::TraverseQualifiedTypeLoc(clang::QualifiedTypeLoc tl)
{
	return TraverseTypeLoc(tl.getUnqualifiedLoc());
}

DEF_TRAVERSE_TYPE(TypeLoc, {}, {})

DEF_TRAVERSE_CUSTOM_TYPE(Type, QualType, {}, {})

DEF_TRAVERSE_TYPE_PTR(Stmt, {}, {})

// same as Base::TraverseCXXRecordDecl(..) but we need to integrate the setter for the context info.
// additionally: skip implicit CXXRecordDecls (this does not skip template specializations).
bool CxxAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* d)
{
	if (utility::isImplicit(d) && d->getMemberSpecializationInfo() == nullptr &&
		!clang::isa<clang::ClassTemplateSpecializationDecl>(utility::getFirstDecl(d)))
	{
		return true;
	}

	if (d->isLambda())
	{
		return TraverseFunctionDecl(d->getLambdaCallOperator());
	}

	WalkUpFromCXXRecordDecl(d);

	TraverseNestedNameSpecifierLoc(d->getQualifierLoc());

	if (d->hasDefinition())
	{
		for (const auto& base: d->bases())
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
	FOREACH_COMPONENT(beginTraverseCXXBaseSpecifier());
	bool ret = TraverseTypeLoc(d.getTypeSourceInfo()->getTypeLoc());
	FOREACH_COMPONENT(endTraverseCXXBaseSpecifier());
	return ret;
}

bool CxxAstVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl* d)
{
	if (d->getTemplatedKind() == clang::CXXMethodDecl::TK_FunctionTemplate)
	{
		if (clang::CXXRecordDecl* recordDecl = d->getParent())
		{
			if (!clang::isa<clang::ClassTemplatePartialSpecializationDecl>(recordDecl) &&
				clang::isa<clang::ClassTemplateSpecializationDecl>(recordDecl) &&
				!clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(recordDecl)
					 ->isExplicitSpecialization())
			{
				return true;	// we skip visiting an implicit definition of a template method and
								// its contents
			}
		}
	}
	return Base::TraverseCXXMethodDecl(d);
}

// same as Base::TraverseTemplateTypeParmDecl(..) but we need to integrate the setter for the context info.
bool CxxAstVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	WalkUpFromTemplateTypeParmDecl(d);

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		FOREACH_COMPONENT(beginTraverseTemplateDefaultArgumentLoc());
		TraverseTypeLoc(d->getDefaultArgumentInfo()->getTypeLoc());
		FOREACH_COMPONENT(endTraverseTemplateDefaultArgumentLoc());
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

// same as Base::TraverseTemplateTemplateParmDecl(..) but we need to integrate the setter for the
// context info.
bool CxxAstVisitor::TraverseTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	WalkUpFromTemplateTemplateParmDecl(d);

	TraverseDecl(d->getTemplatedDecl());

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		FOREACH_COMPONENT(beginTraverseTemplateDefaultArgumentLoc());
		TraverseTemplateArgumentLoc(d->getDefaultArgument());
		FOREACH_COMPONENT(endTraverseTemplateDefaultArgumentLoc());
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

bool CxxAstVisitor::VisitTranslationUnitDecl(clang::TranslationUnitDecl* d)
{
	return true;
}

bool CxxAstVisitor::TraverseNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc loc)
{
	bool ret = true;
	if (loc)
	{
		FOREACH_COMPONENT(beginTraverseNestedNameSpecifierLoc(loc));

		// todo: call method of base class...
		if (clang::NestedNameSpecifierLoc prefix = loc.getPrefix())
		{
			ret = TraverseNestedNameSpecifierLoc(prefix);
		}

		FOREACH_COMPONENT(endTraverseNestedNameSpecifierLoc(loc));
	}
	return ret;
}

bool CxxAstVisitor::TraverseConstructorInitializer(clang::CXXCtorInitializer* init)
{
	FOREACH_COMPONENT(beginTraverseConstructorInitializer(init));

	bool ret = VisitConstructorInitializer(init);
	if (ret)
	{
		ret = Base::TraverseConstructorInitializer(init);
	}

	FOREACH_COMPONENT(endTraverseConstructorInitializer(init));

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
	FOREACH_COMPONENT(beginTraverseCallCommonCallee());
	WalkUpFromCXXConstructExpr(s);
	FOREACH_COMPONENT(endTraverseCallCommonCallee());

	for (unsigned int i = 0; i < s->getNumArgs(); ++i)
	{
		FOREACH_COMPONENT(beginTraverseCallCommonArgument());
		TraverseStmt(s->getArg(i));
		FOREACH_COMPONENT(endTraverseCallCommonArgument());
	}
	return true;
}

DEF_TRAVERSE_TYPE_PTR(CXXTemporaryObjectExpr, {}, {})
DEF_TRAVERSE_TYPE_PTR(LambdaExpr, {}, {})
DEF_TRAVERSE_TYPE_PTR(FunctionDecl, {}, {})

// same as base::TraverseClassTemplateSpecializationDecl but without traversing the typeloc of the
// template specialitation itself
bool CxxAstVisitor::TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* D)
{
	FOREACH_COMPONENT(beginTraverseClassTemplateSpecializationDecl(D));

	bool ShouldVisitChildren = true;
	bool ReturnValue = true;
	if (ReturnValue && !shouldTraversePostOrder())
	{
		if (!WalkUpFromClassTemplateSpecializationDecl(D))
		{
			ReturnValue = false;
		}
	}

	if (ReturnValue)
	{
		if (clang::TypeSourceInfo* TSI = D->getTypeAsWritten())
		{
			clang::TypeLoc::TypeLocClass ccccc = TSI->getTypeLoc().getTypeLocClass();
			const clang::TemplateSpecializationTypeLoc tstl =
				TSI->getTypeLoc().getAs<clang::TemplateSpecializationTypeLoc>();
			if (!tstl.isNull())
			{
				for (unsigned I = 0, E = tstl.getNumArgs(); I != E; ++I)
				{
					if (!TraverseTemplateArgumentLoc(tstl.getArgLoc(I)))
					{
						ReturnValue = false;
					}
				}
			}
		}
	}

	if (ReturnValue)
	{
		if (!TraverseNestedNameSpecifierLoc(D->getQualifierLoc()))
		{
			ReturnValue = false;
		}
	}

	if (ReturnValue && ShouldVisitChildren)
	{
		traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(D));
	}

	if (ReturnValue && shouldTraversePostOrder())
	{
		if (!WalkUpFromClassTemplateSpecializationDecl(D))
		{
			return false;
		}
	}

	FOREACH_COMPONENT(endTraverseClassTemplateSpecializationDecl(D));

	return ReturnValue;
}

DEF_TRAVERSE_TYPE_PTR(ClassTemplatePartialSpecializationDecl, {}, {})
DEF_TRAVERSE_TYPE_PTR(DeclRefExpr, {}, {})
DEF_TRAVERSE_TYPE_PTR(CXXForRangeStmt, {}, {})
DEF_TRAVERSE_TYPE(TemplateSpecializationTypeLoc, {}, {})
DEF_TRAVERSE_TYPE_PTR(UnresolvedLookupExpr, {}, {})
DEF_TRAVERSE_TYPE_PTR(UnresolvedMemberExpr, {}, {})

bool CxxAstVisitor::TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	FOREACH_COMPONENT(beginTraverseTemplateArgumentLoc(loc));
	bool ret = Base::TraverseTemplateArgumentLoc(loc);
	FOREACH_COMPONENT(endTraverseTemplateArgumentLoc(loc));
	return ret;
}

bool CxxAstVisitor::TraverseLambdaCapture(
	clang::LambdaExpr* lambdaExpr, const clang::LambdaCapture* capture, clang::Expr* Init)
{
	FOREACH_COMPONENT(beginTraverseLambdaCapture(lambdaExpr, capture));
	bool ret = true;
	if (lambdaExpr->isInitCapture(capture))
	{
		ret = TraverseDecl(capture->getCapturedVar());
	}
	FOREACH_COMPONENT(endTraverseLambdaCapture(lambdaExpr, capture));
	return ret;
}

bool CxxAstVisitor::TraverseBinComma(clang::BinaryOperator* s)
{
	FOREACH_COMPONENT(beginTraverseBinCommaLhs());
	TraverseStmt(s->getLHS());
	FOREACH_COMPONENT(endTraverseBinCommaLhs());

	FOREACH_COMPONENT(beginTraverseBinCommaRhs());
	TraverseStmt(s->getRHS());
	FOREACH_COMPONENT(endTraverseBinCommaRhs());
	return true;
}

bool CxxAstVisitor::TraverseDeclarationNameInfo(clang::DeclarationNameInfo NameInfo)
{
	// we don't visit any children here
	return true;
}

void CxxAstVisitor::traverseDeclContextHelper(clang::DeclContext* d)
{
	if (!d)
	{
		return;
	}

	for (auto* child: d->decls())
	{
		// BlockDecls and CapturedDecls are traversed through BlockExprs and
		// CapturedStmts respectively.
		if (!llvm::isa<clang::BlockDecl>(child) && !llvm::isa<clang::CapturedDecl>(child))
		{
			TraverseDecl(child);
		}
	}
}

bool CxxAstVisitor::TraverseCallCommon(clang::CallExpr* s)
{
	FOREACH_COMPONENT(beginTraverseCallCommonCallee());
	TraverseStmt(s->getCallee());
	FOREACH_COMPONENT(endTraverseCallCommonCallee());

	for (unsigned int i = 0; i < s->getNumArgs(); ++i)
	{
		FOREACH_COMPONENT(beginTraverseCallCommonArgument());
		TraverseStmt(s->getArg(i));
		FOREACH_COMPONENT(endTraverseCallCommonArgument());
	}
	return true;
}

bool CxxAstVisitor::TraverseAssignCommon(clang::BinaryOperator* s)
{
	FOREACH_COMPONENT(beginTraverseAssignCommonLhs());
	TraverseStmt(s->getLHS());
	FOREACH_COMPONENT(endTraverseAssignCommonLhs());

	FOREACH_COMPONENT(beginTraverseAssignCommonRhs());
	TraverseStmt(s->getRHS());
	FOREACH_COMPONENT(endTraverseAssignCommonRhs());
	return true;
}

#undef DEF_TRAVERSE_CUSTOM_TYPE_PTR
#undef DEF_TRAVERSE_CUSTOM_TYPE
#undef DEF_TRAVERSE_TYPE_PTR
#undef DEF_TRAVERSE_TYPE

#define DEF_VISIT_CUSTOM_TYPE_PTR(__NAME_TYPE__, __PARAM_TYPE__)                                   \
	bool CxxAstVisitor::Visit##__NAME_TYPE__(clang::__PARAM_TYPE__* v)                             \
	{                                                                                              \
		FOREACH_COMPONENT(visit##__NAME_TYPE__(v));                                                \
		return true;                                                                               \
	}

#define DEF_VISIT_CUSTOM_TYPE(__NAME_TYPE__, __PARAM_TYPE__)                                       \
	bool CxxAstVisitor::Visit##__NAME_TYPE__(clang::__PARAM_TYPE__ v)                              \
	{                                                                                              \
		FOREACH_COMPONENT(visit##__NAME_TYPE__(v));                                                \
		return true;                                                                               \
	}

#define DEF_VISIT_TYPE_PTR(__TYPE__) DEF_VISIT_CUSTOM_TYPE_PTR(__TYPE__, __TYPE__)

#define DEF_VISIT_TYPE(__TYPE__) DEF_VISIT_CUSTOM_TYPE(__TYPE__, __TYPE__)

DEF_VISIT_TYPE_PTR(CastExpr)
DEF_VISIT_CUSTOM_TYPE_PTR(UnaryAddrOf, UnaryOperator)
DEF_VISIT_CUSTOM_TYPE_PTR(UnaryDeref, UnaryOperator)
DEF_VISIT_TYPE_PTR(DeclStmt)
DEF_VISIT_TYPE_PTR(ReturnStmt)
DEF_VISIT_TYPE_PTR(CompoundStmt)
DEF_VISIT_TYPE_PTR(InitListExpr)
DEF_VISIT_TYPE_PTR(TagDecl)
DEF_VISIT_TYPE_PTR(ClassTemplateSpecializationDecl)
DEF_VISIT_TYPE_PTR(FunctionDecl)
DEF_VISIT_TYPE_PTR(CXXMethodDecl)
DEF_VISIT_TYPE_PTR(VarDecl)
DEF_VISIT_TYPE_PTR(VarTemplateSpecializationDecl)
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
DEF_VISIT_TYPE_PTR(CXXDependentScopeMemberExpr)
DEF_VISIT_TYPE_PTR(CXXConstructExpr)
DEF_VISIT_TYPE_PTR(CXXDeleteExpr)
DEF_VISIT_TYPE_PTR(LambdaExpr)
DEF_VISIT_TYPE_PTR(MSAsmStmt)
DEF_VISIT_CUSTOM_TYPE_PTR(ConstructorInitializer, CXXCtorInitializer)

#undef DEF_VISIT_CUSTOM_TYPE_PTR
#undef DEF_VISIT_CUSTOM_TYPE
#undef DEF_VISIT_TYPE_PTR
#undef DEF_VISIT_TYPE

#undef FOREACH_COMPONENT

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

ParseLocation CxxAstVisitor::getParseLocation(const clang::SourceLocation& sourceLocation) const
{
	return utility::getParseLocation(
		sourceLocation, m_astContext->getSourceManager(), m_preprocessor, m_canonicalFilePathCache);
}

ParseLocation CxxAstVisitor::getParseLocation(const clang::SourceRange& sourceRange) const
{
	return utility::getParseLocation(
		sourceRange, m_astContext->getSourceManager(), m_preprocessor, m_canonicalFilePathCache);
}

bool CxxAstVisitor::shouldVisitStmt(const clang::Stmt* s) const
{
	if (s)
	{
		clang::SourceLocation loc = m_astContext->getSourceManager().getExpansionLoc(s->getBeginLoc());

		if (loc.isInvalid())
		{
			loc = s->getBeginLoc();
		}

		if (isLocatedInProjectFile(loc))
		{
			return true;
		}
	}
	return false;
}

bool CxxAstVisitor::shouldVisitDecl(const clang::Decl* decl) const
{
	if (decl)
	{
		clang::SourceLocation loc = m_astContext->getSourceManager().getExpansionLoc(
			decl->getLocation());

		if (loc.isInvalid())
		{
			loc = decl->getLocation();
		}

		if (isLocatedInProjectFile(loc))
		{
			return true;
		}
	}
	return false;
}

bool CxxAstVisitor::shouldVisitReference(const clang::SourceLocation& referenceLocation) const
{
	clang::SourceLocation loc = m_astContext->getSourceManager().getExpansionLoc(referenceLocation);
	if (loc.isInvalid())
	{
		loc = referenceLocation;
	}

	if (isLocatedInProjectFile(loc))
	{
		return true;
	}

	return false;
}

bool CxxAstVisitor::isLocatedInProjectFile(clang::SourceLocation loc) const
{
	if (loc.isInvalid())
	{
		return false;
	}

	const clang::SourceManager& sourceManager = m_astContext->getSourceManager();
	return m_canonicalFilePathCache->isProjectFile(sourceManager.getFileID(loc), sourceManager);
}
