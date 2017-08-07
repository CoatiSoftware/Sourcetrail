package com.sourcetrail;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.type.TypeParameter;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserAnnotationDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserAnonymousClassDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserClassDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserConstructorDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserEnumConstantDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserEnumDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserFieldDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserInterfaceDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserParameterDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserSymbolDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserTypeParameter;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserTypeVariableDeclaration;
import com.github.javaparser.symbolsolver.javassistmodel.JavassistClassDeclaration;
import com.github.javaparser.symbolsolver.javassistmodel.JavassistTypeParameter;
import com.github.javaparser.symbolsolver.model.declarations.Declaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodLikeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParametrizable;
import com.github.javaparser.symbolsolver.model.declarations.ValueDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

import com.sourcetrail.name.JavaDeclName;
import com.sourcetrail.name.JavaFunctionDeclName;
import com.sourcetrail.name.JavaTypeName;
import com.sourcetrail.name.JavaVariableDeclName;

public class JavaSymbolSolverDeclNameResolver extends JavaNameResolver
{
	public JavaSymbolSolverDeclNameResolver(TypeSolver typeSolver, ContextList ignoredContexts) 
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaDeclName getQualifiedDeclName(Declaration decl, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(Declaration decl, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaSymbolSolverDeclNameResolver resolver = new JavaSymbolSolverDeclNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(Declaration decl)
	{
		JavaDeclName declName = null;
		
		if (decl instanceof JavaParserAnnotationDeclaration)
        {
			// TODO: implement
			System.out.println("solving name of JavaParserAnnotationDeclaration not implemented");
			declName = JavaDeclName.unsolved();
			
        } 
		if (decl instanceof JavaParserAnonymousClassDeclaration)
        {
			// TODO: implement
			System.out.println("solving name of JavaParserAnonymousClassDeclaration not implemented");
			declName = JavaDeclName.unsolved();
        } 
		else if (decl instanceof JavaParserClassDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserClassDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserConstructorDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserConstructorDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserEnumDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserEnumDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserEnumConstantDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserEnumConstantDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserFieldDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserFieldDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserInterfaceDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserInterfaceDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserMethodDeclaration)
        {
            declName = JavaparserDeclNameResolver.getQualifiedDeclName(
            		((JavaParserMethodDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserParameterDeclaration)
        {
			System.out.println("solving name of JavaParserParameterDeclaration not implemented");
			declName = JavaDeclName.unsolved();
        }
        else if (decl instanceof JavaParserSymbolDeclaration)
        {
			System.out.println("solving name of JavaParserSymbolDeclaration not implemented");
			declName = JavaDeclName.unsolved();
        }
        else if (decl instanceof JavaParserTypeParameter)
        {
        	declName = JavaparserDeclNameResolver.getQualifiedDeclName(
        			((JavaParserTypeParameter)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else if (decl instanceof JavaParserTypeVariableDeclaration)
        {
        	declName = JavaparserDeclNameResolver.getQualifiedDeclName(
        			((JavaParserTypeVariableDeclaration)decl).getWrappedNode(), m_typeSolver, m_ignoredContexts);
        }
        else
		{
        	String name = decl.getName();
	      	List<String> typeParameters = new ArrayList<>();
        	if (decl instanceof TypeParametrizable)
        	{
        		typeParameters = getTypeParameterNames((TypeParametrizable) decl);
        	}
			
			if (decl instanceof ReferenceTypeDeclaration)
			{
				declName = new JavaDeclName(name, typeParameters);
				
				Optional<ReferenceTypeDeclaration> containerType = ((ReferenceTypeDeclaration) decl).containerType();
				if (containerType.isPresent())
				{
					declName.setParent(getQualifiedDeclName(containerType.get(), m_typeSolver, m_ignoredContexts));
				}
				else
				{
					declName.setParent(JavaDeclName.fromDotSeparatedString(((ReferenceTypeDeclaration) decl).getPackageName()));
				}
				
			}
			else if (decl instanceof TypeParameterDeclaration)
			{
				declName = new JavaDeclName(name);
				TypeParametrizable container = ((TypeParameterDeclaration) decl).getContainer();
				if (!m_ignoredContexts.contains(container))
				{
					if (container instanceof ReferenceTypeDeclaration)
					{
						declName.setParent(getQualifiedDeclName((ReferenceTypeDeclaration) container, m_typeSolver, m_ignoredContexts));
					}
					if (container instanceof MethodLikeDeclaration)
					{
						declName.setParent(getQualifiedDeclName((MethodLikeDeclaration) container, m_typeSolver, m_ignoredContexts));
					}
				}
			}
			else if (decl instanceof MethodLikeDeclaration)
			{
				ContextList ignoredContextsForTypes = m_ignoredContexts.copy();
		        ignoredContextsForTypes.add((MethodLikeDeclaration) decl); // adding own decl
		        
				JavaTypeName returnTypeName = new JavaTypeName("", null);
				boolean isStatic = false;
				if (decl instanceof MethodDeclaration)
				{
					returnTypeName = JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(((MethodDeclaration)decl).getReturnType(), m_typeSolver, ignoredContextsForTypes);
					isStatic = ((MethodDeclaration)decl).isStatic();
				}
				
				declName = new JavaFunctionDeclName(
						name, typeParameters, 
						returnTypeName, 
						getParameterTypeNames((MethodLikeDeclaration) decl, m_typeSolver, ignoredContextsForTypes), 
						isStatic);
				
				declName.setParent(
						getQualifiedDeclName(((MethodLikeDeclaration) decl).declaringType(), m_typeSolver, m_ignoredContexts));
				
			}
			else if (decl instanceof com.github.javaparser.symbolsolver.model.declarations.FieldDeclaration)
			{
				com.github.javaparser.symbolsolver.model.declarations.FieldDeclaration fieldDecl = ((com.github.javaparser.symbolsolver.model.declarations.FieldDeclaration) decl);
				declName = new JavaVariableDeclName(
						name, 
						JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(fieldDecl.getType(), m_typeSolver, m_ignoredContexts), 
						fieldDecl.isStatic());
				
				declName.setParent(
						getQualifiedDeclName(
								fieldDecl.declaringType(), 
								m_typeSolver, 
								m_ignoredContexts));
			}
		}
		
		if (declName == null)
		{
			System.out.println("Unable to resolve qualified declaration name of " + decl.getClass().toString() + ": " + decl.toString());
			declName = JavaDeclName.unsolved();
		}
		
		return declName;
	}
	
	private static List<String> getTypeParameterNames(TypeParametrizable decl)
	{
		List<String> typeParameterNames = new ArrayList<>();
		List<TypeParameterDeclaration> typeParameters = decl.getTypeParameters();
		if (typeParameters != null && typeParameters.size() > 0)
		{
			for (int i = 0; i < typeParameters.size(); i++)
			{
				typeParameterNames.add(typeParameters.get(i).getName());
			}
		}
		return typeParameterNames;
	}
	
	private static List<JavaTypeName> getParameterTypeNames(MethodLikeDeclaration decl, TypeSolver typeSolver, ContextList ignoredContexts)
	{		
		List<JavaTypeName> typeTypeParameterNames = new ArrayList<>();
		for (int i = 0; i < decl.getNumberOfParams(); i++)
		{
			typeTypeParameterNames.add(JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(
				decl.getParam(i).getType(), typeSolver, ignoredContexts
			));
		}
		return typeTypeParameterNames;
	}
	
}
