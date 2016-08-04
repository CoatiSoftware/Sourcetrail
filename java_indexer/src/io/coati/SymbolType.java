package io.coati;

public enum SymbolType
{ // these values need to be the same as SymbolType in C++ code
	BUILTIN_TYPE(1),
	CLASS(2),
	ENUM(3),
	ENUM_CONSTANT(4),
	FIELD(5),
	FUNCTION(6),
	GLOBAL_VARIABLE(7),
	INTERFACE(8),
	LOCAL_VARIABLE(9),
	MACRO(10),
	METHOD(11),
	NAMESPACE(12),
	PACKAGE(13),
	PARAMETER(14),
	STRUCT(15),
	TEMPLATE_PARAMETER(16),
	TYPEDEF(17),
	TYPE_PARAMETER(18),
	UNION(19),
	TYPE_MAX(20);
	
	private final int m_value;
	
    private SymbolType(int value) 
    {
        this.m_value = value;
    }

    public int getValue() 
    {
        return m_value;
    }
}
