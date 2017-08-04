package com.sourcetrail.name;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class JavaFunctionDeclName extends JavaDeclName
{
	private JavaTypeName m_returnTypeName = null;
	private List<JavaTypeName> m_parameterNames = new ArrayList<>();
	private boolean m_isStatic = false;
	
	public JavaFunctionDeclName(String name, JavaTypeName returnTypeName, List<JavaTypeName> parameterNames, boolean isStatic)
	{
		super(name);
		
		m_returnTypeName = returnTypeName;
		if (m_parameterNames != null) m_parameterNames = parameterNames;
		m_isStatic = isStatic;
	}
	
	public JavaFunctionDeclName(String name, List<String> typeParameterNames, JavaTypeName returnTypeName, List<JavaTypeName> parameterNames, boolean isStatic)
	{
		super(name, typeParameterNames);
		
		m_returnTypeName = returnTypeName;
		m_parameterNames = parameterNames;
		m_isStatic = isStatic;
	}
	
	@Override
	public NameHierarchy toNameHierarchy()
	{
		String prefix = "";
		if (m_isStatic)
		{
			prefix += "static ";
		}
		if (m_returnTypeName != null)
		{
			prefix += m_returnTypeName.toString();
		}
		
		String postfix = getParameterString();
		
		NameHierarchy nameHierarchy = super.toNameHierarchy();
		
		Optional<NameElement> nameElement = nameHierarchy.peek();
		if (nameElement.isPresent())
		{
			String name = nameElement.get().getName();
			
			nameHierarchy.pop();
			nameHierarchy.push(new NameElement(name, prefix, postfix));
		}
		
		return nameHierarchy;
	}
	
	private String getParameterString()
	{
		String string = "(";
		if (m_parameterNames != null)
		{
			for (int i = 0; i < m_parameterNames.size(); i++)
			{
				if (i != 0)
				{
					string += ", ";
				}
				string += m_parameterNames.get(i).toString();
			}
		}
		string += ")";
		return string;
	}
}
