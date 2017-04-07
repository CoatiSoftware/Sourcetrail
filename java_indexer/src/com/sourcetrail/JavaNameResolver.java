package com.sourcetrail;

import java.util.ArrayList;

import com.github.javaparser.ast.body.BodyDeclaration;

import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

public abstract class JavaNameResolver 
{
	TypeSolver m_typeSolver = null;
	ArrayList<BodyDeclaration> m_ignoredContexts = null;
	 
	public JavaNameResolver(TypeSolver typeSolver, ArrayList<BodyDeclaration> ignoredContexts)
	{
		m_typeSolver = typeSolver;
		if (ignoredContexts != null)
		{
			m_ignoredContexts = ignoredContexts;
		}
		else
		{
			m_ignoredContexts = new ArrayList<BodyDeclaration>();
		}
	} 
	
	protected boolean ignoresContext(BodyDeclaration context)
	{
		if (m_ignoredContexts != null)
		{
			for (BodyDeclaration ignoredContext: m_ignoredContexts)
			{
				if (ignoredContext.equals(context))
				{
					return true;
				}
			}
		}
		return false;
	}
	
}
