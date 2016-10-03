package io.coati;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.IntersectionType;
import com.github.javaparser.ast.type.PrimitiveType;
import com.github.javaparser.ast.type.ReferenceType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.UnionType;
import com.github.javaparser.ast.type.UnknownType;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.ast.type.WildcardType;

import me.tomassetti.symbolsolver.javaparsermodel.JavaParserFacade;
import me.tomassetti.symbolsolver.javaparsermodel.LambdaArgumentTypeUsagePlaceholder;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserClassDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserInterfaceDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserTypeParameter;
import me.tomassetti.symbolsolver.model.declarations.Declaration;
import me.tomassetti.symbolsolver.model.declarations.MethodDeclaration;
import me.tomassetti.symbolsolver.model.declarations.TypeDeclaration;
import me.tomassetti.symbolsolver.model.resolution.TypeParameter;
import me.tomassetti.symbolsolver.model.resolution.TypeSolver;
import me.tomassetti.symbolsolver.model.typesystem.ArrayTypeUsage;
import me.tomassetti.symbolsolver.model.typesystem.NullTypeUsage;
import me.tomassetti.symbolsolver.model.typesystem.PrimitiveTypeUsage;
import me.tomassetti.symbolsolver.model.typesystem.ReferenceTypeUsage;
import me.tomassetti.symbolsolver.model.typesystem.TypeParameterUsage;
import me.tomassetti.symbolsolver.model.typesystem.TypeUsage;
import me.tomassetti.symbolsolver.model.typesystem.VoidTypeUsage;
import me.tomassetti.symbolsolver.model.typesystem.WildcardUsage;

public class JavaSymbolSolverTypeNameResolver extends JavaNameResolver
{
	public JavaSymbolSolverTypeNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaTypeName getQualifiedTypeName(TypeUsage typeUsage, TypeSolver typeSolver)
	{
		return getQualifiedTypeName(typeUsage, typeSolver, null);
	}
	
	public static JavaTypeName getQualifiedTypeName(TypeUsage typeUsage, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		JavaSymbolSolverTypeNameResolver resolver = new JavaSymbolSolverTypeNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedTypeName(typeUsage);
	}
	
	public JavaTypeName getQualifiedTypeName(TypeUsage typeUsage)
	{
		if (typeUsage instanceof ArrayTypeUsage)
		{
			return getQualifiedTypeName(((ArrayTypeUsage)typeUsage).getComponentType());
		}
		else if (typeUsage instanceof LambdaArgumentTypeUsagePlaceholder)
		{
			
		}
		else if (typeUsage instanceof NullTypeUsage)
		{
			return JavaTypeName.fromDotSeparatedString(typeUsage.describe());
		}
		else if (typeUsage instanceof PrimitiveTypeUsage)
		{
			return JavaTypeName.fromDotSeparatedString(typeUsage.describe());
		}
		else if (typeUsage instanceof ReferenceTypeUsage)
		{
			ReferenceTypeUsage refTypeUsage = (ReferenceTypeUsage)typeUsage;
			
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
		else if (typeUsage instanceof TypeParameterUsage)
		{
			TypeParameter typeParam = ((TypeParameterUsage)typeUsage).asTypeParameter();
			if (typeParam instanceof JavaParserTypeParameter)
			{
				com.github.javaparser.ast.TypeParameter jpTypeParameter = ((JavaParserTypeParameter)typeParam).getWrappedNode();
				Node genericDecl = jpTypeParameter.getParentNode();
				if (genericDecl instanceof BodyDeclaration)
				{ 
					JavaDeclName genericName = null;
					if (!ignoresContext((BodyDeclaration)genericDecl))
					{
						genericName = JavaparserDeclNameResolver.getQualifiedDeclName((BodyDeclaration)genericDecl, m_typeSolver, m_ignoredContexts);
					}
					return new JavaTypeName(jpTypeParameter.getName(), genericName);
				}
			}
			else
			{
				// do we need to handle using type parameters of external code? YES! so: TODO: do this!
			}	
		}
		else if (typeUsage instanceof VoidTypeUsage)
		{
			return JavaTypeName.fromDotSeparatedString(typeUsage.describe());
		}
		else if (typeUsage instanceof WildcardUsage)
		{
			// TODO: implement this one
		}
		
		System.out.println("Unable to resolve qualified name of " + typeUsage.getClass().toString() + ": " + typeUsage.toString());
		return new JavaTypeName("unresolved-type", null);
	}
}
