package io.coati;

import com.github.javaparser.ast.AccessSpecifier;

public enum AccessKind 
{ // these values need to be the same as AccesType in C++ code
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
    
    public static AccessKind fromAccessSpecifier(AccessSpecifier specifier)
    {
		switch (specifier)
		{
		case PUBLIC:
			return AccessKind.PUBLIC;
		case PROTECTED:
			return AccessKind.PROTECTED;
		case PRIVATE:
			return AccessKind.PRIVATE;
		case DEFAULT:
			return AccessKind.DEFAULT;
		default:
			return AccessKind.NONE;
		}
    }
}

