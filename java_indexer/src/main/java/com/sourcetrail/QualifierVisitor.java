package com.sourcetrail;

import com.sourcetrail.name.DeclName;
import com.sourcetrail.name.NameHierarchy;
import com.sourcetrail.name.TypeName;
import com.sourcetrail.name.resolver.BindingNameResolver;
import java.io.File;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.CreationReference;
import org.eclipse.jdt.core.dom.Expression;
import org.eclipse.jdt.core.dom.ExpressionMethodReference;
import org.eclipse.jdt.core.dom.FieldAccess;
import org.eclipse.jdt.core.dom.IBinding;
import org.eclipse.jdt.core.dom.IMethodBinding;
import org.eclipse.jdt.core.dom.IPackageBinding;
import org.eclipse.jdt.core.dom.ITypeBinding;
import org.eclipse.jdt.core.dom.IVariableBinding;
import org.eclipse.jdt.core.dom.ImportDeclaration;
import org.eclipse.jdt.core.dom.MethodInvocation;
import org.eclipse.jdt.core.dom.Name;
import org.eclipse.jdt.core.dom.NameQualifiedType;
import org.eclipse.jdt.core.dom.QualifiedName;
import org.eclipse.jdt.core.dom.QualifiedType;
import org.eclipse.jdt.core.dom.SimpleType;
import org.eclipse.jdt.core.dom.SuperFieldAccess;
import org.eclipse.jdt.core.dom.SuperMethodInvocation;
import org.eclipse.jdt.core.dom.SuperMethodReference;
import org.eclipse.jdt.core.dom.ThisExpression;
import org.eclipse.jdt.core.dom.Type;
import org.eclipse.jdt.core.dom.TypeMethodReference;

public class QualifierVisitor
{
	protected AstVisitorClient m_client = null;
	private File m_filePath;
	private CompilationUnit m_compilationUnit;
	private boolean m_recordSymbolKinds = false;

	public QualifierVisitor(
		AstVisitorClient client, File filePath, CompilationUnit compilationUnit, boolean recordSymbolKinds)
	{
		m_client = client;
		m_filePath = filePath;
		m_compilationUnit = compilationUnit;
		m_recordSymbolKinds = recordSymbolKinds;
	}

	public void recordQualifierOfNode(ImportDeclaration node)
	{
		if (node != null && node.getName() instanceof QualifiedName)
		{
			recordNodeAsQualifier(((QualifiedName)node.getName()).getQualifier());
		}
	}

	public void recordQualifierOfNode(SimpleType node)
	{
		if (node != null)
		{
			if (node.getName() instanceof QualifiedName)
			{
				recordNodeAsQualifier(((QualifiedName)node.getName()).getQualifier());
			}
		}
	}

