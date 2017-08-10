package com.sourcetrail;

import java.io.File;

import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

public abstract class JavaNameResolver 
{
	File m_currentFile = null;
	TypeSolver m_typeSolver = null;
	ContextList m_ignoredContexts = null;
	 
	public JavaNameResolver(File currentFile, TypeSolver typeSolver, ContextList ignoredContexts)
	{
		m_currentFile = currentFile;
		m_typeSolver = typeSolver;
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
