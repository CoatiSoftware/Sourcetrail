package com.sourcetrail.name;

import java.util.List;

public class JavaTypeName implements JavaSymbolName
{
	private JavaDeclName m_parent = null;
	private String m_name = "";
	private List<String> m_typeParameterNames = null;
	private List<JavaTypeName> m_typeArguments = null;
	private boolean m_isUnsolved = false;

	public static JavaTypeName unsolved()
	{
		JavaTypeName typeName = new JavaTypeName("unsolved-type", null);
		typeName.m_isUnsolved = true;
		return typeName;
	}
	
	public static JavaTypeName fromDotSeparatedString(String s) 
	{
		JavaTypeName typeName = null;

		int separatorIndex = s.lastIndexOf('.');
		if (separatorIndex != -1)
		{
			typeName = new JavaTypeName(s.substring(separatorIndex + 1), JavaDeclName.fromDotSeparatedString(s.substring(0, separatorIndex)));
		}
		else
		{
			typeName = new JavaTypeName(s, null);
		}
		
		return typeName;
	}
	
	public JavaTypeName(String name, JavaDeclName parent)
	{
		m_parent = parent;
		m_name = name;
	}
	
	public JavaTypeName(String name, List<String> typeParameterNames, List<JavaTypeName> typeArguments, JavaDeclName parent)
	{
		m_parent = parent;
		m_name = name;
		m_typeParameterNames = typeParameterNames;
		m_typeArguments = typeArguments;
	}
	
	public JavaDeclName getParent()
	{
		return m_parent;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public JavaDeclName toDeclName()
	{
		JavaDeclName declName = new JavaDeclName(m_name, m_typeParameterNames);
		declName.setParent(m_parent);
		return declName;
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
		
		nameHierarchy.push(new NameElement(m_name + getTypeArgumentString()));
		
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
		string += getTypeArgumentString();
		
		return string;
	}
	
	private String getTypeArgumentString()
	{
		String string = "";
		if (m_typeArguments != null && !m_typeArguments.isEmpty())
		{
			string += "<";
			for (int i = 0; i < m_typeArguments.size(); i++)
			{
				if (i != 0)
				{
					string += ", ";
				}
				string += m_typeArguments.get(i).toString();
			}
			string += ">";
		}
		return string;
	}
}
