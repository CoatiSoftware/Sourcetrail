package com.sourcetrail.name.resolver;

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

import com.sourcetrail.ContextList;
import com.sourcetrail.Position;
import com.sourcetrail.Utility;
import com.sourcetrail.name.JavaDeclName;
import com.sourcetrail.name.JavaFunctionDeclName;
import com.sourcetrail.name.JavaTypeName;
import com.sourcetrail.name.JavaVariableDeclName;

public class DeclNameResolver extends NameResolver
{	
	public DeclNameResolver(File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts) 
	{
		super(currentFile, compilationUnit, ignoredContexts);
	}

	public static JavaDeclName getQualifiedDeclName(VariableDeclarationFragment decl, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedDeclName(decl, currentFile, compilationUnit, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(VariableDeclarationFragment decl, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		DeclNameResolver resolver = new DeclNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(VariableDeclarationFragment decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();

		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				JavaDeclName parentDeclName = getQualifiedContextName(decl);
				if (parentDeclName != null)
				{
					if (!parentDeclName.getIsUnsolved())
					{
						declName.setParent(parentDeclName);
					}
					else
					{
						declName = JavaDeclName.unsolved();
					}
				}
			}
		}
		return declName;
	}
	
	public JavaDeclName getDeclName(VariableDeclarationFragment decl)
	{
		JavaTypeName typeName = JavaTypeName.unsolved();
		
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
			typeName = BindingNameResolver.getQualifiedName(
					fieldDeclaration.get().getType().resolveBinding(),
					m_currentFile, 
					m_compilationUnit, 
					m_ignoredContexts.copy()).orElse(JavaTypeName.unsolved());
		}
		
		return new JavaVariableDeclName(decl.getName().getIdentifier(), typeName, isStatic);
	}
	
