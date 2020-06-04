package com.sourcetrail.name;

public class NameElement
{
	private String m_name = "";
	private String m_prefix = "";
	private String m_postfix = "";


	public NameElement(String name)
	{
		if (name != null)
			m_name = name;
	}

	public NameElement(String name, String prefix, String postfix)
	{
		if (name != null)
			m_name = name;
		if (prefix != null)
			m_prefix = prefix;
		if (postfix != null)
			m_postfix = postfix;
	}

	String getName()
	{
		return m_name;
	}

	String getNameWithSignature()
	{
		String nameWithSignature = m_name;

		if (m_prefix.length() > 0 || m_postfix.length() > 0)
		{
			nameWithSignature = m_prefix;
			if (!m_name.isEmpty())
			{
				if (!m_prefix.isEmpty())
				{
					nameWithSignature += " ";
				}
				nameWithSignature += m_name;
			}
			nameWithSignature += m_postfix;
		}

		return nameWithSignature;
	}

	String serialize()
	{
		return m_name + "\ts" + m_prefix + "\tp" + m_postfix;
	}
}
