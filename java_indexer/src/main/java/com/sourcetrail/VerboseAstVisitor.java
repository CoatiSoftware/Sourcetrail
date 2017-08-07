package com.sourcetrail;

import com.github.javaparser.ast.*;
import com.github.javaparser.ast.body.*;
import com.github.javaparser.ast.comments.*;
import com.github.javaparser.ast.expr.*;
import com.github.javaparser.ast.nodeTypes.NodeWithName;
import com.github.javaparser.ast.stmt.*;
import com.github.javaparser.ast.type.*;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

public class VerboseAstVisitor extends AstVisitor{
	
	public VerboseAstVisitor(AstVisitorClient client, String filePath, FileContent fileContent, TypeSolver typeSolver) {
		super(client, filePath, fileContent, typeSolver);
	}

	int indent = 0;
	String indentSymbol = "| ";
	
	private String obfuscate(final String s) 
	{
		if (s.isEmpty())
		{
			return "";
		}
		else if (s.length() <= 2)
		{
			return s;
		}
		return s.substring(0, 1) + ".." + s.substring(s.length() - 1, s.length());
	}
	
	private void dump(Node n)
	{
		String line = "";
		
		for (int i = 0; i < this.indent; i++)
		{
			line += this.indentSymbol;
		}
		
		line += n.getClass().getName();
		if (n instanceof NodeWithName<?>)
		{
			line += " [" + obfuscate(((NodeWithName<?>)n).getNameAsString()) + "]";
		}
		
		if (n.getBegin().isPresent())
		{
			line += " line: " + n.getBegin().get().line;
		}
		
		m_client.logInfo(line);
	}
	
	 //- Compilation Unit ----------------------------------

	public Boolean visit(CompilationUnit n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(PackageDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ImportDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }
	
	public Boolean visit(TypeParameter n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(LineComment n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(BlockComment n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	//- Body ----------------------------------------------

	public Boolean visit(ClassOrInterfaceDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(EnumDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(EnumConstantDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(AnnotationDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(AnnotationMemberDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(FieldDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(VariableDeclarator n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ConstructorDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(MethodDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(Parameter n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(InitializerDeclaration n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(JavadocComment n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	//- Type ----------------------------------------------

	public Boolean visit(ClassOrInterfaceType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(PrimitiveType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ArrayType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ArrayCreationLevel n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

    public Boolean visit(IntersectionType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

    public Boolean visit(UnionType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(VoidType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(WildcardType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(UnknownType n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	//- Expression ----------------------------------------

	public Boolean visit(ArrayAccessExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ArrayCreationExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ArrayInitializerExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(AssignExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(BinaryExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(CastExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ClassExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ConditionalExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(EnclosedExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(FieldAccessExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(InstanceOfExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(StringLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(IntegerLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(LongLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(CharLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(DoubleLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(BooleanLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(NullLiteralExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(MethodCallExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(NameExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ObjectCreationExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ThisExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(SuperExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(UnaryExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(VariableDeclarationExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(MarkerAnnotationExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(SingleMemberAnnotationExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(NormalAnnotationExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(MemberValuePair n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	//- Statements ----------------------------------------

	public Boolean visit(ExplicitConstructorInvocationStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(AssertStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(BlockStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(LabeledStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(EmptyStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ExpressionStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(SwitchStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(SwitchEntryStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(BreakStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ReturnStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(IfStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(WhileStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ContinueStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(DoStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ForeachStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ForStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(ThrowStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(SynchronizedStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(TryStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

	public Boolean visit(CatchClause n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

    public Boolean visit(LambdaExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

    public Boolean visit(MethodReferenceExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

    public Boolean visit(TypeExpr n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }

    public Boolean visit(UnparsableStmt n, Void v) { dump(n); indent++; Boolean result = super.visit(n, v); indent--; return result; }
}
