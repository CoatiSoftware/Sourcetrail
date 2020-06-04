package com.sourcetrail;

public enum DefinitionKind {	// these values need to be the same as DefinitionKind in C++ code
	NONE(0),
	IMPLICIT(1),
	EXPLICIT(2);

	private final int m_value;

	private DefinitionKind(int value)
	{
		this.m_value = value;
	}

	public int getValue()
	{
		return m_value;
	}
}
