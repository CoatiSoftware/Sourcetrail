package com.sourcetrail;

import java.nio.file.Path;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.BodyDeclaration;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.Javadoc;

public class Utility
{
	public static Range getRange(ASTNode node, CompilationUnit compilationUnit)
	{
		int startPosition = node.getStartPosition() + 1;
		int endPosition = node.getStartPosition() + node.getLength() - 1;

		if (node instanceof BodyDeclaration && ((BodyDeclaration)node).getJavadoc() != null)
		{
			Javadoc javadoc = ((BodyDeclaration)node).getJavadoc();
			startPosition = javadoc.getStartPosition() + javadoc.getLength() + 2;
		}

		return new Range(
			compilationUnit.getLineNumber(startPosition),
			compilationUnit.getColumnNumber(startPosition),
			compilationUnit.getLineNumber(endPosition),
			compilationUnit.getColumnNumber(endPosition) + 1);
	}

	public static String getFilenameWithoutExtension(Path filePath)
	{
		return filePath.getFileName().toString().replaceFirst("[.][^.]+$", "");
	}
}
