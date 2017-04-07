package com.sourcetrail;

import java.util.List;

public class JavaTypeName 
{
	private JavaDeclName m_parent = null;
	private String m_name = "";
	private List<JavaTypeName> m_typeArgumentNames = null;

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
	
	public JavaTypeName(String name, List<JavaTypeName> typeArgumentNames, JavaDeclName parent)
	{
		m_parent = parent;
		m_name = name;
		m_typeArgumentNames = typeArgumentNames;
	}
	
	public JavaDeclName getParent()
	{
		return m_parent;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public String toSerializedNameHierarchy()
	{
		String nameHierarchy = "";
		if (m_parent != null)
		{
			nameHierarchy = m_parent.toSerializedNameHierarchy();
			nameHierarchy += "\tn";
		}
		
		nameHierarchy += m_name;
		nameHierarchy += getTypeArgumentString();
		nameHierarchy += "\ts\tp";
		
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
		if (m_typeArgumentNames != null && !m_typeArgumentNames.isEmpty())
		{
			string += "<";
			for (int i = 0; i < m_typeArgumentNames.size(); i++)
			{
				if (i != 0)
				{
					string += ", ";
				}
				string += m_typeArgumentNames.get(i).toString();
			}
			string += ">";
		}
		return string;
	}
}