	public static JavaDeclName getQualifiedDeclName(BodyDeclaration decl, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedDeclName(decl, currentFile, compilationUnit, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(BodyDeclaration decl, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		DeclNameResolver resolver = new DeclNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(BodyDeclaration decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();
		
		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				JavaDeclName parentDeclName = getQualifiedContextName(decl);
				if (parentDeclName != null)
				{
					if (!parentDeclName.getIsUnsolved())
					{
						declName.setParent(parentDeclName);
					}
					else
					{
						declName = JavaDeclName.unsolved();
					}
				}
			}
		}
		
		return declName;
	}	
	
	public JavaDeclName getDeclName(BodyDeclaration decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();
		
		if (decl != null)
		{
			if (decl instanceof AnnotationTypeDeclaration)
			{
				declName = new JavaDeclName("AnnotationTypeDeclaration");
			}
			else if (decl instanceof EnumDeclaration)
			{
				declName = new JavaDeclName(((EnumDeclaration) decl).getName().getIdentifier());
			}
			else if (decl instanceof TypeDeclaration)
			{
				TypeDeclaration typeDeclaration = (TypeDeclaration) decl;

				List<String> typeParameterNames = new ArrayList<>();
				for (Object typeParameter: typeDeclaration.typeParameters())
				{
					if (typeParameter instanceof TypeParameter)
					{
						typeParameterNames.add(((TypeParameter) typeParameter).getName().getIdentifier());
					}
				}
				
				declName = new JavaDeclName(typeDeclaration.getName().getIdentifier(), typeParameterNames);
			}
			else if (decl instanceof AnnotationTypeMemberDeclaration)
			{
				declName = new JavaDeclName("AnnotationTypeMemberDeclaration");
			}
			else if (decl instanceof EnumConstantDeclaration)
			{
				declName = new JavaDeclName(((EnumConstantDeclaration) decl).getName().getIdentifier());
			}
			else if (decl instanceof FieldDeclaration)
			{
				declName = new JavaDeclName("FieldDeclaration");
			}
			else if (decl instanceof Initializer)
			{
				declName = new JavaDeclName("Initializer");
			}
			else if (decl instanceof MethodDeclaration)
			{
				MethodDeclaration methodDeclaration = (MethodDeclaration) decl;
				
				List<String> typeParameterNames = new ArrayList<>();
				for (Object typeParameter: methodDeclaration.typeParameters())
				{
					if (typeParameter instanceof TypeParameter)
					{
						typeParameterNames.add(((TypeParameter) typeParameter).getName().getIdentifier());
					}
				}
				
				ContextList ignoredContexts = m_ignoredContexts.copy();
				ignoredContexts.add(((MethodDeclaration) decl).resolveBinding());
				
				JavaTypeName returnTypeName = methodDeclaration.isConstructor() ? null : BindingNameResolver.getQualifiedName(
						methodDeclaration.getReturnType2().resolveBinding(), m_currentFile, m_compilationUnit, ignoredContexts).orElse(JavaTypeName.unsolved());
				
				List<JavaTypeName> parameterTypeNames =  new ArrayList<>();
				for (Object parameter: methodDeclaration.parameters())
				{
					if (parameter instanceof SingleVariableDeclaration)
					{
						parameterTypeNames.add(BindingNameResolver.getQualifiedName(
								((SingleVariableDeclaration) parameter).getType().resolveBinding(), m_currentFile, m_compilationUnit, ignoredContexts).orElse(JavaTypeName.unsolved()));
					}
				}
				
				declName = new JavaFunctionDeclName(
						methodDeclaration.getName().getIdentifier(), 
						typeParameterNames, 
						returnTypeName, 
						parameterTypeNames, 
						Modifier.isStatic(methodDeclaration.getModifiers()));
			}
		}
		
		return declName;
	}
	
	public static JavaDeclName getQualifiedDeclName(AnonymousClassDeclaration decl, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedDeclName(decl, currentFile, compilationUnit, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(AnonymousClassDeclaration decl, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		DeclNameResolver resolver = new DeclNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(AnonymousClassDeclaration decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();

		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				JavaDeclName parentDeclName = getQualifiedContextName(decl);
				if (parentDeclName != null)
				{
					if (!parentDeclName.getIsUnsolved())
					{
						declName.setParent(parentDeclName);
					}
					else
					{
						declName = JavaDeclName.unsolved();
					}
				}
			}
		}
		return declName;
	}
	
	public JavaDeclName getDeclName(AnonymousClassDeclaration decl)
	{
		Position pos = Utility.getRange(decl, m_compilationUnit).begin;
		return JavaDeclName.anonymousClass(m_currentFile, pos.line, pos.column);
	}
	
	
	public static JavaDeclName getQualifiedName(Name name)
	{
		if (name.isSimpleName())
		{
			return new JavaDeclName(((SimpleName) name).getIdentifier());
		}
		else
		{
			JavaDeclName declName = new JavaDeclName(((QualifiedName) name).getName().getIdentifier());
			declName.setParent(getQualifiedName(((QualifiedName) name).getQualifier()));
			return declName;
		}
	}
	
	private JavaDeclName getQualifiedContextName(ASTNode decl)
	{
		ASTNode parentNode = decl.getParent();
		if (parentNode == null)
		{
			return null;
		}
		
		if (parentNode instanceof BodyDeclaration && !(parentNode instanceof FieldDeclaration))
		{
			return getQualifiedDeclName((BodyDeclaration) parentNode);
		}
		else if (parentNode instanceof AnonymousClassDeclaration)
		{
			return getQualifiedDeclName((AnonymousClassDeclaration) parentNode);
		}
		else if (parentNode instanceof CompilationUnit)
		{
			PackageDeclaration packageDecl = ((CompilationUnit) parentNode).getPackage();
			if (packageDecl != null)
			{
				return getQualifiedName(packageDecl.getName());
			}
			return null;
		}
		
		return getQualifiedContextName(parentNode);
	}
}
