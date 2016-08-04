package io.coati;

import java.util.List;

import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.UnknownType;

public class CallableConstructorDecl implements CallableDecl
{
	private ConstructorDeclaration m_decl;
	
	public CallableConstructorDecl(ConstructorDeclaration decl)
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
		return new UnknownType();
	}
}
