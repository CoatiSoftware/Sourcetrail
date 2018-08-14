package com.sourcetrail;

public enum SymbolKind
{ // these values need to be the same as SymbolKind in C++ code
	ANNOTATION(1),
	BUILTIN_TYPE(2),
	CLASS(3),
	ENUM(4),
	ENUM_CONSTANT(5),
	FIELD(6),
	FUNCTION(7),
	GLOBAL_VARIABLE(8),
	INTERFACE(9),
	LOCAL_VARIABLE(10),
	MACRO(11),
	METHOD(12),
	NAMESPACE(13),
	PACKAGE(14),
	PARAMETER(15),
	STRUCT(16),
	TEMPLATE_PARAMETER(17),
	TYPEDEF(18),
	TYPE_PARAMETER(19),
	UNION(20),
	TYPE_MAX(21);
	
	private final int m_value;
	
    private SymbolKind(int value) 
    {
        this.m_value = value;
    }

    public int getValue() 
    {
        return m_value;
    }
}
