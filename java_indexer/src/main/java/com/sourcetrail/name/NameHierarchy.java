package com.sourcetrail.name;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class NameHierarchy
{
	private List<NameElement> m_elements = new ArrayList<>();
	private char m_separatpr = '.';

	public NameHierarchy() {}

	public NameHierarchy(String name)
	{
		m_elements.add(new NameElement(name));
	}

	public NameHierarchy(NameElement name)
	{
		m_elements.add(name);
	}

	public NameHierarchy(List<NameElement> names)
	{
		m_elements.addAll(names);
	}

	public void setSeparator(char separator)
	{
		m_separatpr = separator;
	}

	public void push(NameElement element)
	{
		m_elements.add(element);
	}

	public void pop()
	{
		if (!m_elements.isEmpty())
		{
			m_elements.remove(m_elements.size() - 1);
		}
	}

	public Optional<NameElement> peek()
	{
		if (!m_elements.isEmpty())
		{
			return Optional.of(m_elements.get(m_elements.size() - 1));
		}
		return Optional.empty();
	}

	public String serialize()
	{
		String serialized = m_separatpr + "\tm";

		for (int i = 0; i < m_elements.size(); i++)
		{
			if (i != 0)
			{
				serialized += "\tn";
			}

			serialized += m_elements.get(i).serialize();
		}

		return serialized;
	}
}
