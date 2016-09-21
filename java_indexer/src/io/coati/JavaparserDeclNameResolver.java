package io.coati;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EmptyMemberDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.InitializerDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.TypeDeclaration;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.QualifiedNameExpr;

import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserClassDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserInterfaceDeclaration;
import me.tomassetti.symbolsolver.model.resolution.TypeSolver;

public class JavaparserDeclNameResolver extends JavaNameResolver
{	
	public JavaparserDeclNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts) 
	{
		super(typeSolver, ignoredContexts);
	}

	public static JavaDeclName getQualifiedDeclName(VariableDeclarator decl, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(VariableDeclarator decl, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		JavaparserDeclNameResolver resolver = new JavaparserDeclNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(VariableDeclarator decl)
	{
		JavaDeclName declName = null;
		
		if (decl != null)
		{
			declName = getDeclName(decl);
			
			BodyDeclaration declContext = getBodyDeclContext(decl);
			if (declContext != null)
			{
				if (!ignoresContext(declContext))
				{
					declName.setParent(getQualifiedDeclName(declContext));
				}
			}
			else
			{
				CompilationUnit compilationUnit = getCompilationUnitContext(decl);
			
				if (compilationUnit != null)
				{
					PackageDeclaration packageDecl = compilationUnit.getPackage();
					if (packageDecl != null)
					{
						declName.setParent(getQualifiedName(packageDecl.getName()));
					}
				}
				else
				{
					throw new UnsupportedOperationException();
				}
			}
		}
		return declName;
	}
	
	public static JavaDeclName getQualifiedDeclName(BodyDeclaration decl, TypeSolver typeSolver)
	{
		return getQualifiedDeclName(decl, typeSolver, null);
	}
	
	public static JavaDeclName getQualifiedDeclName(BodyDeclaration decl, TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		JavaparserDeclNameResolver resolver = new JavaparserDeclNameResolver(typeSolver, ignoredContexts);
		return resolver.getQualifiedDeclName(decl);
	}
	
	public JavaDeclName getQualifiedDeclName(BodyDeclaration decl)
	{
		JavaDeclName declName = null;
		
		if (decl != null)
		{
			declName = getDeclName(decl);
			
			BodyDeclaration declContext = getBodyDeclContext(decl);
			if (declContext != null)
			{
				if (!ignoresContext(declContext))
				{
					declName.setParent(getQualifiedDeclName(declContext));
				}
			}
			else
			{
				CompilationUnit compilationUnit = getCompilationUnitContext(decl);
			
				if (compilationUnit != null)
				{
					PackageDeclaration packageDecl = compilationUnit.getPackage();
					if (packageDecl != null)
					{
						declName.setParent(getQualifiedName(packageDecl.getName()));
					}
				}
				else
				{
					throw new UnsupportedOperationException();
				}
			}
		}
		return declName;
	}	
	
	public JavaDeclName getDeclName(VariableDeclarator decl)
	{
		return new JavaDeclName(decl.getId().getName());
	}
	
	public JavaDeclName getDeclName(BodyDeclaration decl)
	{
		JavaDeclName declName = null;
		
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
			else if (decl instanceof EmptyMemberDeclaration)
			{
				declName = new JavaDeclName("EmptyMemberDeclaration");
			}
			else if (decl instanceof EnumConstantDeclaration)
			{
				declName = new JavaDeclName(((EnumConstantDeclaration)decl).getName());
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
				declName = new JavaDeclName(((TypeDeclaration)decl).getName(), getTypeParameterNames((TypeDeclaration)decl));
			}
		}
		
		return declName;
	}
	
	private static List<String> getTypeParameterNames(TypeDeclaration decl)
	{
		List<TypeParameter> typeParameters = null;
		if (decl instanceof ClassOrInterfaceDeclaration)
		{
			typeParameters = ((ClassOrInterfaceDeclaration)decl).getTypeParameters();
		}

		return getTypeParameterNames(typeParameters);
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
	
	public static JavaDeclName getQualifiedName(NameExpr nameExpr)
	{
		JavaDeclName declName = new JavaDeclName(nameExpr.getName());
		if (nameExpr instanceof QualifiedNameExpr)
		{
			declName.setParent(getQualifiedName(((QualifiedNameExpr)nameExpr).getQualifier()));
		}
		return declName;
	}
	
	private <T extends CallableDecl> JavaDeclName getDeclNameOfCallable(T decl)
	{
		ArrayList<BodyDeclaration> ignoredContextsForTypes = new ArrayList<BodyDeclaration>(m_ignoredContexts);
		ignoredContextsForTypes.add(decl.getWrappedNode()); // adding own decl
		
		String name = decl.getName();
		List<String> typeParameterNames = getTypeParameterNames(decl.getTypeParameters());
		JavaTypeName returnTypeName = JavaparserTypeNameResolver.getQualifiedTypeName(decl.getType(), m_typeSolver, ignoredContextsForTypes);
		List<JavaTypeName> parameterNames = new ArrayList<>();
		for (Parameter parameter: decl.getParameters())
		{
			parameterNames.add(JavaparserTypeNameResolver.getQualifiedTypeName(parameter.getType(), m_typeSolver, ignoredContextsForTypes));
		}
		
		return new JavaDeclName(name, typeParameterNames, returnTypeName, parameterNames);
	}

	private static BodyDeclaration getBodyDeclContext(Node decl)
	{
		BodyDeclaration context = null;
	
		Node parentNode = decl.getParentNode();
		while (
			parentNode != null && 
			!(
				parentNode instanceof BodyDeclaration && 
				(!(parentNode instanceof FieldDeclaration))
			) 
		)
		{
			parentNode = parentNode.getParentNode();
		}
		
		if (parentNode != null)
		{
			context = (BodyDeclaration)parentNode;
		}
		
		return context;
	}
	
	private static CompilationUnit getCompilationUnitContext(Node decl)
	{
		CompilationUnit context = null;
		{
			Node parentNode = decl.getParentNode();
			while (parentNode != null && !(parentNode instanceof CompilationUnit))
			{
				parentNode = parentNode.getParentNode();
			}
			
			if (parentNode != null)
			{
				context = (CompilationUnit)parentNode;
			}
		}
		return context;
	}
}
