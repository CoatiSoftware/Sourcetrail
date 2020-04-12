package com.sourcetrail.name;

import com.sourcetrail.Position;
import java.io.File;
import java.util.List;

public class DeclName implements SymbolName
{
	private DeclName m_parent = null;
	private String m_name = "";
	private List<String> m_typeParameterNames = null;
	private boolean m_isUnsolved = false;
	private boolean m_isAnonymous = false;
	private boolean m_isLocal = false;
	private boolean m_isGlobal = false;

	public static DeclName unsolved()
	{
		DeclName declName = new DeclName("unsolved-symbol");
		declName.m_isUnsolved = true;
		return declName;
	}

	public static DeclName anonymousClass(File filePath, int line, int col)
	{
		DeclName declName = new DeclName(
			"anonymous class (" + filePath.getName() + "<" + line + ":" + col + ">)");
		declName.m_isAnonymous = true;
		return declName;
	}

	public static DeclName localSymbol(DeclName methodContextName, int id)
	{
		DeclName declName = new DeclName(methodContextName + "<" + id + ">");
		declName.m_isLocal = true;
		return declName;
	}

	public static DeclName globalSymbol(File fileContext, int id)
	{
		DeclName declName = new DeclName(fileContext.getName() + "<" + id + ">");
		declName.m_isGlobal = true;
		return declName;
	}

	public static DeclName scope(File fileContext, Position begin)
	{
		DeclName declName = new DeclName(
			fileContext.getName() + "<" + begin.line + ":" + begin.column + ">");
		return declName;
	}

	public static DeclName fromDotSeparatedString(String s)
	{
		DeclName declName = null;

		int separatorIndex = s.lastIndexOf('.');
		if (separatorIndex != -1)
		{
			declName = new DeclName(s.substring(separatorIndex + 1));
			declName.setParent(DeclName.fromDotSeparatedString(s.substring(0, separatorIndex)));
		}
		else
		{
			declName = new DeclName(s, null);
		}

		return declName;
	}

	public DeclName(String name)
	{
		m_name = name;
	}

	public DeclName(String name, List<String> typeParameterNames)
	{
		m_name = name;
		m_typeParameterNames = typeParameterNames;
	}

	public void setParent(DeclName parent)
	{
		m_parent = parent;
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

	public boolean getIsAnonymous()
	{
		return m_isAnonymous;
	}

	public boolean getIsLocal()
	{
		return m_isLocal;
	}

	public boolean getIsGlobal()
	{
		return m_isGlobal;
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

	public List<String> getTypeParameterNames()
	{
		return m_typeParameterNames;
	}
}
