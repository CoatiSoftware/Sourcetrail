package io.coati;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.Parameter;

import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserClassDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserInterfaceDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import me.tomassetti.symbolsolver.model.declarations.Declaration;
import me.tomassetti.symbolsolver.model.declarations.MethodDeclaration;
import me.tomassetti.symbolsolver.model.declarations.ParameterDeclaration;
import me.tomassetti.symbolsolver.model.declarations.TypeDeclaration;
import me.tomassetti.symbolsolver.model.resolution.TypeParameter;
import me.tomassetti.symbolsolver.model.resolution.TypeSolver;

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
					for (int i = 0; i < methodDecl.getNoParams(); i++)
					{
						parameterNames.add(JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(
							methodDecl.getParam(i).getType(), m_typeSolver, m_ignoredContexts
						));
					}
					
					declName = new JavaDeclName(
						methodDecl.getName(), 
						getTypeParameterNames(methodDecl.getTypeParameters()), 
						JavaSymbolSolverTypeNameResolver.getQualifiedTypeName(methodDecl.getReturnType(), m_typeSolver, m_ignoredContexts),
						parameterNames
					);
							
					declName.setParent(
						getQualifiedDeclName(methodDecl.declaringType(), m_typeSolver, m_ignoredContexts)
					);
				}
				
			}
		}
		return declName;
	}
	
	private static List<String> getTypeParameterNames(List<TypeParameter> typeParameters)
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
