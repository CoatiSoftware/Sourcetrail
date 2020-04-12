package com.sourcetrail.name.resolver;

import com.sourcetrail.ContextList;
import com.sourcetrail.Position;
import com.sourcetrail.Utility;
import com.sourcetrail.name.DeclName;
import com.sourcetrail.name.FunctionDeclName;
import com.sourcetrail.name.TypeName;
import com.sourcetrail.name.VariableDeclName;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.AnnotationTypeDeclaration;
import org.eclipse.jdt.core.dom.AnnotationTypeMemberDeclaration;
import org.eclipse.jdt.core.dom.AnonymousClassDeclaration;
import org.eclipse.jdt.core.dom.BodyDeclaration;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.EnumConstantDeclaration;
import org.eclipse.jdt.core.dom.EnumDeclaration;
import org.eclipse.jdt.core.dom.FieldDeclaration;
import org.eclipse.jdt.core.dom.IVariableBinding;
import org.eclipse.jdt.core.dom.Initializer;
import org.eclipse.jdt.core.dom.MethodDeclaration;
import org.eclipse.jdt.core.dom.Modifier;
import org.eclipse.jdt.core.dom.Name;
import org.eclipse.jdt.core.dom.PackageDeclaration;
import org.eclipse.jdt.core.dom.QualifiedName;
import org.eclipse.jdt.core.dom.SimpleName;
import org.eclipse.jdt.core.dom.SingleVariableDeclaration;
import org.eclipse.jdt.core.dom.TypeDeclaration;
import org.eclipse.jdt.core.dom.TypeParameter;
import org.eclipse.jdt.core.dom.VariableDeclarationFragment;

public class DeclNameResolver extends NameResolver
{
	public DeclNameResolver(File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		super(currentFile, compilationUnit, ignoredContexts);
	}

	public static DeclName getQualifiedDeclName(
		VariableDeclarationFragment decl, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedDeclName(decl, currentFile, compilationUnit, null);
	}

	public static DeclName getQualifiedDeclName(
		VariableDeclarationFragment decl,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		DeclNameResolver resolver = new DeclNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}

