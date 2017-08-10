package com.sourcetrail;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

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

import com.sourcetrail.name.JavaDeclName;
import com.sourcetrail.name.JavaTypeName;

public class JavaSymbolSolverTypeNameResolver extends JavaNameResolver
{
	public JavaSymbolSolverTypeNameResolver(File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		super(currentFile, typeSolver, ignoredContexts);
	}

	public static JavaTypeName getQualifiedTypeName(Type type, File currentFile, TypeSolver typeSolver)
	{
		return getQualifiedTypeName(type, currentFile, typeSolver, null);
	}
	
	public static JavaTypeName getQualifiedTypeName(Type type, File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		JavaSymbolSolverTypeNameResolver resolver = new JavaSymbolSolverTypeNameResolver(currentFile, typeSolver, ignoredContexts);
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
					m_currentFile, 
					m_typeSolver, 
					m_ignoredContexts);
			return new JavaTypeName(declName.getName() + declName.getTypeParameterString(), new ArrayList<JavaTypeName>(), declName.getParent());
		}
		else if (type instanceof TypeVariable)
		{
			JavaDeclName declName = JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(
					((TypeVariable)type).asTypeParameter(), 
					m_currentFile, 
					m_typeSolver, 
					m_ignoredContexts);
			return new JavaTypeName(declName.getName(), new ArrayList<JavaTypeName>(), declName.getParent());
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
		
		return JavaTypeName.unsolved(); 
	}
}
