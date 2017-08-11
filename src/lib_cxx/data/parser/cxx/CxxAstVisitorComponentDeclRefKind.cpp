#include "data/parser/cxx/CxxAstVisitorComponentDeclRefKind.h"

CxxAstVisitorComponentDeclRefKind::CxxAstVisitorComponentDeclRefKind(CxxAstVisitor* astVisitor)
	: CxxAstVisitorComponent(astVisitor)
	, m_thisRefKind(REFERENCE_USAGE)
	, m_childRefKind(REFERENCE_USAGE)
{
}

CxxAstVisitorComponentDeclRefKind::~CxxAstVisitorComponentDeclRefKind()
{
}

ReferenceKind CxxAstVisitorComponentDeclRefKind::getReferenceKind() const
{
	return m_thisRefKind;
}


void CxxAstVisitorComponentDeclRefKind::beginTraverseDecl(clang::Decl* d)
{
	saveAll();
	m_thisRefKind = REFERENCE_USAGE;
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::endTraverseDecl(clang::Decl* d)
{
	restoreAll();
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseStmt(clang::Stmt* s)
{
	saveAll();

	if (s == nullptr)
	{
		return;
	}

	m_thisRefKind = m_childRefKind;
	if (!clang::isa<clang::Expr>(s))
	{
		m_thisRefKind = REFERENCE_USAGE;
	}
	m_childRefKind = m_thisRefKind;
}

void CxxAstVisitorComponentDeclRefKind::endTraverseStmt(clang::Stmt* s)
{
	restoreAll();
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseType(const clang::QualType& t)
{
	saveAll();
	m_thisRefKind = REFERENCE_USAGE;
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::endTraverseType(const clang::QualType& t)
{
	restoreAll();
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseTypeLoc(const clang::TypeLoc& tl)
{
	saveAll();
	m_thisRefKind = REFERENCE_USAGE;
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::endTraverseTypeLoc(const clang::TypeLoc& tl)
{
	restoreAll();
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseCallCommonCallee()
{
	m_thisRefKind = REFERENCE_CALL;
	m_childRefKind = REFERENCE_CALL;
}
void CxxAstVisitorComponentDeclRefKind::beginTraverseCallCommonArgument()
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseBinCommaLhs()
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseBinCommaRhs()
{
	m_childRefKind = m_thisRefKind;
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseAssignCommonLhs()
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseAssignCommonRhs()
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseConstructorInitializer(clang::CXXCtorInitializer* init)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::beginTraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* s)
{
	m_thisRefKind = REFERENCE_CALL;
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitVarDecl(clang::VarDecl* d)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitCastExpr(clang::CastExpr* s)
{
	switch (s->getCastKind())
	{
	case clang::CK_ArrayToPointerDecay:
	case clang::CK_ToVoid:
	case clang::CK_LValueToRValue:
		m_childRefKind = REFERENCE_USAGE;
	default:
		break;
	}
}

void CxxAstVisitorComponentDeclRefKind::visitUnaryAddrOf(clang::UnaryOperator* s)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitUnaryDeref(clang::UnaryOperator* s)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitDeclStmt(clang::DeclStmt* s)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitReturnStmt(clang::ReturnStmt* s)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitInitListExpr(clang::InitListExpr* s)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::visitMemberExpr(clang::MemberExpr* s)
{
	m_childRefKind = REFERENCE_USAGE;
}

void CxxAstVisitorComponentDeclRefKind::saveAll()
{
	m_oldThisRefKinds.push_back(m_thisRefKind);
	m_oldChildRefKinds.push_back(m_childRefKind);
}

void CxxAstVisitorComponentDeclRefKind::restoreAll()
{
	m_thisRefKind = m_oldThisRefKinds.back();
	m_oldThisRefKinds.pop_back();

	m_childRefKind = m_oldChildRefKinds.back();
	m_oldChildRefKinds.pop_back();
}
