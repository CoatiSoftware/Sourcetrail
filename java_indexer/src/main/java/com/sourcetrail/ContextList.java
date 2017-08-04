package com.sourcetrail;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParametrizable;

public class ContextList
{
	private List<BodyDeclaration> m_bodyDeclarations = new ArrayList<>();
	private List<TypeParametrizable> m_typeParameterizables = new ArrayList<>();
	
	public ContextList copy()
	{
		ContextList contextList = new ContextList();

		contextList.m_bodyDeclarations = new ArrayList<>(m_bodyDeclarations);
		contextList.m_typeParameterizables = new ArrayList<>(m_typeParameterizables);
		
		return contextList;
	}
	
	public void add(BodyDeclaration v) 
	{
		m_bodyDeclarations.add(v);
	}
	
	public boolean contains(BodyDeclaration v)
	{
		for (BodyDeclaration bodyDeclaration: m_bodyDeclarations)
		{
			if (bodyDeclaration.equals(v))
			{
				return true;
			}
		}
		return false;
	}
	
	public void add(TypeParametrizable v) 
	{
		m_typeParameterizables.add(v);
	}
	
	public boolean contains(TypeParametrizable v)
	{
		for (TypeParametrizable typeParameterizable: m_typeParameterizables)
		{
			if (typeParameterizable.toString().equals(v.toString()))
			{
				return true;
			}
		}
		return false;
	}
}
