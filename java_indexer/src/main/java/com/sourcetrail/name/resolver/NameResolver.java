package com.sourcetrail.name.resolver;

import com.sourcetrail.ContextList;
import java.io.File;
import java.util.Optional;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.CompilationUnit;

public abstract class NameResolver
{
	protected File m_currentFile = null;
	protected ContextList m_ignoredContexts = null;
	protected CompilationUnit m_compilationUnit = null;

	static protected <N> Optional<N> getAncestorOfType(ASTNode node, Class<N> classType)
	{
		ASTNode parent = node.getParent();
		while (parent != null)
		{
			if (classType.isAssignableFrom(parent.getClass()))
			{
				return Optional.of(classType.cast(parent));
			}
			parent = parent.getParent();
		}
		return Optional.empty();
	}

	public NameResolver(File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		m_currentFile = currentFile;
		m_compilationUnit = compilationUnit;

		if (ignoredContexts != null)
		{
			m_ignoredContexts = ignoredContexts;
		}
		else
		{
			m_ignoredContexts = new ContextList();
		}
	}
}
