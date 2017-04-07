package com.sourcetrail;

import com.github.javaparser.ast.visitor.VoidVisitorAdapter;

import com.github.javaparser.ast.*;
import com.github.javaparser.ast.body.*;
import com.github.javaparser.ast.comments.Comment;
import com.github.javaparser.ast.expr.*;
import com.github.javaparser.ast.nodeTypes.NodeWithAnnotations;
import com.github.javaparser.ast.stmt.*;
import com.github.javaparser.ast.type.*;

import java.util.Optional;
import java.util.Stack;

public abstract class JavaAstVisitorAdapter extends VoidVisitorAdapter<Void>
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
	
	@Override public void visit(final CompilationUnit n, Void arg)
	{
		m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
		visitComment(n.getComment(), arg);
		if (n.getPackageDeclaration().isPresent()) 
		{
			n.getPackageDeclaration().get().accept(this, arg);
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

	//- Body ----------------------------------------------

	@Override public void visit(ClassOrInterfaceDeclaration n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
        n.getName().accept(this, arg);
		for (final TypeParameter t : n.getTypeParameters()) 
		{
			t.accept(this, arg);
		}
		
		m_typeRefKind.push(ReferenceKind.INHERITANCE);
		for (final ClassOrInterfaceType c : n.getExtendedTypes()) 
		{
			c.accept(this, arg);
		}
		for (final ClassOrInterfaceType c : n.getImplementedTypes()) 
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
        n.getName().accept(this, arg);
		
		m_typeRefKind.push(ReferenceKind.INHERITANCE);
		if (n.getImplementedTypes() != null) 
		{
			for (final ClassOrInterfaceType c : n.getImplementedTypes()) 
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

	//- Type ----------------------------------------------

	@Override public void visit(ClassOrInterfaceType n, Void arg)
	{
		visitComment(n.getComment(), arg);
		visitAnnotations(n, arg);
		// don't visit the qualifier here.
//		if (n.getScope().isPresent()) 
//		{
//			n.getScope().get().accept(this, arg);
//		}
		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArguments().isPresent())
		{
			for (final Type t : n.getTypeArguments().get())
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
	}

	//- Expression ----------------------------------------

	@Override public void visit(MethodCallExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getScope().isPresent()) 
		{
			n.getScope().get().accept(this, arg);
		}
		
		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArguments().isPresent())
		{
			for (final Type t : n.getTypeArguments().get())
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();

		n.getName().accept(this, arg);
		if (n.getArguments() != null)
		{
			for (final Expression e : n.getArguments())
			{
				e.accept(this, arg);
			}
		}
	}
	
	@Override public void visit(ObjectCreationExpr n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (n.getScope().isPresent()) 
		{
			n.getScope().get().accept(this, arg);
		}
		
		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArguments().isPresent())
		{
			for (final Type t : n.getTypeArguments().get())
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
		
//		n.getType().accept(this, arg);
		if (n.getArguments() != null) 
		{
			for (final Expression e : n.getArguments()) 
			{
				e.accept(this, arg);
			}
		}
		if (n.getAnonymousClassBody().isPresent()) 
		{
            for (final BodyDeclaration<?> member : n.getAnonymousClassBody().get()) 
            {
				member.accept(this, arg);
			}
		}
	}

	//- Statements ----------------------------------------

	@Override public void visit(ExplicitConstructorInvocationStmt n, Void arg)
	{
		visitComment(n.getComment(), arg);
		if (!n.isThis() && n.getExpression().isPresent()) 
		{
			n.getExpression().get().accept(this, arg);
		}

		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
		if (n.getTypeArguments().isPresent())
		{
			for (final Type t : n.getTypeArguments().get())
			{
				t.accept(this, arg);
			}
		}
		m_typeRefKind.pop();
		
		if (n.getArguments() != null) 
		{
			for (final Expression e : n.getArguments())
			{
				e.accept(this, arg);
			}
		}
	}

	private void visitComment(final Optional<Comment> n, final Void arg) 
	{
		if (n.isPresent())
		{
			n.get().accept(this, arg);
		}
	}
	
	private void visitAnnotations(NodeWithAnnotations<?> n, final Void arg) 
	{
		for (AnnotationExpr annotation : n.getAnnotations()) 
		{
			annotation.accept(this, arg);
		}
	}
}
