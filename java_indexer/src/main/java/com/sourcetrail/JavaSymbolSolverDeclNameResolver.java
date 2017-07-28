package com.sourcetrail;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;

import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserClassDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserInterfaceDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.Declaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

import com.sourcetrail.name.JavaDeclName;
import com.sourcetrail.name.JavaFunctionDeclName;
import com.sourcetrail.name.JavaTypeName;

public class JavaSymbolSolverDeclNameResolver extends JavaNameResolver
{
	public JavaSymbolSolverDeclNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts) 
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaDeclName getQualifiedDeclName(Declaration decl, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(Declaration decl, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		JavaSymbolSolverDeclNameResolver resolver = new JavaSymbolSolverDeclNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(Declaration decl)
	{
		JavaDeclName declName = null;
		
		if (decl != null)
		{
			if (decl instanceof TypeDeclaration)
			{
				TypeDeclaration typeDecl = (TypeDeclaration)decl;
				
				ClassOrInterfaceDeclaration javaparserDecl = null;
				if (typeDecl instanceof JavaParserClassDeclaration)
				{
					javaparserDecl = ((JavaParserClassDeclaration)typeDecl).getWrappedNode();
				}
				else if (typeDecl instanceof JavaParserInterfaceDeclaration)
				{
					javaparserDecl = ((JavaParserInterfaceDeclaration)typeDecl).getWrappedNode();
				}
				
				if (javaparserDecl != null)
				{
					declName = JavaparserDeclNameResolver.getQualifiedDeclName(javaparserDecl, m_typeSolver, m_ignoredContexts);
				}
				else
				{
					declName = JavaDeclName.fromDotSeparatedString(typeDecl.getQualifiedName());
				}
			}
			else if (decl instanceof MethodDeclaration)
			{
				MethodDeclaration methodDecl = (MethodDeclaration)decl;
				
				if (methodDecl instanceof JavaParserMethodDeclaration)
				{
					declName = JavaparserDeclNameResolver.getQualifiedDeclName(
						((JavaParserMethodDeclaration)methodDecl).getWrappedNode(),
						m_typeSolver, 
						m_ignoredContexts
					);
				}
				else
				{
					// TODO: what about endless recursion regarding type parameters and return type??
					
					List<JavaTypeName> parameterNames = new ArrayList<>();
					for (int i = 0; i < methodDecl.getNumberOfParams(); i++)
					{
						parameterNames.add(JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(
							methodDecl.getParam(i).getType(), m_typeSolver, m_ignoredContexts
						));
					}
					
					declName = new JavaFunctionDeclName(
						methodDecl.getName(), 
						getTypeParameterNames(methodDecl.getTypeParameters()), 
						JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(methodDecl.getReturnType(), m_typeSolver, m_ignoredContexts),
						parameterNames,
						methodDecl.isStatic()
					);
							
					declName.setParent(
						getQualifiedDeclName(methodDecl.declaringType(), m_typeSolver, m_ignoredContexts)
					);
				}
				
			}
		}
		return declName;
	}
	
	private static List<String> getTypeParameterNames(List<TypeParameterDeclaration> typeParameters)
	{
		List<String> typeParameterNames = new ArrayList<>();
		if (typeParameters != null && typeParameters.size() > 0)
		{
			for (int i = 0; i < typeParameters.size(); i++)
			{
				typeParameterNames.add(typeParameters.get(i).getName());
			}
		}
		return typeParameterNames;
	}
	
}
