#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <unordered_map>

#include "clang/AST/ASTContext.h"
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"



#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "data/parser/cxx/utilityCxx.h"

class ASTVisitor: clang::RecursiveASTVisitor<ASTVisitor>
{
public:

	class TypeNameCache
	{
	public:
		NameHierarchy getName(const clang::Type* type)
		{
			if (type)
			{
				NameHierarchy nameHierarchy;
				std::unordered_map<const clang::Type*, NameHierarchy>::const_iterator it = m_typeNameMap.find(type);
				if (it != m_typeNameMap.end())
				{
					nameHierarchy = it->second;
				}
				else
				{
					CxxTypeNameResolver resolver;
					nameHierarchy = resolver.getTypeNameHierarchy(type);
					m_typeNameMap[type] = nameHierarchy;
				}
				return nameHierarchy;
			}
			return NameHierarchy("global");
		}

	private:
		std::unordered_map<const clang::Type*, NameHierarchy> m_typeNameMap;
	};

	class DeclNameCache
	{
	public:
		NameHierarchy getName(const clang::NamedDecl* decl)
		{
			if (decl)
			{
				NameHierarchy nameHierarchy;
				std::unordered_map<const clang::Decl*, NameHierarchy>::const_iterator it = m_declNameCache.find(decl);
				if (it != m_declNameCache.end())
				{
					nameHierarchy = it->second;
				}
				else
				{
					nameHierarchy = utility::getDeclNameHierarchy(decl);
					m_declNameCache[decl] = nameHierarchy;
				}
				return nameHierarchy;
			}
			return NameHierarchy("global");
		}

	private:
		std::unordered_map<const clang::Decl*, NameHierarchy> m_declNameCache;
	};

	class ContextNameGenerator
	{
	public:
		virtual ~ContextNameGenerator() {}
		virtual NameHierarchy getName() const = 0;
	};

	class ContextDeclNameGenerator: public ContextNameGenerator
	{
	public:
		ContextDeclNameGenerator(const clang::NamedDecl* decl, std::shared_ptr<DeclNameCache> nameCache)
			: m_decl(decl)
			, m_nameCache(nameCache)
		{}

		virtual ~ContextDeclNameGenerator() {}

		virtual NameHierarchy getName() const
		{
			return m_nameCache->getName(m_decl);
		}
	private:
		const clang::NamedDecl* m_decl;
		std::shared_ptr<DeclNameCache> m_nameCache;
	};

	class ContextTypeNameGenerator: public ContextNameGenerator
	{
	public:
		ContextTypeNameGenerator(const clang::Type* type, std::shared_ptr<TypeNameCache> nameCache)
			: m_type(type)
			, m_nameCache(nameCache)
		{}

		virtual ~ContextTypeNameGenerator() {}

		virtual NameHierarchy getName() const
		{
			return m_nameCache->getName(m_type);
		}
	private:
		const clang::Type* m_type;
		std::shared_ptr<TypeNameCache> m_nameCache;
	};




	ASTVisitor(clang::ASTContext* context, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister);

	// Left for debugging purposes. Uncomment to see a colored ast-dump of the parsed file.
	virtual bool VisitTranslationUnitDecl(clang::TranslationUnitDecl* decl);


    void indexDecl(clang::Decl *d) { TraverseDecl(d); }

private:
	typedef clang::RecursiveASTVisitor<ASTVisitor> base;
	friend class clang::RecursiveASTVisitor<ASTVisitor>;

    // XXX: The CF_Read flag is useful mostly for lvalues -- for rvalues, we
    // don't set the CF_Read flag, but the rvalue is assumed to be read anyway.

    enum ContextFlags {
        CF_Called       = 0x1,      // the value is read only to be called
        CF_Read         = 0x2,      // the value is read for any other use
        CF_AddressTaken = 0x4,      // the gl-value's address escapes
        CF_Assigned     = 0x8,      // the gl-value is assigned to
        CF_Modified     = 0x10      // the gl-value is updated (i.e. compound assignment)
    };

	enum RefType : int {
		RT_AddressTaken,
		RT_Assigned,
		RT_BaseClass,
		RT_Called,
		RT_Declaration,
		RT_DefinedTest,
		RT_Definition,
		RT_Expansion,
		RT_Included,
		RT_Initialized,
		RT_Modified,
		RT_NamespaceAlias,
		RT_Other,
		RT_Qualifier,
		RT_Read,
		RT_Reference,
		RT_TemplateArgument,
		RT_TemplateDefaultArgument,
		RT_Undefinition,
		RT_Using,
		RT_UsingDirective,
		RT_Max
	};

	enum SymbolType : int { // we dont need this. decl already knows it!
		ST_Class,
		ST_ClassTemplateSpecialization,
		ST_Constructor,
		ST_Destructor,
		ST_Enum,
		ST_Enumerator,
		ST_Field,
		ST_Function,
		ST_FunctionTemplateSpecialization,
		ST_GlobalVariable,
		ST_LocalVariable,
		ST_Macro,
		ST_Method,
		ST_Namespace,
		ST_Parameter,
		ST_Path,
		ST_Struct,
		ST_Typedef,
		ST_TemplateParameter,
		ST_Union,
		ST_Max
	};

	struct Location {
		unsigned int fileID;
		unsigned int line;  // 1-based
		int column;         // 1-based
	};

    typedef unsigned int Context;

	clang::ASTContext* m_context;
	clang::Preprocessor* m_preprocessor;
	ParserClient* m_client;
	FileRegister* m_fileRegister;

    Context m_thisContext;
    Context m_childContext;
    RefType m_typeContext;

