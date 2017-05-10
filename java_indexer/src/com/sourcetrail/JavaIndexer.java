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
import com.github.javaparser.Problem;
import com.github.javaparser.Range;
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
		logInfo(address, "indexing source file: " + filePath);
		
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

			JavaAstVisitor astVisitor = (
				verbose == 1 ? 
				new JavaVerboseAstVisitor(address, filePath, new FileContent(fileContent), combinedTypeSolver) : 
				new JavaAstVisitor(address, filePath, new FileContent(fileContent), combinedTypeSolver)
			);
			
			cu.accept(astVisitor, null);
		} 
		catch (ParseProblemException e) 
		{
			for (Problem problem: e.getProblems())
			{
				String message = problem.toString();
				if (message.startsWith("(line "))
				{
					int startLine = Integer.parseInt(message.substring(
						message.indexOf("line ") + ("line ").length(), 
						message.indexOf(",")
					));
					
					int startColumn = Integer.parseInt(message.substring(
						message.indexOf("col ") + ("col ").length(), 
						message.indexOf(")")
					));
					
					recordError(
						address, "Encountered unexpected token.", true, true, 
						Range.range(startLine, startColumn, startLine, startColumn)
					);
				}
				else
				{		
					Optional<Range> range = problem.getRange();
					if (range.isPresent())
					{
						recordError(
							address, problem.toString(), true, true, 
							range.get()
						);
					}
				}
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
	
	// helpers
	
	static public void recordSymbol(
		int address, String symbolName, SymbolKind symbolType, 
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbol(
			address, symbolName, symbolType.getValue(),  
			access.getValue(), definitionKind.getValue()
		);
	}
	
	static public void recordSymbolWithLocation(
		int address, String symbolName, SymbolKind symbolType, 
		Optional<Range> range,
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbolWithLocation(
			address, symbolName, symbolType, 
			range.orElse(Range.range(0, 0, 0, 0)),
			access, definitionKind
		);
	}
	
	static public void recordSymbolWithLocation(
		int address, String symbolName, SymbolKind symbolType, 
		Range range,
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbolWithLocation(
			address, symbolName, symbolType.getValue(), 
			range.begin.line, range.begin.column, range.end.line, range.end.column, 
			access.getValue(), definitionKind.getValue()
		);
	}
	
	static public void recordSymbolWithLocationAndScope(
		int address, String symbolName, SymbolKind symbolType, 
		Optional<Range> range,
		Optional<Range> scopeRange,
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbolWithLocationAndScope(
			address, symbolName, symbolType, 
			range.orElse(Range.range(0, 0, 0, 0)),
			scopeRange.orElse(Range.range(0, 0, 0, 0)),
			access, definitionKind
		);
	}
	
	static public void recordSymbolWithLocationAndScope(
		int address, String symbolName, SymbolKind symbolType, 
		Range range,
		Range scopeRange,
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbolWithLocationAndScope(
			address, symbolName, symbolType.getValue(), 
			range.begin.line, range.begin.column, range.end.line, range.end.column, 
			scopeRange.begin.line, scopeRange.begin.column, scopeRange.end.line, scopeRange.end.column, 
			access.getValue(), definitionKind.getValue()
		);
	}
	
	static public void recordReference(
		int address, ReferenceKind referenceKind, String referencedName, String contextName, 
		Optional<Range> range
	)
	{
		recordReference(
			address, referenceKind, referencedName, contextName, 
			range.orElse(Range.range(0, 0, 0, 0))
		);
	}
	
	static public void recordReference(
		int address, ReferenceKind referenceKind, String referencedName, String contextName, 
		Range range
	)
	{
		recordReference(
			address, referenceKind.getValue(), referencedName, contextName, 
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}
	
	static public void recordLocalSymbol(
		int address, String symbolName,
		Optional<Range> range
	)
	{
		recordLocalSymbol(address, symbolName, range.orElse(Range.range(0, 0, 0, 0)));
	}
		
	static public void recordLocalSymbol(
		int address, String symbolName,
		Range range
	)
	{
		recordLocalSymbol(
			address, symbolName,
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}
	
	static public void recordComment(
		int address,
		Optional<Range> range
	)
	{
		recordComment(address, range.orElse(Range.range(0, 0, 0, 0)));
	}
	
	static public void recordComment(
		int address,
		Range range
	)
	{
		recordComment(
			address,
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}
	
	static public void recordError(
		int address, String message, boolean fatal, boolean indexed, 
		Optional<Range> range
	)
	{
		recordError(
			address, message, fatal, indexed, 
			range.orElse(Range.range(0, 0, 0, 0))
		);
	}
	
	static public void recordError(
		int address, String message, boolean fatal, boolean indexed, 
		Range range
	)
	{
		recordError(
			address, message, (fatal ? 1 : 0), (indexed ? 1 : 0), 
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}
	
	// the following methods are defined in the native c++ code

	static public native void logInfo(int address, String info);
	
	static public native void logWarning(int address, String warning);
	
	static public native void logError(int address, String error);
	
	static private native void recordSymbol(
		int address, String symbolName, int symbolType, 
		int access, int definitionKind
	);

	static private native void recordSymbolWithLocation(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int access, int definitionKind
	);
	
	static private native void recordSymbolWithLocationAndScope(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
		int access, int definitionKind
	);
	
	static private native void recordReference(
		int address, int referenceKind, String referencedName, String contextName, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static public native void recordLocalSymbol(
		int address, String symbolName, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static public native void recordComment(
		int address, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static private native void recordError(
		int address, String message, int fatal, int indexed, int beginLine, int beginColumn, int endLine, int endColumn
	);
}