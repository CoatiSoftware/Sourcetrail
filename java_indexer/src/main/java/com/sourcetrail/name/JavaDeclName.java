package com.sourcetrail.name;

import java.util.List;

public class JavaDeclName 
{
	private JavaDeclName m_parent = null;
	private String m_name = "";
	private List<String> m_typeParameterNames = null;
	
	public static JavaDeclName unsolved()
	{
		return new JavaDeclName("unsolved-symbol");
	}
	
	public static JavaDeclName fromDotSeparatedString(String s)
	{
		JavaDeclName declName = null;

		int separatorIndex = s.lastIndexOf('.');
		if (separatorIndex != -1)
		{
			declName = new JavaDeclName(s.substring(separatorIndex + 1));
			declName.setParent(JavaDeclName.fromDotSeparatedString(s.substring(0, separatorIndex)));
		}
		else
		{
			declName = new JavaDeclName(s, null);
		}
		
		return declName;
	}
	
	public JavaDeclName(String name)
	{
		m_name = name;
	}
	
	public JavaDeclName(String name, List<String> typeParameterNames)
	{
		m_name = name;
		m_typeParameterNames = typeParameterNames;
	}
	
	public void setParent(JavaDeclName parent)
	{
		m_parent = parent;
	}
	
	public JavaDeclName getParent()
	{
		return m_parent;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public NameHierarchy toNameHierarchy()
	{
		NameHierarchy nameHierarchy;
		
		if (m_parent != null)
		{
			nameHierarchy = m_parent.toNameHierarchy();
		}
		else
		{
			nameHierarchy = new NameHierarchy();
		}
		
		nameHierarchy.push(new NameElement(m_name + getTypeParameterString()));
		
		return nameHierarchy;
	}
	
	public String toString()
	{
		String string = "";
		if (m_parent != null)
		{
			string = m_parent.toString();
			string += ".";
		}
		
		string += m_name;
		string += getTypeParameterString();
		
		return string;
	}
	
	public String getTypeParameterString()
	{
		String string = "";
		if (m_typeParameterNames != null && !m_typeParameterNames.isEmpty())
		{
			string += "<";
			for (int i = 0; i < m_typeParameterNames.size(); i++)
			{
				if (i != 0)
				{
					string += ", ";
				}
				string += m_typeParameterNames.get(i);
			}
			string += ">";
		}
		return string;
	}
}
