package com.sourcetrail.name;

import java.io.File;

public class JavaFileName implements JavaSymbolName
{
	private File m_filePath = null;

	public JavaFileName(File filePath)
	{
		m_filePath = filePath;
	}

	@Override
	public NameHierarchy toNameHierarchy() 
	{
		NameHierarchy nameHierarchy;
		if (m_filePath != null)
		{
			nameHierarchy = new NameHierarchy(m_filePath.toString().replaceAll("\\\\", "/"));
		}
		else
		{
			nameHierarchy = new NameHierarchy("unsolved-file");
		}
		nameHierarchy.setSeparator('/');
		return nameHierarchy;
	}
}
