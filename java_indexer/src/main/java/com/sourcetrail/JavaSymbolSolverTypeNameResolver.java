package com.sourcetrail;

import java.util.ArrayList;
import java.util.Optional;

import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.body.BodyDeclaration;

import com.github.javaparser.symbolsolver.javaparsermodel.LambdaArgumentTypePlaceholder;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserTypeParameter;
import com.github.javaparser.symbolsolver.logic.InferenceVariableType;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.typesystem.ArrayType;
import com.github.javaparser.symbolsolver.model.typesystem.NullType;
import com.github.javaparser.symbolsolver.model.typesystem.PrimitiveType;
import com.github.javaparser.symbolsolver.model.typesystem.ReferenceType;
import com.github.javaparser.symbolsolver.model.typesystem.Type;
import com.github.javaparser.symbolsolver.model.typesystem.TypeVariable;
import com.github.javaparser.symbolsolver.model.typesystem.VoidType;
import com.github.javaparser.symbolsolver.model.typesystem.Wildcard;

public class JavaSymbolSolverTypeNameResolver extends JavaNameResolver
{
	public JavaSymbolSolverTypeNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver)
	{
		return getQualifiedTypeName(type, typeSolver, null);
	}
	
	public static JavaTypeName getQualifiedTypeName(Type type, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		JavaSymbolSolverTypeNameResolver resolver = new JavaSymbolSolverTypeNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedTypeName(type);
	}
	
	public JavaTypeName getQualifiedTypeName(Type type)
	{
		if (type instanceof ArrayType)
		{
			return getQualifiedTypeName(((ArrayType)type).getComponentType());
		}
		else if (type instanceof LambdaArgumentTypePlaceholder)
		{
			
		}
		else if (type instanceof InferenceVariableType)
		{
			
		}
		else if (type instanceof NullType)
		{
			return JavaTypeName.fromDotSeparatedString(type.describe());
		}
		else if (type instanceof PrimitiveType)
		{
			return JavaTypeName.fromDotSeparatedString(type.describe());
		}
		else if (type instanceof ReferenceType)
		{
			ReferenceType refTypeUsage = (ReferenceType)type;
			
			JavaDeclName declName = JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(
				refTypeUsage.getTypeDeclaration(), 
				m_typeSolver, 
				m_ignoredContexts
			);
			
			return new JavaTypeName(declName.getName() + declName.getTypeParameterString(), new ArrayList<JavaTypeName>(), declName.getParent());
			/*
			if (declName != null)
			{
				List<JavaTypeName> typeArgumentNames = new ArrayList<>();
				
				for (TypeUsage typeArgument: refTypeUsage.parameters()) // don't know why this method is called "parameters()"
				{
					typeArgumentNames.add(getQualifiedTypeName(typeArgument, m_typeSolver, m_ignoredContexts));
				}
				return new JavaTypeName(declName.getName(), typeArgumentNames, declName.getParent());
			}
			*/
		}
		else if (type instanceof TypeVariable)
		{
			TypeParameterDeclaration typeParam = ((TypeVariable)type).asTypeParameter();
			if (typeParam instanceof JavaParserTypeParameter)
			{
				com.github.javaparser.ast.type.TypeParameter jpTypeParameter = ((JavaParserTypeParameter)typeParam).getWrappedNode();
				Optional<BodyDeclaration> genericDecl = jpTypeParameter.getAncestorOfType(BodyDeclaration.class);
				if (genericDecl.isPresent())
				{ 
					JavaDeclName genericName = null;
					if (!ignoresContext(genericDecl.get()))
					{
						genericName = JavaparserDeclNameResolver.getQualifiedDeclName(genericDecl.get(), m_typeSolver, m_ignoredContexts);
					}
					return new JavaTypeName(jpTypeParameter.getName().getId(), genericName);
				}
			}
			else
			{
				// do we need to handle using type parameters of external code? YES! so: TODO: do this!
			}	
		}
		else if (type instanceof VoidType)
		{
			return JavaTypeName.fromDotSeparatedString(type.describe());
		}
		else if (type instanceof Wildcard)
		{
			return new JavaTypeName("?", null);
		}
		
		System.out.println("Unable to resolve qualified name of " + type.getClass().toString() + ": " + type.toString());
		return new JavaTypeName("unresolved-type", null);
	}
}