	public void recordQualifierOfNode(QualifiedType node)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getQualifier());
		}
	}

	public void recordQualifierOfNode(NameQualifiedType node)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getQualifier());
		}
	}

	public void recordQualifierOfNode(QualifiedName node)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getQualifier());
		}
	}

	public void recordQualifierOfNode(FieldAccess node, FileContent fileContent)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getExpression(), fileContent);
		}
	}

	public void recordQualifierOfNode(SuperFieldAccess node, FileContent fileContent)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node, fileContent);
		}
	}

	public void recordQualifierOfNode(ThisExpression node, FileContent fileContent)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getQualifier());
		}
	}

	public void recordQualifierOfNode(MethodInvocation node, FileContent fileContent)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getExpression(), fileContent);
		}
	}

	public void recordQualifierOfNode(SuperMethodInvocation node, FileContent fileContent)
	{
		if (node != null)
		{
			IMethodBinding methodBinding = node.resolveMethodBinding();
			if (methodBinding != null)
			{
				ITypeBinding typeBinding = methodBinding.getDeclaringClass();
				if (typeBinding != null)
				{
					m_client.recordQualifierLocation(
						BindingNameResolver
							.getQualifiedName(typeBinding, m_filePath, m_compilationUnit)
							.orElse(TypeName.unsolved())
							.toDeclName()
							.toNameHierarchy(),
						fileContent.findRange("super", getRange(node).begin));
				}
			}

			recordNodeAsQualifier(node.getQualifier());
		}
	}

	public void recordQualifierOfNode(CreationReference node)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getType());
		}
	}

	public void recordQualifierOfNode(ExpressionMethodReference node, FileContent fileContent)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getExpression(), fileContent);
		}
	}

	public void recordQualifierOfNode(SuperMethodReference node, FileContent fileContent)
	{
		if (node != null)
		{
			IMethodBinding methodBinding = node.resolveMethodBinding();
			if (methodBinding != null)
			{
				ITypeBinding typeBinding = methodBinding.getDeclaringClass();
				if (typeBinding != null)
				{
					m_client.recordQualifierLocation(
						BindingNameResolver
							.getQualifiedName(typeBinding, m_filePath, m_compilationUnit)
							.orElse(TypeName.unsolved())
							.toDeclName()
							.toNameHierarchy(),
						fileContent.findRange("super", getRange(node).begin));
				}
			}

			recordNodeAsQualifier(node.getQualifier());
		}
	}

	public void recordQualifierOfNode(TypeMethodReference node)
	{
		if (node != null)
		{
			recordNodeAsQualifier(node.getType());
		}
	}


	private void recordNodeAsQualifier(Expression node, FileContent fileContent)
	{
		if (node != null)
		{
			if (node instanceof Name)
			{
				recordNodeAsQualifier((Name)node);
			}
			else if (node instanceof SuperFieldAccess)
			{
				SuperFieldAccess expression = (SuperFieldAccess)node;
				IVariableBinding fieldBinding = expression.resolveFieldBinding();

				if (fieldBinding != null)
				{
					m_client.recordQualifierLocation(
						BindingNameResolver
							.getQualifiedName(
								fieldBinding.getDeclaringClass(), m_filePath, m_compilationUnit)
							.orElse(TypeName.unsolved())
							.toDeclName()
							.toNameHierarchy(),
						fileContent.findRange(
							"super",
							expression.getQualifier() != null ? getRange(expression.getQualifier()).end
															  : getRange(expression).begin));
				}

				recordNodeAsQualifier(expression.getQualifier());
			}
			else if (node instanceof ThisExpression)
			{
				ThisExpression expression = (ThisExpression)node;

				m_client.recordQualifierLocation(
					BindingNameResolver
						.getQualifiedName(
							expression.resolveTypeBinding(), m_filePath, m_compilationUnit)
						.orElse(TypeName.unsolved())
						.toDeclName()
						.toNameHierarchy(),
					fileContent.findRange(
						"this",
						expression.getQualifier() != null ? getRange(expression.getQualifier()).end
														  : getRange(expression).begin));

				recordNodeAsQualifier(expression.getQualifier());
			}
		}
	}

	private void recordNodeAsQualifier(Type node)
	{
		if (node != null)
		{
			if (node instanceof SimpleType)
			{
				recordNodeAsQualifier(((SimpleType)node).getName());
			}
			else if (node instanceof QualifiedType)
			{
				recordNodeAsQualifier(((QualifiedType)node).getName());
				recordNodeAsQualifier(((QualifiedType)node).getQualifier());
			}
			else if (node instanceof NameQualifiedType)
			{
				recordNodeAsQualifier(((NameQualifiedType)node).getName());
				recordNodeAsQualifier(((NameQualifiedType)node).getQualifier());
			}
		}
	}

	private void recordNodeAsQualifier(Name node)
	{
		if (node != null)
		{
			IBinding binding = node.resolveBinding();
			if (binding instanceof ITypeBinding)
			{
				Range range = getRange(node);
				if (node instanceof QualifiedName)
				{
					range = getRange(((QualifiedName)node).getName());
				}

				m_client.recordQualifierLocation(
					BindingNameResolver
						.getQualifiedName((ITypeBinding)binding, m_filePath, m_compilationUnit)
						.orElse(TypeName.unsolved())
						.toDeclName()
						.toNameHierarchy(),
					range);
			}
			else if (binding instanceof IPackageBinding)
			{
				Range range = getRange(node);
				if (node instanceof QualifiedName)
				{
					range = getRange(((QualifiedName)node).getName());
				}

				NameHierarchy symbolName =
					BindingNameResolver
						.getQualifiedName((IPackageBinding)binding, m_filePath, m_compilationUnit)
						.orElse(DeclName.unsolved())
						.toNameHierarchy();

				if (m_recordSymbolKinds)
				{
					m_client.recordSymbol(
						symbolName, SymbolKind.PACKAGE, AccessKind.NONE, DefinitionKind.NONE);
				}

				m_client.recordQualifierLocation(symbolName, range);
			}

			if (node instanceof QualifiedName)
			{
				recordNodeAsQualifier(((QualifiedName)node).getQualifier());
			}
		}
	}

	private Range getRange(ASTNode node)
	{
		return Utility.getRange(node, m_compilationUnit);
	}
}
