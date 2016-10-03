package io.coati;

import java.util.List;

public class JavaDeclName 
{
	private JavaDeclName m_parent = null;
	private String m_name = "";
	private List<String> m_typeParameterNames = null;
	private JavaTypeName m_returnTypeName = null;
	private List<JavaTypeName> m_parameterNames = null;
	
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
	
	public JavaDeclName(String name, JavaTypeName returnTypeName, List<JavaTypeName> parameterNames)
	{
		m_name = name;
		m_returnTypeName = returnTypeName;
		m_parameterNames = parameterNames;
	}
	
	public JavaDeclName(String name, List<String> typeParameterNames)
	{
		m_name = name;
		m_typeParameterNames = typeParameterNames;
	}
	public JavaDeclName(String name, List<String> typeParameterNames, JavaTypeName returnTypeName, List<JavaTypeName> parameterNames)
	{
		m_name = name;
		m_typeParameterNames = typeParameterNames;
		m_returnTypeName = returnTypeName;
		m_parameterNames = parameterNames;
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
	
	public String toSerializedNameHierarchy()
	{
		String nameHierarchy = "";
		if (m_parent != null)
		{
			nameHierarchy = m_parent.toSerializedNameHierarchy();
			nameHierarchy += "\tn";
		}
		
		nameHierarchy += m_name;
		nameHierarchy += getTypeParameterString();

		nameHierarchy += "\ts";
		if (m_returnTypeName != null)
		{
			nameHierarchy += m_returnTypeName.toString();
		}
		
		nameHierarchy += "\tp";
		nameHierarchy += getParameterString();
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
	
	private String getParameterString()
	{
		String string = "";
		if (m_parameterNames != null)
		{
			string += "(";
			for (int i = 0; i < m_parameterNames.size(); i++)
			{
				if (i != 0)
				{
					string += ", ";
				}
				string += m_parameterNames.get(i).toString();
			}
			string += ")";
		}
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
