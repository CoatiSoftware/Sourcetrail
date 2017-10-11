package com.sourcetrail;

import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.eclipse.jdt.core.JavaCore;
import org.eclipse.jdt.core.compiler.IProblem;
import org.eclipse.jdt.core.dom.AST;
import org.eclipse.jdt.core.dom.ASTParser;
import org.eclipse.jdt.core.dom.ASTVisitor;
import org.eclipse.jdt.core.dom.BlockComment;
import org.eclipse.jdt.core.dom.Comment;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.LineComment;
import org.eclipse.jdt.core.dom.PackageDeclaration;

public class JavaIndexer 
{	
	public static void processFile(int address, String filePath, String fileContent, String classPath, int verbose)
	{
		processFile(new JavaIndexerAstVisitorClient(address), filePath, fileContent, classPath, verbose);
	}
	
	public static void processFile(AstVisitorClient astVisitorClient, String filePath, String fileContent, String classPath, int verbose)
	{
		astVisitorClient.logInfo("indexing source file: " + filePath);
		
		Path path = Paths.get(filePath);
	
		ASTParser parser = ASTParser.newParser(AST.JLS8);
		
		parser.setResolveBindings(true); // solve "bindings" like the declatarion of the type used in a var decl
		parser.setKind(ASTParser.K_COMPILATION_UNIT); // specify to parse the entire compilation unit
		parser.setBindingsRecovery(true); // also return bindings that are not resolved completely
        parser.setStatementsRecovery(true);
		
        Hashtable<String, String> options = JavaCore.getOptions();
        JavaCore.setComplianceOptions(JavaCore.VERSION_1_8, options);
        parser.setCompilerOptions(options);
		
		parser.setUnitName(path.getFileName().toString());

		List<String> classpath = new ArrayList<>();
		List<String> sources = new ArrayList<>();
		
		for (String classPathEntry: classPath.split("\\;"))
		{	
			if (classPathEntry.endsWith(".jar"))
			{
				classpath.add(classPathEntry);
			}
			else if (!classPathEntry.isEmpty())
			{
				sources.add(classPathEntry);
			}		
		}
		
		parser.setEnvironment(classpath.toArray(new String[0]), sources.toArray(new String[0]), null, true);
		parser.setSource(fileContent.toCharArray());
		
		CompilationUnit cu = (CompilationUnit) parser.createAST(null);
		
		ASTVisitor visitor;
		if (verbose != 0)
		{
			visitor = new VerboseContextAwareAstVisitor(astVisitorClient, path.toFile(), fileContent, cu);
		}
		else
		{
			visitor = new ContextAwareAstVisitor(astVisitorClient, path.toFile(), fileContent, cu);
		}
		
		cu.accept(visitor);

		for (IProblem problem: cu.getProblems())
		{
			if (problem.isError())
			{
				Range range = new Range(
						cu.getLineNumber(problem.getSourceStart()),
						cu.getColumnNumber(problem.getSourceStart() + 1),
						cu.getLineNumber(problem.getSourceEnd()),
						cu.getColumnNumber(problem.getSourceEnd()) + 1);

				astVisitorClient.recordError(problem.getMessage(), false, true, range);
			}
		}
		
		for (Object commentObject: cu.getCommentList())
		{
			if ((commentObject instanceof LineComment) || (commentObject instanceof BlockComment))
			{
				((Comment) commentObject).accept(visitor);
			}
		}
	
	}
	
	public static String getPackageName(String fileContent)
	{
		String packageName = "";
		
		ASTParser parser = ASTParser.newParser(AST.JLS8);
		parser.setKind(ASTParser.K_COMPILATION_UNIT); // specify to parse the entire compilation unit
		parser.setSource(fileContent.toCharArray());
		CompilationUnit cu = (CompilationUnit) parser.createAST(null);
		PackageDeclaration packageDeclaration = cu.getPackage();
		if (packageDeclaration != null)
		{
			packageName = packageDeclaration.getName().getFullyQualifiedName();
		}
		return packageName;
	}
	
	public static void clearCaches()
	{
		Runtime.getRuntime().gc();
	}
	
	
	
	// the following methods are defined in the native c++ code

	static public native boolean getInterrupted(int address);
	
	static public native void logInfo(int address, String info);
	
	static public native void logWarning(int address, String warning);
	
	static public native void logError(int address, String error);
	
	static public native void recordSymbol(
			int address, String symbolName, int symbolType, 
			int access, int definitionKind);

	static public native void recordSymbolWithLocation(
			int address, String symbolName, int symbolType, 
			int beginLine, int beginColumn, int endLine, int endColumn,
			int access, int definitionKind);
	
	static public native void recordSymbolWithLocationAndScope(
			int address, String symbolName, int symbolType, 
			int beginLine, int beginColumn, int endLine, int endColumn,
			int scopeBeginLine, int scopeBeginColumn, int scopeEndLine, int scopeEndColumn,
			int access, int definitionKind
	);

	static public native void recordReference(
			int address, int referenceKind, String referencedName, String contextName, int beginLine, int beginColumn, int endLine, int endColumn);

	static public native void recordQualifierLocation(
			int address, String qualifierName, int beginLine, int beginColumn, int endLine, int endColumn);
	
	static public native void recordLocalSymbol(
			int address, String symbolName, int beginLine, int beginColumn, int endLine, int endColumn);
	
	static public native void recordComment(
			int address, int beginLine, int beginColumn, int endLine, int endColumn);
	
	static public native void recordError(
			int address, String message, int fatal, int indexed, int beginLine, int beginColumn, int endLine, int endColumn);
}