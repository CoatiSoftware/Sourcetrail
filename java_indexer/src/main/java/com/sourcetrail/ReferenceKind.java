package com.sourcetrail;

public enum ReferenceKind
{ // these values need to be the same as ReferenceKind in C++ code
	UNDEFINED(0),
	TYPE_USAGE(1),
	USAGE(2),
	CALL(3),
	INHERITANCE(4),
	OVERRIDE(5),
	TEMPLATE_ARGUMENT(6),
	TYPE_ARGUMENT(7),
	TEMPLATE_DEFAULT_ARGUMENT(8),
	TEMPLATE_SPECIALIZATION_OF(9),
	TEMPLATE_MEMBER_SPECIALIZATION_OF(10),
	INCLUDE(11),
	IMPORT(12),
	MACRO_USAGE(13),
	ANNOTATION_USAGE(14);
	
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
