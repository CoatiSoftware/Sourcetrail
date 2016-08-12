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

public class JavaTypeNameResolver extends JavaNameResolver
{
	public JavaTypeNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver)
	{
		return getQualifiedTypeName(type, typeSolver, null);
	}
	
	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		JavaTypeNameResolver resolver = new JavaTypeNameResolver(typeSolver, ignoredContexts);
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
				if (typeUsage.isReferenceType())
				{
					JavaDeclName declName = JavaDeclNameResolver.getQualifiedDeclName(typeUsage.asReferenceTypeUsage().getTypeDeclaration(), m_typeSolver, m_ignoredContexts);
					
					if (declName != null)
					{
						List<JavaTypeName> typeArgumentNames = new ArrayList<>();
						for (Type typeArgument: ((ClassOrInterfaceType)type).getTypeArgs())
						{
							typeArgumentNames.add(getQualifiedTypeName(typeArgument, m_typeSolver, m_ignoredContexts));
						}
						JavaTypeName ret = new JavaTypeName(declName.getName(), typeArgumentNames, declName.getParent());
						
						return ret;
					}
				}
				else if (typeUsage instanceof TypeParameterUsage)
				{
					TypeParameter typeParam = typeUsage.asTypeParameter();
					if (typeParam instanceof JavaParserTypeParameter)
					{
						com.github.javaparser.ast.TypeParameter jpTypeParameter = ((JavaParserTypeParameter)typeParam).getWrappedNode();
						Node genericDecl = jpTypeParameter.getParentNode();
						if (genericDecl instanceof BodyDeclaration)
						{ 
							JavaDeclName genericName = null;
							if (!ignoresContext((BodyDeclaration)genericDecl))
							{
								genericName = JavaDeclNameResolver.getQualifiedDeclName((BodyDeclaration)genericDecl, m_typeSolver, m_ignoredContexts);
							}
							return new JavaTypeName(jpTypeParameter.getName(), genericName);
						}
					}
					else
					{
						// do we need to handle using type parameters of external code?
					}
				}
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
