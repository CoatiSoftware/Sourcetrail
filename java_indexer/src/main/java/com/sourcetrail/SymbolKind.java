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
	MACRO(10),
	METHOD(11),
	MODULE(12),
	NAMESPACE(13),
	PACKAGE(14),
	STRUCT(15),
	TEMPLATE_PARAMETER(16),
	TYPEDEF(17),
	TYPE_PARAMETER(18),
	UNION(19),
	TYPE_MAX(20);
	
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
