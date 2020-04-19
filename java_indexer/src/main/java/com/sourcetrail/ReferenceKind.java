package com.sourcetrail;

public enum ReferenceKind {	   // these values need to be the same as ReferenceKind in C++ code
	UNDEFINED(0),
	TYPE_USAGE(1),
	USAGE(2),
	CALL(3),
	INHERITANCE(4),
	OVERRIDE(5),
	TYPE_ARGUMENT(6),
	TEMPLATE_SPECIALIZATION(7),
	INCLUDE(8),
	IMPORT(9),
	MACRO_USAGE(10),
	ANNOTATION_USAGE(11);

	private final int m_value;

	private ReferenceKind(int value)
	{
		this.m_value = value;
	}

	public int getValue()
	{
		return m_value;
	}
}
