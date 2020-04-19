package com.sourcetrail.name;

import java.util.List;

public class TypeName implements SymbolName
{
	private DeclName m_parent = null;
	private String m_name = "";
	private List<String> m_typeParameterNames = null;
	private List<TypeName> m_typeArguments = null;
	private boolean m_isUnsolved = false;

	public static TypeName unsolved()
	{
		TypeName typeName = new TypeName("unsolved-type", null);
		typeName.m_isUnsolved = true;
		return typeName;
	}

	public static TypeName fromDotSeparatedString(String s)
	{
		TypeName typeName = null;

		int separatorIndex = s.lastIndexOf('.');
		if (separatorIndex != -1)
		{
			typeName = new TypeName(
				s.substring(separatorIndex + 1),
				DeclName.fromDotSeparatedString(s.substring(0, separatorIndex)));
		}
		else
		{
			typeName = new TypeName(s, null);
		}

		return typeName;
	}

	public TypeName(String name, DeclName parent)
	{
		m_parent = parent;
		m_name = name;
	}

	public TypeName(
		String name, List<String> typeParameterNames, List<TypeName> typeArguments, DeclName parent)
	{
		m_parent = parent;
		m_name = name;
		m_typeParameterNames = typeParameterNames;
		m_typeArguments = typeArguments;
	}

	public DeclName getParent()
	{
		return m_parent;
	}

	public String getName()
	{
		return m_name;
	}

	public boolean getIsUnsolved()
	{
		return m_isUnsolved;
	}

	public DeclName toDeclName()
	{
		DeclName declName = new DeclName(m_name, m_typeParameterNames);
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
