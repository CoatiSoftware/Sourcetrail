package com.sourcetrail;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.body.BodyDeclaration;

import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

public abstract class JavaNameResolver 
{
	TypeSolver m_typeSolver = null;
	ContextList m_ignoredContexts = null;
	 
	public JavaNameResolver(TypeSolver typeSolver, ContextList ignoredContexts)
	{
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
