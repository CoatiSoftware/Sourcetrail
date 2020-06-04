package com.sourcetrail;

import org.eclipse.jdt.core.dom.Modifier;

public enum AccessKind {	// these values need to be the same as AccesKind in C++ code
	NONE(0),
	PUBLIC(1),
	PROTECTED(2),
	PRIVATE(3),
	DEFAULT(4),
	TEMPLATE_PARAMETER(5),
	TYPE_PARAMETER(6);

	private final int m_value;

	private AccessKind(int value)
	{
		this.m_value = value;
	}

	public int getValue()
	{
		return m_value;
	}

	public static AccessKind fromModifiers(int modifiers)
	{
		if ((modifiers & Modifier.PUBLIC) != 0)
		{
			return AccessKind.PUBLIC;
		}
		if ((modifiers & Modifier.PROTECTED) != 0)
		{
			return AccessKind.PROTECTED;
		}
		if ((modifiers & Modifier.PRIVATE) != 0)
		{
			return AccessKind.PRIVATE;
		}
		if ((modifiers & Modifier.DEFAULT) != 0)
		{
			return AccessKind.DEFAULT;
		}
		return AccessKind.DEFAULT;
	}
}
