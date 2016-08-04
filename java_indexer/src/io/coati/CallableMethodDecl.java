package io.coati;

import java.util.List;

import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.type.Type;

public class CallableMethodDecl implements CallableDecl
{
	private MethodDeclaration m_decl;
	
	public CallableMethodDecl(MethodDeclaration decl)
	{
		m_decl = decl;
	}
	
	public BodyDeclaration getWrappedNode()
	{
		return m_decl;
	}
	
	public String getName()
	{
		return m_decl.getName();
	}

	public List<TypeParameter> getTypeParameters()
	{
		return m_decl.getTypeParameters();
	}
	
	public List<Parameter> getParameters()
	{
		return m_decl.getParameters();
	}
	
	public Type getType()
	{
		return m_decl.getType();
	}
}
