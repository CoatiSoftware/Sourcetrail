package com.sourcetrail;

import java.util.List;

import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.type.*;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFacade;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

import com.sourcetrail.name.JavaTypeName;

public class JavaparserTypeNameResolver extends JavaNameResolver
{
	public JavaparserTypeNameResolver(TypeSolver typeSolver, ContextList ignoredContexts)
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver)
	{
		return getQualifiedTypeName(type, typeSolver, null);
	}
	
	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaparserTypeNameResolver resolver = new JavaparserTypeNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedTypeName(type);
	}
	
	public JavaTypeName getQualifiedTypeName(Type type)
	{
		String fallbackTypeName = type.toString();

		if (type instanceof ClassOrInterfaceType)
		{
			try
			{
				return JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(
					JavaParserFacade.get(m_typeSolver).convert(type, type), 
					m_typeSolver, 
					m_ignoredContexts
				);
			}
			catch (Exception e)
			{
				// log...
			}
		}
		else if (type instanceof ArrayType)
		{
			ArrayType arrayType = (ArrayType)type;
			
			// TODO: regard array info!
			return getQualifiedTypeName(arrayType.getComponentType());
		}
		else if (type instanceof TypeParameter)
		{
			return JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(
				JavaParserFacade.get(m_typeSolver).convert(type, type), 
				m_typeSolver, 
				m_ignoredContexts
			);
		}
		else if (type instanceof IntersectionType)
		{
		//	System.out.println("  IntersectionType: " + fallbackTypeName);

			return JavaTypeName.fromDotSeparatedString(type.toString());
		}
		else if (type instanceof PrimitiveType)
		{
			return JavaTypeName.fromDotSeparatedString(type.toString());
		}
		else if (type instanceof UnionType)
		{
	//		System.out.println("  UnionType: " + fallbackTypeName);
			return JavaTypeName.fromDotSeparatedString(type.toString());
		}
		else if (type instanceof UnknownType)
		{
	//		System.out.println("  UnknownType: " + fallbackTypeName);
			return JavaTypeName.fromDotSeparatedString(type.toString());
		}
		else if (type instanceof VoidType)
		{
			return JavaTypeName.fromDotSeparatedString(type.toString());
		}
		else if (type instanceof WildcardType)
		{
	//		System.out.println("  WildcardType: " + fallbackTypeName);
			return JavaTypeName.fromDotSeparatedString(type.toString());
		}
		
		System.out.println("Unable to resolve qualified name of " + type.getClass().toString() + ": " + fallbackTypeName);

		return new JavaTypeName("unsolved-jp-type", null); // JavaTypeName.unsolved(); 
	}
	
	
}
