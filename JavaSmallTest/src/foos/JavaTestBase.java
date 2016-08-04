package foos;

public interface JavaTestBaseBase
{	
	default void bar()
	{
	}
}

public interface JavaTestBase extends JavaTestBaseBase
{	
	
}