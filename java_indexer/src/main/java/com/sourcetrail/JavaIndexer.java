package com.sourcetrail;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.lang.String;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.JavaParser;
import com.github.javaparser.ParseProblemException;
import com.github.javaparser.Position;
import com.github.javaparser.Problem;
import com.github.javaparser.Range;
import com.github.javaparser.TokenRange;
import com.github.javaparser.symbolsolver.javaparser.Navigator;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFacade;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.resolution.typesolvers.CombinedTypeSolver;
import com.github.javaparser.symbolsolver.resolution.typesolvers.JarTypeSolver;
import com.github.javaparser.symbolsolver.resolution.typesolvers.JavaParserTypeSolver;
import com.github.javaparser.symbolsolver.resolution.typesolvers.ReflectionTypeSolver;

public class JavaIndexer 
{
	private static Map<String, TypeSolver> typeSolvers = new HashMap<>();
			
	public static void processFile(int address, String filePath, String fileContent, String classPath, int verbose)
	{
		AstVisitorClient astVisitorClient = new JavaIndexerAstVisitorClient(address);
		
		astVisitorClient.logInfo( "indexing source file: " + filePath);
		
		try 
		{
			CombinedTypeSolver combinedTypeSolver = new CombinedTypeSolver();

			combinedTypeSolver.add(new ReflectionTypeSolver());
			for (String path: classPath.split("\\;"))
			{
				if (typeSolvers.containsKey(path))
				{
					combinedTypeSolver.add(typeSolvers.get(path));
				}
				else 
				{
					TypeSolver typeSolver = null;
					
					if (path.endsWith(".jar"))
					{
						try
						{
							typeSolver = new JarTypeSolver(path);
						}
						catch (IOException e)
						{
							System.out.println("unable to add jar file: " + path);
						}
					}
					else if (!path.isEmpty())
					{
						typeSolver = new JavaParserTypeSolver(new File(path));
					}
					
					if (typeSolver != null)
					{
						typeSolvers.put(path, typeSolver);
						combinedTypeSolver.add(typeSolver);
					}
				}
			}
			
			CompilationUnit cu = JavaParser.parse(new StringReader(fileContent));

			AstVisitor astVisitor = (
				verbose == 1 ? 
				new VerboseAstVisitor(astVisitorClient, filePath, new FileContent(fileContent), combinedTypeSolver) : 
				new AstVisitor(astVisitorClient, filePath, new FileContent(fileContent), combinedTypeSolver)
			);
			
			cu.accept(astVisitor, null);
		} 
		catch (ParseProblemException e) 
		{
			for (Problem problem: e.getProblems())
			{
				// "Parse error. Found \"package\", expected one of  \";\" \"@\" \"\\u001a\" \"abstract\" \"class\" \"default\" \"enum\" \"final\" \"import\" \"interface\" \"module\" \"native\" \"open\" \"private\" \"protected\" \"public\" \"static\" \"stric...
				String message = problem.toString();
				if (message.startsWith("Parse error. Found "))
				{
					message = "Encountered unexpected token.";
				}		
				
				Range range = new Range(new Position(0, 0), new Position(0, 0));
				if (problem.getLocation().isPresent())
				{
					range = problem.getLocation().get().toRange();
				}
				
				astVisitorClient.recordError(message, true, true, range);
			}
		}
	}
	
	public static String getPackageName(String fileContent)
	{
		String packageName = "";
		try 
		{
			CompilationUnit cu = JavaParser.parse(new StringReader(fileContent));
			PackageDeclaration pd = Navigator.findNodeOfGivenClass(cu, PackageDeclaration.class);
			if (pd != null)
			{
				packageName = JavaparserDeclNameResolver.getQualifiedName(pd.getName()).toString();
			}
		} 
		catch (ParseProblemException e) 
		{
			// do nothing
		}
		catch (IllegalArgumentException e) 
		{
			// do nothing
		}
		
		return packageName;
	}
	
	public static void clearCaches()
	{
		typeSolvers.clear();
		JavaParserFacade.clearInstances();
		Runtime.getRuntime().gc();
	}
	
	
	
	// the following methods are defined in the native c++ code

	static public native void logInfo(int address, String info);
	
	static public native void logWarning(int address, String warning);
	
	static public native void logError(int address, String error);
	
	static public native void recordSymbol(
		int address, String symbolName, int symbolType, 
		int access, int definitionKind
	);

	static public native void recordSymbolWithLocation(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int access, int definitionKind
	);
	
	static public native void recordSymbolWithLocationAndScope(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
		int access, int definitionKind
	);
	
	static public native void recordReference(
		int address, int referenceKind, String referencedName, String contextName, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static public native void recordLocalSymbol(
		int address, String symbolName, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static public native void recordComment(
		int address, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static public native void recordError(
		int address, String message, int fatal, int indexed, int beginLine, int beginColumn, int endLine, int endColumn
	);
}