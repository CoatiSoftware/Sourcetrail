package io.coati;

import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.Comment;
import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.AnnotationDeclaration;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EmptyMemberDeclaration;
import com.github.javaparser.ast.body.EmptyTypeDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.EnumDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.InitializerDeclaration;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.TypeDeclaration;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.expr.*;
import com.github.javaparser.ast.nodeTypes.NodeWithAnnotations;
import com.github.javaparser.ast.nodeTypes.NodeWithArrays;
import com.github.javaparser.ast.stmt.AssertStmt;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.BreakStmt;
import com.github.javaparser.ast.stmt.CatchClause;
import com.github.javaparser.ast.stmt.ContinueStmt;
import com.github.javaparser.ast.stmt.DoStmt;
import com.github.javaparser.ast.stmt.EmptyStmt;
import com.github.javaparser.ast.stmt.ExplicitConstructorInvocationStmt;
import com.github.javaparser.ast.stmt.ExpressionStmt;
import com.github.javaparser.ast.stmt.ForStmt;
import com.github.javaparser.ast.stmt.ForeachStmt;
import com.github.javaparser.ast.stmt.IfStmt;
import com.github.javaparser.ast.stmt.LabeledStmt;
import com.github.javaparser.ast.stmt.ReturnStmt;
import com.github.javaparser.ast.stmt.Statement;
import com.github.javaparser.ast.stmt.SwitchEntryStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;
import com.github.javaparser.ast.stmt.SynchronizedStmt;
import com.github.javaparser.ast.stmt.ThrowStmt;
import com.github.javaparser.ast.stmt.TryStmt;
import com.github.javaparser.ast.stmt.TypeDeclarationStmt;
import com.github.javaparser.ast.stmt.WhileStmt;
import com.github.javaparser.ast.type.*;
import com.github.javaparser.ast.visitor.VoidVisitor;

import static com.github.javaparser.utils.Utils.isNullOrEmpty;

import java.util.List;
import java.util.Stack;

public abstract class JavaAstVisitorAdapter implements VoidVisitor<Void>
{
	private Stack<ReferenceKind> m_typeRefKind = new Stack<ReferenceKind>();
	
	protected ReferenceKind getTypeReferenceKind()
	{
		if (!m_typeRefKind.isEmpty())
		{
			return m_typeRefKind.peek();
		}
		return ReferenceKind.TYPE_USAGE;
	}
	
	//- Compilation Unit ----------------------------------
	
