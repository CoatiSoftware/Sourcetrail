package io.coati;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.lang.String;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.JavaParser;
import com.github.javaparser.ParseProblemException;
import com.github.javaparser.Problem;

import me.tomassetti.symbolsolver.javaparser.Navigator;
import me.tomassetti.symbolsolver.javaparsermodel.JavaParserFacade;
import me.tomassetti.symbolsolver.resolution.typesolvers.CombinedTypeSolver;
import me.tomassetti.symbolsolver.resolution.typesolvers.JarTypeSolver;
import me.tomassetti.symbolsolver.resolution.typesolvers.JavaParserTypeSolver;
import me.tomassetti.symbolsolver.resolution.typesolvers.JreTypeSolver;

public class JavaIndexer 
{
	public static void processFile(int address, String filePath, String fileContent, String classPath, int verbose)
	{
		logInfo(address, "indexing source file: " + filePath);
		
		try 
		{
			CombinedTypeSolver typeSolver = new CombinedTypeSolver();

			typeSolver.add(new JreTypeSolver());
			for (String path: classPath.split("\\;"))
			{
				if (path.endsWith(".jar"))
				{
					try
					{
						JarTypeSolver solver = new JarTypeSolver(path);
						typeSolver.add(solver);
					}
					catch (IOException e)
					{
						System.out.println("unable to add jar file: " + path);
					}
				}
				else
				{
					JavaParserTypeSolver solver = new JavaParserTypeSolver(new File(path));
					typeSolver.add(solver);
				}
			}
			CompilationUnit cu = JavaParser.parse(new StringReader(fileContent), true);

			JavaAstVisitor astVisitor = (
				verbose == 1 ? 
				new JavaVerboseAstVisitor(address, filePath, new FileContent(fileContent), typeSolver) : 
				new JavaAstVisitor(address, filePath, new FileContent(fileContent), typeSolver)
			);
			
			cu.accept(astVisitor, null);
		} 
		catch (ParseProblemException e) 
		{
			for (Problem problem: e.problems)
			{
				if (problem.message.startsWith("Encountered unexpected token"))
				{
					int startLine = Integer.parseInt(problem.message.substring(
						problem.message.indexOf("line ") + ("line ").length(), 
						problem.message.indexOf(",")
					));
					
					int startColumn = Integer.parseInt(problem.message.substring(
						problem.message.indexOf("column ") + ("column ").length(), 
						problem.message.indexOf(".")
					));
					
					recordError(
						address, "Encountered unexpected token.", true, true, 
						startLine, startColumn, 
						startLine, startColumn
					);
				}
				else
				{			
					recordError(
						address, problem.toString(), true, true, 
						problem.range.begin.line, problem.range.begin.column, 
						problem.range.end.line, problem.range.end.column
					);
				}
			}
		}
		
		JavaParserFacade.clearInstances();
	}
	
	public static String getPackageName(String fileContent)
	{
		String packageName = "";
		try 
		{
			CompilationUnit cu = JavaParser.parse(new StringReader(fileContent), true);
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
	
	// helpers
	
	static public void recordSymbol(
		int address, String symbolName, SymbolType symbolType, 
		AccessKind access, boolean isImplicit
	)
	{
		recordSymbol(
			address, symbolName, symbolType.getValue(),  
			access.getValue(), (isImplicit ? 1 : 0)
		);
	}
	
	static public void recordSymbolWithLocation(
		int address, String symbolName, SymbolType symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		AccessKind access, boolean isImplicit
	)
	{
		recordSymbolWithLocation(
			address, symbolName, symbolType.getValue(), 
			beginLine, beginColumn, endLine, endColumn, 
			access.getValue(), (isImplicit ? 1 : 0)
		);
	}
	
	static public void recordSymbolWithLocationAndScope(
		int address, String symbolName, SymbolType symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
		AccessKind access, boolean isImplicit
	)
	{
		recordSymbolWithLocationAndScope(
			address, symbolName, symbolType.getValue(), 
			beginLine, beginColumn, endLine, endColumn, 
			scopeBeginLine, scopeBeginColumn, scopeEndLine, scopeEndColumn,  
			access.getValue(), (isImplicit ? 1 : 0)
		);
	}
	
	static public void recordReference(
		int address, ReferenceKind referenceKind, String referencedName, String contextName, 
		int beginLine, int beginColumn, int endLine, int endColumn
	)
	{
		recordReference(
			address, referenceKind.getValue(), referencedName, contextName, 
			beginLine, beginColumn, endLine, endColumn
		);
	}
	
	static public void recordError(
		int address, String message, boolean fatal, boolean indexed, int beginLine, int beginColumn, int endLine, int endColumn
	)
	{
		recordError(
			address, message, (fatal ? 1 : 0), (indexed ? 1 : 0), 
			beginLine, beginColumn, endLine, endColumn
		);
	}
	
	// the following methods are defines in the native c++ code

	static public native void logInfo(int address, String info);
	
	static public native void logWarning(int address, String warning);
	
	static public native void logError(int address, String error);
	
	static private native void recordSymbol(
		int address, String symbolName, int symbolType, 
		int access, int isImplicit
	);

	static private native void recordSymbolWithLocation(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int access, int isImplicit
	);
	
	static private native void recordSymbolWithLocationAndScope(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
		int access, int isImplicit
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