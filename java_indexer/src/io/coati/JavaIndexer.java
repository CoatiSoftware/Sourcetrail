package io.coati;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.lang.String;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.JavaParser;
import com.github.javaparser.ParseException;
import com.github.javaparser.Token;

import me.tomassetti.symbolsolver.javaparsermodel.JavaParserFacade;
import me.tomassetti.symbolsolver.resolution.typesolvers.CombinedTypeSolver;
import me.tomassetti.symbolsolver.resolution.typesolvers.JarTypeSolver;
import me.tomassetti.symbolsolver.resolution.typesolvers.JavaParserTypeSolver;
import me.tomassetti.symbolsolver.resolution.typesolvers.JreTypeSolver;

public class JavaIndexer 
{
	public static void processFile(int address, String filePath, String fileContent, String classPath)
	{
		System.out.println("indexing file: " + filePath);
		
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
						JarTypeSolver jarTypeSolver = new JarTypeSolver(path);
						typeSolver.add(jarTypeSolver);
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
			
			JavaAstVisitor astVisitor = new JavaAstVisitor(address, filePath, typeSolver);
		//	JavaAstVisitor astVisitor = new ASTDumper(address, filePath, typeSolver);
			cu.accept(astVisitor, null);
		} 
		catch (ParseException e) 
		{
			if (e.expectedTokenSequences == null || e.expectedTokenSequences.length == 0)
			{
				Token token = e.currentToken;
				recordError(
					address, e.getMessage(), true, true, 
					token.beginLine, token.beginColumn, token.endLine, token.endColumn
				);
			}
			else
			{
				Token token = e.currentToken.next;
				recordError(
					address, "Encountered unexpected token.", true, true, 
					token.beginLine, token.beginColumn, token.endLine, token.endColumn
				);
			}
		}
		
		JavaParserFacade.clearInstances();
		
	//	String fileName = filePath.substring(filePath.lastIndexOf("/"), filePath.lastIndexOf(".java"));
		
	//	System.gc();
	//	HeapDumper.dumpHeap("D:/dump/" + fileName, false);
	}
	
	
	
	
	
	
	static public void recordSymbol(
		int address, String symbolName, SymbolType symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		AccessKind access, boolean isImplicit
	)
	{
		recordSymbol(
			address, symbolName, symbolType.getValue(), 
			beginLine, beginColumn, endLine, endColumn, 
			access.getValue(), (isImplicit ? 1 : 0)
		);
	}
	
	static public void recordSymbolWithoutLocation(
			int address, String symbolName, SymbolType symbolType, 
			AccessKind access, boolean isImplicit
		)
		{
			recordSymbolWithoutLocation(
				address, symbolName, symbolType.getValue(),  
				access.getValue(), (isImplicit ? 1 : 0)
			);
		}
	
	static public void recordSymbolWithScope(
		int address, String symbolName, SymbolType symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
		AccessKind access, boolean isImplicit
	)
	{
		recordSymbolWithScope(
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

	static private native void recordSymbol(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int access, int isImplicit
	);
	
	static private native void recordSymbolWithoutLocation(
		int address, String symbolName, int symbolType, 
		int access, int isImplicit
	);
	
	static private native void recordSymbolWithScope(
		int address, String symbolName, int symbolType, 
		int beginLine, int beginColumn, int endLine, int endColumn,
		int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
		int access, int isImplicit
	);
	
	static private native void recordReference(
		int address, int referenceKind, String referencedName, String contextName, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static native void recordLocalSymbol(
		int address, String symbolName, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static native void recordComment(
		int address, int beginLine, int beginColumn, int endLine, int endColumn
	);
	
	static private native void recordError(
		int address, String message, int fatal, int indexed, int beginLine, int beginColumn, int endLine, int endColumn
	);
}