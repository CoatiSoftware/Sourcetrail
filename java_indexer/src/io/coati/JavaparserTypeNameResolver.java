package io.coati;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.type.*;

import me.tomassetti.symbolsolver.javaparsermodel.JavaParserFacade;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserTypeParameter;
import me.tomassetti.symbolsolver.model.resolution.TypeParameter;
import me.tomassetti.symbolsolver.model.resolution.TypeSolver;
import me.tomassetti.symbolsolver.model.typesystem.TypeParameterUsage;
import me.tomassetti.symbolsolver.model.typesystem.TypeUsage;

public class JavaparserTypeNameResolver extends JavaNameResolver
{
	public JavaparserTypeNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver)
	{
		return getQualifiedTypeName(type, typeSolver, null);
	}
	
	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
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
				TypeUsage typeUsage = JavaParserFacade.get(m_typeSolver).convert(type, type); 
				
				return JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(typeUsage, m_typeSolver, m_ignoredContexts);
			}
			catch (Exception e)
			{
				// log...
			}
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
		else if (type instanceof ReferenceType)
		{
			ReferenceType referenceType = (ReferenceType)type;
			
			boolean isArray = (referenceType.getArrayCount() == 0); // TODO: regard array info!
			return getQualifiedTypeName(referenceType.getType());
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
		return new JavaTypeName("unresolved-type", null);
	}
	
	
}