	public DeclName getQualifiedDeclName(VariableDeclarationFragment decl)
	{
		DeclName declName = DeclName.unsolved();

		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				DeclName parentDeclName = getQualifiedContextName(decl);
				if (parentDeclName != null)
				{
					if (!parentDeclName.getIsUnsolved())
					{
						declName.setParent(parentDeclName);
					}
					else
					{
						declName = DeclName.unsolved();
					}
				}
			}
		}
		return declName;
	}

	public DeclName getDeclName(VariableDeclarationFragment decl)
	{
		TypeName typeName = TypeName.unsolved();

		boolean isStatic = false;
		Optional<FieldDeclaration> fieldDeclaration = getAncestorOfType(decl, FieldDeclaration.class);
		if (fieldDeclaration.isPresent())
		{
			if (decl.resolveBinding() instanceof IVariableBinding)
			{
				isStatic = Modifier.isStatic(decl.resolveBinding().getModifiers());
			}
			else
			{
				isStatic = Modifier.isStatic(fieldDeclaration.get().getModifiers());
			}
			typeName = BindingNameResolver
						   .getQualifiedName(
							   fieldDeclaration.get().getType().resolveBinding(),
							   m_currentFile,
							   m_compilationUnit,
							   m_ignoredContexts.copy())
						   .orElse(TypeName.unsolved());
		}

		return new VariableDeclName(decl.getName().getIdentifier(), typeName, isStatic);
	}

	public static DeclName getQualifiedDeclName(
		BodyDeclaration decl, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedDeclName(decl, currentFile, compilationUnit, null);
	}

	public static DeclName getQualifiedDeclName(
		BodyDeclaration decl,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		DeclNameResolver resolver = new DeclNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}

	public DeclName getQualifiedDeclName(BodyDeclaration decl)
	{
		DeclName declName = DeclName.unsolved();

		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				DeclName parentDeclName = getQualifiedContextName(decl);
				if (parentDeclName != null)
				{
					if (!parentDeclName.getIsUnsolved())
					{
						declName.setParent(parentDeclName);
					}
					else
					{
						declName = DeclName.unsolved();
					}
				}
			}
		}

		return declName;
	}

	public DeclName getDeclName(BodyDeclaration decl)
	{
		DeclName declName = DeclName.unsolved();

		if (decl != null)
		{
			if (decl instanceof AnnotationTypeDeclaration)
			{
				declName = new DeclName(((AnnotationTypeDeclaration)decl).getName().getIdentifier());
			}
			else if (decl instanceof EnumDeclaration)
			{
				declName = new DeclName(((EnumDeclaration)decl).getName().getIdentifier());
			}
			else if (decl instanceof TypeDeclaration)
			{
				TypeDeclaration typeDeclaration = (TypeDeclaration)decl;

				List<String> typeParameterNames = new ArrayList<>();
				for (Object typeParameter: typeDeclaration.typeParameters())
				{
					if (typeParameter instanceof TypeParameter)
					{
						typeParameterNames.add(
							((TypeParameter)typeParameter).getName().getIdentifier());
					}
				}

				declName = new DeclName(
					typeDeclaration.getName().getIdentifier(), typeParameterNames);
			}
			else if (decl instanceof AnnotationTypeMemberDeclaration)
			{
				declName = new DeclName(
					((AnnotationTypeMemberDeclaration)decl).getName().getIdentifier());
			}
			else if (decl instanceof EnumConstantDeclaration)
			{
				declName = new DeclName(((EnumConstantDeclaration)decl).getName().getIdentifier());
			}
			else if (decl instanceof FieldDeclaration)
			{
				declName = new DeclName("FieldDeclaration");
			}
			else if (decl instanceof Initializer)
			{
				declName = new DeclName("Initializer");
			}
			else if (decl instanceof MethodDeclaration)
			{
				MethodDeclaration methodDeclaration = (MethodDeclaration)decl;

				List<String> typeParameterNames = new ArrayList<>();
				for (Object typeParameter: methodDeclaration.typeParameters())
				{
					if (typeParameter instanceof TypeParameter)
					{
						typeParameterNames.add(
							((TypeParameter)typeParameter).getName().getIdentifier());
					}
				}

				ContextList ignoredContexts = m_ignoredContexts.copy();
				ignoredContexts.add(((MethodDeclaration)decl).resolveBinding());

				TypeName returnTypeName = methodDeclaration.isConstructor()
					? null
					: BindingNameResolver
						  .getQualifiedName(
							  methodDeclaration.getReturnType2().resolveBinding(),
							  m_currentFile,
							  m_compilationUnit,
							  ignoredContexts)
						  .orElse(TypeName.unsolved());

				List<TypeName> parameterTypeNames = new ArrayList<>();
				for (Object parameter: methodDeclaration.parameters())
				{
					if (parameter instanceof SingleVariableDeclaration)
					{
						parameterTypeNames.add(
							BindingNameResolver
								.getQualifiedName(
									((SingleVariableDeclaration)parameter).getType().resolveBinding(),
									m_currentFile,
									m_compilationUnit,
									ignoredContexts)
								.orElse(TypeName.unsolved()));
					}
				}

				declName = new FunctionDeclName(
					methodDeclaration.getName().getIdentifier(),
					typeParameterNames,
					returnTypeName,
					parameterTypeNames,
					Modifier.isStatic(methodDeclaration.getModifiers()));
			}
		}

		return declName;
	}

	public static DeclName getQualifiedDeclName(
		AnonymousClassDeclaration decl, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedDeclName(decl, currentFile, compilationUnit, null);
	}

	public static DeclName getQualifiedDeclName(
		AnonymousClassDeclaration decl,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		DeclNameResolver resolver = new DeclNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}

	public DeclName getQualifiedDeclName(AnonymousClassDeclaration decl)
	{
		DeclName declName = DeclName.unsolved();

		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				DeclName parentDeclName = getQualifiedContextName(decl);
				if (parentDeclName != null)
				{
					if (!parentDeclName.getIsUnsolved())
					{
						declName.setParent(parentDeclName);
					}
					else
					{
						declName = DeclName.unsolved();
					}
				}
			}
		}
		return declName;
	}

	public DeclName getDeclName(AnonymousClassDeclaration decl)
	{
		Position pos = Utility.getRange(decl, m_compilationUnit).begin;
		return DeclName.anonymousClass(m_currentFile, pos.line, pos.column);
	}


	public static DeclName getQualifiedName(Name name)
	{
		if (name.isSimpleName())
		{
			return new DeclName(((SimpleName)name).getIdentifier());
		}
		else
		{
			DeclName declName = new DeclName(((QualifiedName)name).getName().getIdentifier());
			declName.setParent(getQualifiedName(((QualifiedName)name).getQualifier()));
			return declName;
		}
	}

	private DeclName getQualifiedContextName(ASTNode decl)
	{
		ASTNode parentNode = decl.getParent();
		if (parentNode == null)
		{
			return null;
		}

		if (parentNode instanceof BodyDeclaration && !(parentNode instanceof FieldDeclaration))
		{
			return getQualifiedDeclName((BodyDeclaration)parentNode);
		}
		else if (parentNode instanceof AnonymousClassDeclaration)
		{
			return getQualifiedDeclName((AnonymousClassDeclaration)parentNode);
		}
		else if (parentNode instanceof CompilationUnit)
		{
			PackageDeclaration packageDecl = ((CompilationUnit)parentNode).getPackage();
			if (packageDecl != null)
			{
				return getQualifiedName(packageDecl.getName());
			}
			return null;
		}

		return getQualifiedContextName(parentNode);
	}
}