    // Misc routines
    bool shouldVisitTemplateInstantiations() const { return true; }
    bool shouldUseDataRecursionFor(clang::Stmt *s) const;

    // Dispatcher routines
    bool TraverseStmt(clang::Stmt *stmt);
    bool TraverseType(clang::QualType t);
    bool TraverseTypeLoc(clang::TypeLoc tl);
	bool TraverseDecl(clang::Decl *d);
	bool TraverseLambdaExpr(clang::LambdaExpr* e);
	bool TraverseFunctionDecl(clang::FunctionDecl* d);
	bool TraverseTypedefDecl(clang::TypedefDecl *d);
	bool TraverseFieldDecl(clang::FieldDecl *d);
	bool TraverseVarDecl(clang::VarDecl *d);
	bool TraverseClassTemplateDecl(clang::ClassTemplateDecl* d);
	bool TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d);
	bool TraverseTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d);
	bool TraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d);
	bool TraverseDeclRefExpr(clang::DeclRefExpr* expr);
	bool TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc);
	bool TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);

    // Expression context propagation
    bool TraverseCallExpr(clang::CallExpr *e) { return TraverseCallCommon(e); }
    bool TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr *e) { return TraverseCallCommon(e); }
    bool TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr *e) { return TraverseCallCommon(e); }
    bool TraverseBinComma(clang::BinaryOperator *s);
    bool TraverseBinAssign(clang::BinaryOperator *e) { return TraverseAssignCommon(e, CF_Assigned); }
#define OPERATOR(NAME) bool TraverseBin##NAME##Assign(clang::CompoundAssignOperator *e) { return TraverseAssignCommon(e, CF_Modified); }
    OPERATOR(Mul) OPERATOR(Div) OPERATOR(Rem) OPERATOR(Add) OPERATOR(Sub)
    OPERATOR(Shl) OPERATOR(Shr) OPERATOR(And) OPERATOR(Or)  OPERATOR(Xor)
#undef OPERATOR
    bool VisitParenExpr(clang::ParenExpr *e) { m_childContext = m_thisContext; return true; }
    bool VisitCastExpr(clang::CastExpr *e);
    bool VisitUnaryAddrOf(clang::UnaryOperator *e);
    bool VisitUnaryDeref(clang::UnaryOperator *e);
    bool VisitDeclStmt(clang::DeclStmt *s);
    bool VisitReturnStmt(clang::ReturnStmt *s);
    bool VisitVarDecl(clang::VarDecl *d);
    bool VisitInitListExpr(clang::InitListExpr *e);
    bool TraverseConstructorInitializer(clang::CXXCtorInitializer *init);
    bool TraverseCallCommon(clang::CallExpr *call);
    bool TraverseAssignCommon(clang::BinaryOperator *e, ContextFlags lhsFlag);

    // Expression reference recording
	bool VisitLambdaExpr(clang::LambdaExpr* e);
	bool VisitMemberExpr(clang::MemberExpr *e);
    bool VisitDeclRefExpr(clang::DeclRefExpr *e);
    bool VisitCXXConstructExpr(clang::CXXConstructExpr *e);
    void RecordDeclRefExpr(clang::NamedDecl *d, clang::SourceLocation loc, clang::Expr *e, Context context);

    // NestedNameSpecifier handling
    bool TraverseNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc qualifier);

    // Declaration and TypeLoc handling
    void traverseDeclContextHelper(clang::DeclContext *d);
	bool TraverseCXXRecordDecl(clang::CXXRecordDecl *d);
    bool TraverseNamespaceAliasDecl(clang::NamespaceAliasDecl *d);
    bool TraverseClassTemplateSpecializationDecl(
            clang::ClassTemplateSpecializationDecl *d);
    void templateParameterListsHelper(clang::DeclaratorDecl *d);
    bool VisitDecl(clang::Decl *d);
    bool VisitTypeLoc(clang::TypeLoc tl);

    // Reference recording
	ParseLocation getDeclRefRange(
            clang::NamedDecl *decl,
            clang::SourceLocation loc);

	void RecordTypeRef(
		const clang::Type* type,
		clang::SourceLocation beginLoc,
		RefType refType,
		SymbolType symbolType = ST_Max);

	bool isImpliciit(clang::Decl* d) const;

    void RecordDeclRef(
            clang::NamedDecl *d,
            clang::SourceLocation beginLoc,
            RefType refType,
            SymbolType symbolType = ST_Max);

	bool isLocatedInUnparsedProjectFile(clang::SourceLocation loc);
	bool isLocatedInProjectFile(clang::SourceLocation loc);
	
	ParserClient::AccessType convertAccessType(clang::AccessSpecifier access) const;
	ParserClient::AbstractionType getAbstractionType(const clang::CXXMethodDecl* methodDecl) const;
	ParseLocation getParseLocationOfRecordBody(clang::RecordDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	struct FileIdHash {
		size_t operator()(clang::FileID fileID) const {
			return fileID.getHashValue();
		}
	};

	std::unordered_map<const clang::FileID, bool, FileIdHash> m_inUnparsedProjectFileMap;
	std::unordered_map<const clang::FileID, bool, FileIdHash> m_inProjectFileMap;

	std::shared_ptr<ContextNameGenerator> m_contextNameGenerator;
	std::shared_ptr<ContextNameGenerator> m_childContextNameGenerator;

	std::shared_ptr<DeclNameCache> m_declNameCache;
	std::shared_ptr<TypeNameCache> m_typeNameCache;

	ParserClient::AccessType m_contextAccess;
};

#endif // AST_VISITOR_H
