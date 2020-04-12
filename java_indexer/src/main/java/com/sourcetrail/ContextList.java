package com.sourcetrail;

import java.util.HashSet;
import java.util.Set;
import org.eclipse.jdt.core.dom.IBinding;

public class ContextList
{
	private Set<String> m_bindingKeys = new HashSet<>();

	public ContextList copy()
	{
		ContextList contextList = new ContextList();

		contextList.m_bindingKeys = new HashSet<>(m_bindingKeys);

		return contextList;
	}

	public void add(IBinding v)
	{
		if (v != null)
		{
			m_bindingKeys.add(v.getKey());
		}
	}

	public boolean contains(IBinding v)
	{
		return v != null && m_bindingKeys.contains(v.getKey());
	}
}