	@Override public void visit(CompilationUnit n, Void arg)
	{
		m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
		visitComment(n.getComment(), arg);
		if (n.getPackage() != null) 
		{
			n.getPackage().accept(this, arg);
		}
		if (n.getImports() != null) 
		{
			for (final ImportDeclaration i : n.getImports()) 
			{
				i.accept(this, arg);
			}
		}
		if (n.getTypes() != null) 
		{
            for (final TypeDeclaration<?> typeDeclaration : n.getTypes()) 
            {
				typeDeclaration.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
	}

	@Override public void visit(PackageDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
//		n.getName().accept(this, arg);
	}

	@Override public void visit(ImportDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
//		n.getName().accept(this, arg);
	}

	@Override public void visit(TypeParameter n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getTypeBound() != null) 
		{
			for (final ClassOrInterfaceType c : n.getTypeBound()) 
			{
				c.accept(this, arg);
			}
		}
	}

	@Override public void visit(LineComment n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(BlockComment n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	//- Body ----------------------------------------------

	@Override public void visit(ClassOrInterfaceDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
//		n.getNameExpr().accept(this, arg);
		for (final TypeParameter t : n.getTypeParameters()) 
		{
			t.accept(this, arg);
		}
		
		m_typeRefKind.push(ReferenceKind.INHERITANCE);
		for (final ClassOrInterfaceType c : n.getExtends()) 
		{
			c.accept(this, arg);
		}
		for (final ClassOrInterfaceType c : n.getImplements()) 
		{
			c.accept(this, arg);
		}
		m_typeRefKind.pop();
		
		for (final BodyDeclaration<?> member : n.getMembers()) 
		{
			member.accept(this, arg);
		}
	}

	@Override public void visit(EnumDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		
//		n.getNameExpr().accept(this, arg);
		
		m_typeRefKind.push(ReferenceKind.INHERITANCE);
		if (n.getImplements() != null) 
		{
			for (final ClassOrInterfaceType c : n.getImplements()) 
			{
				c.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
		
		if (n.getEntries() != null) 
		{
			for (final EnumConstantDeclaration e : n.getEntries()) 
			{
				e.accept(this, arg);
			}
		}
		if (n.getMembers() != null) 
		{
			for (final BodyDeclaration<?> member : n.getMembers())
			{
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(EmptyTypeDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
//		n.getNameExpr().accept(this, arg);
	}

	@Override public void visit(EnumConstantDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		if (n.getArgs() != null) 
		{
			for (final Expression e : n.getArgs()) 
			{
				e.accept(this, arg);
			}
		}
		if (n.getClassBody() != null)
		{
			for (final BodyDeclaration<?> member : n.getClassBody()) 
			{
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(AnnotationDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
//		n.getNameExpr().accept(this, arg);
		if (n.getMembers() != null) 
		{
			for (final BodyDeclaration<?> member : n.getMembers()) 
			{
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(AnnotationMemberDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		
		n.getType().accept(this, arg);
		if (n.getDefaultValue() != null) 
		{
			n.getDefaultValue().accept(this, arg);
		}
	}

	@Override public void visit(FieldDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		n.getType().accept(this, arg);
		for (final VariableDeclarator var : n.getVariables()) 
		{
			var.accept(this, arg);
		}
	}

	@Override public void visit(VariableDeclarator n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getId().accept(this, arg);
		if (n.getInit() != null) 
		{
			n.getInit().accept(this, arg);
		}
	}

	@Override public void visit(VariableDeclaratorId n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(ConstructorDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		
		if (n.getTypeParameters() != null)
		{
			for (final TypeParameter t : n.getTypeParameters())
			{
				t.accept(this, arg);
			}
		}
//		n.getNameExpr().accept(this, arg);
		if (n.getParameters() != null)
		{
			for (final Parameter p : n.getParameters())
			{
				p.accept(this, arg);
			}
		}
		if (n.getThrows() != null)
		{
			for (final ReferenceType name : n.getThrows())
			{
				name.accept(this, arg);
			}
		}
		n.getBody().accept(this, arg);
	}

	@Override public void visit(MethodDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		
		if (n.getTypeParameters() != null) 
		{
			for (final TypeParameter t : n.getTypeParameters()) 
			{
				t.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
//		n.getNameExpr().accept(this, arg);
		if (n.getParameters() != null)
		{
			for (final Parameter p : n.getParameters())
			{
				p.accept(this, arg);
			}
		}
		if (n.getThrows() != null) 
		{
			for (final ReferenceType name : n.getThrows())
			{
				name.accept(this, arg);
			}
		}
		if (n.getBody() != null) 
		{
			n.getBody().accept(this, arg);
		}
	}

	@Override public void visit(Parameter n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		
		n.getType().accept(this, arg);
		n.getId().accept(this, arg);
	}

	@Override public void visit(EmptyMemberDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(InitializerDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		
		n.getBlock().accept(this, arg);
	}

	@Override public void visit(JavadocComment n, Void arg)
	{
	}

	//- Type ----------------------------------------------

	@Override public void visit(ClassOrInterfaceType n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
//		if (n.getScope() != null) // don't visit the qualifier here.
//		{
//			n.getScope().accept(this, arg);
//		}
		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArgs() != null) 
		{
			for (final Type t : n.getTypeArgs()) 
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
	}

	@Override public void visit(PrimitiveType n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
	}

	@Override public void visit(ReferenceType n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		visitArraysAnnotations(n, arg);
		n.getType().accept(this, arg);
	}

    @Override public void visit(IntersectionType n, Void arg)
	{
    	visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
        for (ReferenceType element : n.getElements()) 
        {
            element.accept(this, arg);
        }
	}

    @Override public void visit(UnionType n, Void arg)
	{
    	visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
        for (ReferenceType element : n.getElements()) 
        {
            element.accept(this, arg);
        }
	}

	@Override public void visit(VoidType n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
	}

	@Override public void visit(WildcardType n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		if (n.getExtends() != null)
		{
			n.getExtends().accept(this, arg);
		}
		if (n.getSuper() != null)
		{
			n.getSuper().accept(this, arg);
		}
	}

	@Override public void visit(UnknownType n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	//- Expression ----------------------------------------

	@Override public void visit(ArrayAccessExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
		n.getIndex().accept(this, arg);
	}

	@Override public void visit(ArrayCreationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitArraysAnnotations(n, arg);
		n.getType().accept(this, arg);
		if (!isNullOrEmpty(n.getDimensions())) 
		{
			for (final Expression dim : n.getDimensions()) 
			{
				dim.accept(this, arg);
			}
		} 
		if (n.getInitializer() != null)
		{
			n.getInitializer().accept(this, arg);
		}
	}

	@Override public void visit(ArrayInitializerExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getValues() != null) 
		{
			for (final Expression expr : n.getValues()) 
			{
				expr.accept(this, arg);
			}
		}
	}

	@Override public void visit(AssignExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getTarget().accept(this, arg);
		n.getValue().accept(this, arg);
	}

	@Override public void visit(BinaryExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getLeft().accept(this, arg);
		n.getRight().accept(this, arg);
	}

	@Override public void visit(CastExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getType().accept(this, arg);
		n.getExpr().accept(this, arg);
	}

	@Override public void visit(ClassExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getType().accept(this, arg);
	}

	@Override public void visit(ConditionalExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getCondition().accept(this, arg);
		n.getThenExpr().accept(this, arg);
		n.getElseExpr().accept(this, arg);
	}

	@Override public void visit(EnclosedExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getInner().accept(this, arg);
	}

	@Override public void visit(FieldAccessExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getScope().accept(this, arg); 
		n.getFieldExpr().accept(this, arg); // hmm.. this could be a name Expr
	}

	@Override public void visit(InstanceOfExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
		n.getType().accept(this, arg);
	}

	@Override public void visit(StringLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(IntegerLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(LongLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(IntegerLiteralMinValueExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(LongLiteralMinValueExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(CharLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(DoubleLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(BooleanLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(NullLiteralExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(MethodCallExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getScope() != null) 
		{
			n.getScope().accept(this, arg);
		}
		
		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArgs() != null) 
		{
			for (final Type t : n.getTypeArgs()) 
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();

//		n.getNameExpr().accept(this, arg);
		if (n.getArgs() != null)
		{
			for (final Expression e : n.getArgs())
			{
				e.accept(this, arg);
			}
		}
	}

	@Override public void visit(NameExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(ObjectCreationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getScope() != null) 
		{
			n.getScope().accept(this, arg);
		}
		
		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArgs() != null)
		{
			for (final Type t : n.getTypeArgs())
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
		
		n.getType().accept(this, arg);
		if (n.getArgs() != null) 
		{
			for (final Expression e : n.getArgs()) 
			{
				e.accept(this, arg);
			}
		}
		if (n.getAnonymousClassBody() != null) 
		{
			for (final BodyDeclaration<?> member : n.getAnonymousClassBody())
			{
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(QualifiedNameExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getQualifier().accept(this, arg);
	}

	@Override public void visit(ThisExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getClassExpr() != null)
		{
			n.getClassExpr().accept(this, arg);
		}
	}

	@Override public void visit(SuperExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getClassExpr() != null)
		{
			n.getClassExpr().accept(this, arg);
		}
	}

	@Override public void visit(UnaryExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
	}

	@Override public void visit(VariableDeclarationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		n.getType().accept(this, arg);
		for (final VariableDeclarator v : n.getVars())
		{
			v.accept(this, arg);
		}
	}

	@Override public void visit(MarkerAnnotationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
	}

	@Override public void visit(SingleMemberAnnotationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
		n.getMemberValue().accept(this, arg);
	}

	@Override public void visit(NormalAnnotationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
		if (n.getPairs() != null) 
		{
			for (final MemberValuePair m : n.getPairs())
			{
				m.accept(this, arg);
			}
		}
	}

	@Override public void visit(MemberValuePair n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getValue().accept(this, arg);
	}

	//- Statements ----------------------------------------

	@Override public void visit(ExplicitConstructorInvocationStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (!n.isThis() && n.getExpr() != null) 
		{
			n.getExpr().accept(this, arg);
		}

		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArgs() != null) {
			for (final Type t : n.getTypeArgs()) 
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
		
		if (n.getArgs() != null) {
			for (final Expression e : n.getArgs())
			{
				e.accept(this, arg);
			}
		}
	}

	@Override public void visit(TypeDeclarationStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getTypeDeclaration().accept(this, arg);
	}

	@Override public void visit(AssertStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getCheck().accept(this, arg);
		if (n.getMessage() != null) 
		{
			n.getMessage().accept(this, arg);
		}
	}

	@Override public void visit(BlockStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getStmts() != null) 
		{
			for (final Statement s : n.getStmts())
			{
				s.accept(this, arg);
			}
		}
	}

	@Override public void visit(LabeledStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getStmt().accept(this, arg);
	}

	@Override public void visit(EmptyStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(ExpressionStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getExpression().accept(this, arg);
	}

	@Override public void visit(SwitchStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getSelector().accept(this, arg);
		if (n.getEntries() != null) 
		{
			for (final SwitchEntryStmt e : n.getEntries())
			{
				e.accept(this, arg);
			}
		}
	}

	@Override public void visit(SwitchEntryStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getLabel() != null) 
		{
			n.getLabel().accept(this, arg);
		}
		if (n.getStmts() != null)
		{
			for (final Statement s : n.getStmts()) 
			{
				s.accept(this, arg);
			}
		}
	}

	@Override public void visit(BreakStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(ReturnStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getExpr() != null) 
		{
			n.getExpr().accept(this, arg);
		}
	}

	@Override public void visit(IfStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getCondition().accept(this, arg);
		n.getThenStmt().accept(this, arg);
		if (n.getElseStmt() != null) 
		{
			n.getElseStmt().accept(this, arg);
		}
	}

	@Override public void visit(WhileStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getCondition().accept(this, arg);
		n.getBody().accept(this, arg);
	}

	@Override public void visit(ContinueStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(DoStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getBody().accept(this, arg);
		n.getCondition().accept(this, arg);
	}

	@Override public void visit(ForeachStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getVariable().accept(this, arg);
		n.getIterable().accept(this, arg);
		n.getBody().accept(this, arg);
	}

	@Override public void visit(ForStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getInit() != null) 
		{
			for (final Expression e : n.getInit())
			{
				e.accept(this, arg);
			}
		}
		if (n.getCompare() != null)
		{
			n.getCompare().accept(this, arg);
		}
		if (n.getUpdate() != null) 
		{
			for (final Expression e : n.getUpdate())
			{
				e.accept(this, arg);
			}
		}
		n.getBody().accept(this, arg);
	}

	@Override public void visit(ThrowStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
	}

	@Override public void visit(SynchronizedStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
		n.getBlock().accept(this, arg);
	}

	@Override public void visit(TryStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getResources() != null) 
		{
			for (final VariableDeclarationExpr v : n.getResources()) 
			{
				v.accept(this, arg);
			}
		}
		n.getTryBlock().accept(this, arg);
		if (n.getCatchs() != null) 
		{
			for (final CatchClause c : n.getCatchs()) 
			{
				c.accept(this, arg);
			}
		}
		if (n.getFinallyBlock() != null)
		{
			n.getFinallyBlock().accept(this, arg);
		}
	}

	@Override public void visit(CatchClause n, Void arg)
	{
		visitComment(n.getComment(), arg);
		n.getParam().accept(this, arg);
		n.getCatchBlock().accept(this, arg);
	}

    @Override public void visit(LambdaExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getParameters() != null) 
		{
			for (final Parameter a : n.getParameters()) 
			{
				a.accept(this, arg);
			}
		}
		if (n.getBody() != null) 
		{
			n.getBody().accept(this, arg);
		}
	}

    @Override public void visit(MethodReferenceExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getTypeArguments().getTypeArguments() != null) 
		{
		    for (final Type t : n.getTypeArguments().getTypeArguments()) 
		    {
			    t.accept(this, arg);
		    }
	    }
		if (n.getScope() != null) 
		{
			n.getScope().accept(this, arg);
		}
	}

    @Override public void visit(TypeExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getType() != null)
		{
			n.getType().accept(this, arg);
		}
	}
    
    private void visitComment(final Comment n, final Void arg) 
    {
		if (n != null) 
		{
			n.accept(this, arg);
		}
	}
    
    private void visitAnnotations(NodeWithAnnotations<?> n, final Void arg) 
    {
		for (AnnotationExpr annotation : n.getAnnotations()) 
		{
			annotation.accept(this, arg);
		}
	}
    
    private void visitArraysAnnotations(NodeWithArrays<?> n, final Void arg) 
    {
		for (List<AnnotationExpr> aux : n.getArraysAnnotations()) 
		{
			if (aux != null) 
			{
				for (AnnotationExpr annotation : aux)
				{
					annotation.accept(this, arg);
				}
			}
		}
	}
}
