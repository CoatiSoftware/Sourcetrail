package com.sourcetrail;

import java.io.File;
import java.util.List;
import java.util.Stack;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.ClassInstanceCreation;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.ConstructorInvocation;
import org.eclipse.jdt.core.dom.CreationReference;
import org.eclipse.jdt.core.dom.EnumConstantDeclaration;
import org.eclipse.jdt.core.dom.EnumDeclaration;
import org.eclipse.jdt.core.dom.ExpressionMethodReference;
import org.eclipse.jdt.core.dom.FieldDeclaration;
import org.eclipse.jdt.core.dom.ImportDeclaration;
import org.eclipse.jdt.core.dom.Javadoc;
import org.eclipse.jdt.core.dom.MethodInvocation;
import org.eclipse.jdt.core.dom.NameQualifiedType;
import org.eclipse.jdt.core.dom.ParameterizedType;
import org.eclipse.jdt.core.dom.QualifiedType;
import org.eclipse.jdt.core.dom.SuperConstructorInvocation;
import org.eclipse.jdt.core.dom.SuperMethodInvocation;
import org.eclipse.jdt.core.dom.SuperMethodReference;
import org.eclipse.jdt.core.dom.TypeDeclaration;
import org.eclipse.jdt.core.dom.TypeMethodReference;
import org.eclipse.jdt.core.dom.VariableDeclarationFragment;

public class ContextAwareAstVisitor extends AstVisitor
{
	private Stack<ReferenceKind> m_typeRefKind = new Stack<>();

	public ContextAwareAstVisitor(
		AstVisitorClient client, File filePath, String fileContent, CompilationUnit compilationUnit)
	{
		super(client, filePath, fileContent, compilationUnit);
	}

	@Override public boolean visit(TypeDeclaration node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getJavadoc());
			acceptChildren(node.modifiers());
			acceptChild(node.getName());
			acceptChildren(node.typeParameters());
			{
				m_typeRefKind.push(ReferenceKind.INHERITANCE);
				acceptChild(node.getSuperclassType());
				acceptChildren(node.superInterfaceTypes());
				m_typeRefKind.pop();
			}
			acceptChildren(node.bodyDeclarations());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(EnumDeclaration node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getJavadoc());
			acceptChildren(node.modifiers());
			acceptChild(node.getName());
			{
				m_typeRefKind.push(ReferenceKind.INHERITANCE);
				acceptChildren(node.superInterfaceTypes());
				m_typeRefKind.pop();
			}
			acceptChildren(node.enumConstants());
			acceptChildren(node.bodyDeclarations());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(EnumConstantDeclaration node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			// we don't visit and record the name of enum constant declarations because
			// this would create an unwanted self-reference.
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getJavadoc());
			acceptChildren(node.modifiers());
			acceptChildren(node.arguments());
			acceptChild(node.getAnonymousClassDeclaration());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(VariableDeclarationFragment node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			// we don't visit and record the name of field declarations because
			// this would create an unwanted self-reference.
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			if (!(node.getParent() instanceof FieldDeclaration))
			{
				acceptChild(node.getName());
			}
			acceptChildren(node.extraDimensions());
			acceptChild(node.getInitializer());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(final ImportDeclaration node)
	{
		// We don't want to visit the name of the ImportDeclaration because this could cause a self
		// reference.
		super.visit(node);
		return false;
	}

	@Override public boolean visit(ParameterizedType node)
	{
		boolean visitChildren = super.visit(node);
		if (visitChildren)
		{
			acceptChild(node.getType());
			{
				m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
				acceptChildren(node.typeArguments());
				m_typeRefKind.pop();
			}
		}

		return false;
	}

	@Override public boolean visit(QualifiedType node)
	{
		// We don't want to visit the qualifier right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.annotations());
			acceptChild(node.getName());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(NameQualifiedType node)
	{
		// We don't want to visit the qualifier right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.annotations());
			acceptChild(node.getName());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(MethodInvocation node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getExpression());
			acceptChildren(node.typeArguments());
			acceptChild(node.getName());
			acceptChildren(node.arguments());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(SuperMethodInvocation node)
	{
		// We don't want to visit the qualifier right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.typeArguments());
			acceptChild(node.getName());
			acceptChildren(node.arguments());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(ConstructorInvocation node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.typeArguments());
			acceptChildren(node.arguments());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(SuperConstructorInvocation node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getExpression());
			acceptChildren(node.typeArguments());
			acceptChildren(node.arguments());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(CreationReference node)
	{
		// We don't want to visit the qualifying type right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.typeArguments());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(ExpressionMethodReference node)
	{
		// We don't want to visit qualifiers right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getExpression());
			acceptChildren(node.typeArguments());
			acceptChild(node.getName());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(SuperMethodReference node)
	{
		// We don't want to visit qualifiers right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.typeArguments());
			acceptChild(node.getName());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(TypeMethodReference node)
	{
		// We don't want to visit the qualifying type right now.
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChildren(node.typeArguments());
			acceptChild(node.getName());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(ClassInstanceCreation node)
	{
		boolean visitChildren = super.visit(node);

		if (visitChildren)
		{
			m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
			acceptChild(node.getExpression());
			acceptChildren(node.typeArguments());

			if (node.getAnonymousClassDeclaration() != null)
			{
				m_typeRefKind.push(ReferenceKind.INHERITANCE);
				acceptChild(node.getType());
				m_typeRefKind.pop();
			}
			else
			{
				acceptChild(node.getType());
			}

			acceptChildren(node.arguments());
			acceptChild(node.getAnonymousClassDeclaration());
			m_typeRefKind.pop();
		}

		return false;
	}

	@Override public boolean visit(Javadoc node)
	{
		// We don't want to visit symbol references inside Javadoc right now.
		super.visit(node);
		return false;
	}

	@Override protected ReferenceKind getTypeReferenceKind()
	{
		if (!m_typeRefKind.isEmpty())
		{
			return m_typeRefKind.peek();
		}
		return ReferenceKind.TYPE_USAGE;
	}

	private void acceptChild(ASTNode node)
	{
		if (node != null)
		{
			node.accept(this);
		}
	}

	private void acceptChildren(List<?> nodes)
	{
		if (nodes != null)
		{
			for (Object node: nodes)
			{
				if (node instanceof ASTNode)
				{
					acceptChild((ASTNode)node);
				}
			}
		}
	}
}
