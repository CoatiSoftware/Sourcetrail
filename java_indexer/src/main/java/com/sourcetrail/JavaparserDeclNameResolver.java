package com.sourcetrail;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import com.github.javaparser.Position;
import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.NodeList;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.InitializerDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.TypeDeclaration;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.expr.Name;
import com.github.javaparser.ast.expr.ObjectCreationExpr;
import com.github.javaparser.ast.type.TypeParameter;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

import com.sourcetrail.name.JavaDeclName;
import com.sourcetrail.name.JavaFunctionDeclName;
import com.sourcetrail.name.JavaTypeName;
import com.sourcetrail.name.JavaVariableDeclName;

public class JavaparserDeclNameResolver extends JavaNameResolver
{	
	public JavaparserDeclNameResolver(File currentFile, TypeSolver typeSolver, ContextList ignoredContexts) 
	{
		super(currentFile, typeSolver, ignoredContexts);
	}

	public static JavaDeclName getQualifiedDeclName(VariableDeclarator decl, File currentFile, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, currentFile, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(VariableDeclarator decl, File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaparserDeclNameResolver resolver = new JavaparserDeclNameResolver(currentFile, typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(VariableDeclarator decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();

		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				declName.setParent(getQualifiedContextName(decl));
			}
		}
		return declName;
	}
	
	public JavaDeclName getDeclName(VariableDeclarator decl)
	{
		JavaTypeName typeName = JavaparserTypeNameResolver.getQualifiedTypeName(decl.getType(), m_currentFile, m_typeSolver, m_ignoredContexts.copy());
		
		boolean isStatic = false;
		Optional<FieldDeclaration> fieldDeclaration = decl.getAncestorOfType(FieldDeclaration.class);
		if (fieldDeclaration.isPresent())
		{
			isStatic = fieldDeclaration.get().isStatic();
		}
		
		return new JavaVariableDeclName(decl.getNameAsString(), typeName, isStatic);
	}
	
	public static JavaDeclName getQualifiedDeclName(BodyDeclaration decl, File currentFile, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, currentFile, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(BodyDeclaration decl, File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaparserDeclNameResolver resolver = new JavaparserDeclNameResolver(currentFile, typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(BodyDeclaration<?> decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();
		
		if (decl != null)
		{
			declName = getDeclName(decl);
			if (!declName.getIsUnsolved())
			{
				declName.setParent(getQualifiedContextName(decl));
			}
		}
		
		return declName;
	}	
	
	public JavaDeclName getDeclName(BodyDeclaration decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();
		
		if (decl != null)
		{
			if (decl instanceof AnnotationMemberDeclaration)
			{
				declName = new JavaDeclName("AnnotationMemberDeclaration");
			}
			else if (decl instanceof ConstructorDeclaration)
			{
				CallableConstructorDecl callableDecl = new CallableConstructorDecl((ConstructorDeclaration)decl);
				declName = getDeclNameOfCallable(callableDecl);
			}
			else if (decl instanceof EnumConstantDeclaration)
			{
				declName = new JavaDeclName(((EnumConstantDeclaration)decl).getNameAsString());
			}
			else if (decl instanceof FieldDeclaration)
			{
				throw new UnsupportedOperationException();
			}
			else if (decl instanceof InitializerDeclaration)
			{
				declName = new JavaDeclName("InitializerDeclaration");
			}
			else if (decl instanceof MethodDeclaration)
			{
				CallableMethodDecl callableDecl = new CallableMethodDecl((MethodDeclaration)decl);
				declName = getDeclNameOfCallable(callableDecl);
			}
			else if (decl instanceof TypeDeclaration)
			{
				declName = new JavaDeclName(((TypeDeclaration)decl).getNameAsString(), getTypeParameterNames((TypeDeclaration)decl));
			}
		}
		
		return declName;
	}
	
	public static JavaDeclName getQualifiedDeclName(TypeParameter decl, File currentFile, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, currentFile, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(TypeParameter decl, File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaparserDeclNameResolver resolver = new JavaparserDeclNameResolver(currentFile, typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(TypeParameter decl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();
		
		if (decl != null)
		{
			declName = getDeclName(decl);

			if (!declName.getIsUnsolved())
			{
				declName.setParent(getQualifiedContextName(decl));
			}
		}
		return declName;
	}
	
	public JavaDeclName getDeclName(TypeParameter decl)
	{
		return new JavaDeclName(decl.getName().asString());
	}
	
	public static JavaDeclName getQualifiedDeclName(ObjectCreationExpr anonymousClassDecl, File currentFile, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(anonymousClassDecl, currentFile, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(ObjectCreationExpr anonymousClassDecl, File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaparserDeclNameResolver resolver = new JavaparserDeclNameResolver(currentFile, typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(anonymousClassDecl);
	}
	
	public JavaDeclName getQualifiedDeclName(ObjectCreationExpr anonymousClassDecl)
	{
		JavaDeclName declName = JavaDeclName.unsolved();
		
		if (anonymousClassDecl != null)
		{
			declName = getDeclName(anonymousClassDecl);
			if (!declName.getIsUnsolved())
			{
				declName.setParent(getQualifiedContextName(anonymousClassDecl));
			}
		}
		return declName;
	}
	
	public JavaDeclName getDeclName(ObjectCreationExpr anonymousClassDecl)
	{
		if (anonymousClassDecl.getAnonymousClassBody().isPresent())
		{
			Position position = anonymousClassDecl.getBegin().orElse(new Position(0, 0));
			return JavaDeclName.anonymousClass(m_currentFile.getName(), position.line, position.column);
		}
		return JavaDeclName.unsolved();
	}
	
	private static List<String> getTypeParameterNames(TypeDeclaration decl)
	{
		NodeList<TypeParameter> typeParameters = null;
		if (decl instanceof ClassOrInterfaceDeclaration)
		{
			typeParameters = (NodeList<TypeParameter>) ((ClassOrInterfaceDeclaration)decl).getTypeParameters();
		}

		return getTypeParameterNames(typeParameters);
	}
	
	private static List<String> getTypeParameterNames(NodeList<TypeParameter> typeParameters)
	{
		List<String> typeParameterNames = new ArrayList<>();
		if (typeParameters != null && typeParameters.size() > 0)
		{
			for (int i = 0; i < typeParameters.size(); i++)
			{
				typeParameterNames.add(typeParameters.get(i).getNameAsString());
			}
		}
		return typeParameterNames;
	}
	
	public static JavaDeclName getQualifiedName(Name name)
	{
		JavaDeclName declName = new JavaDeclName(name.getId());
		if (name.getQualifier().isPresent())
		{
			declName.setParent(getQualifiedName(name.getQualifier().get()));
		}
		return declName;
	}
	
	private JavaDeclName getQualifiedContextName(Node decl)
	{
		Node parentNode = decl.getParentNode().orElse(null);
		if (parentNode == null)
		{
			return null;
		}
		
		if (parentNode instanceof BodyDeclaration && !(parentNode instanceof FieldDeclaration))
		{
			if (m_ignoredContexts.contains((BodyDeclaration) parentNode))
			{
				return null;
			}
			return getQualifiedDeclName((BodyDeclaration) parentNode);
		}
		else if (parentNode instanceof CompilationUnit)
		{
			Optional<PackageDeclaration> packageDecl = ((CompilationUnit) parentNode).getPackageDeclaration();
			if (packageDecl.isPresent())
			{
				return getQualifiedName(packageDecl.get().getName());
			}
			return null;
		}
		else if (parentNode instanceof ObjectCreationExpr && ((ObjectCreationExpr) parentNode).getAnonymousClassBody().isPresent())
		{
			return getQualifiedDeclName((ObjectCreationExpr) parentNode);
		}
		
		return getQualifiedContextName(parentNode);
	}
	
	private <T extends CallableDecl> JavaDeclName getDeclNameOfCallable(T decl)
	{
		ContextList ignoredContextsForTypes = m_ignoredContexts.copy();
		ignoredContextsForTypes.add(decl.getWrappedNode()); // adding own decl
		
		List<JavaTypeName> parameterNames = new ArrayList<>();
		for (Parameter parameter: decl.getParameters())
		{
			parameterNames.add(JavaparserTypeNameResolver.getQualifiedTypeName(parameter.getType(), m_currentFile, m_typeSolver, ignoredContextsForTypes));
		}
		
		JavaTypeName returnType = new JavaTypeName("", null);		
		if (decl.isMethod())
		{
			returnType = JavaparserTypeNameResolver.getQualifiedTypeName(decl.getType(), m_currentFile, m_typeSolver, ignoredContextsForTypes);
		}
		
		return new JavaFunctionDeclName(
			decl.getName(), 
			getTypeParameterNames(decl.getTypeParameters()), 
			returnType, 
			parameterNames, 
			decl.isStatic()
		);
	}
}
