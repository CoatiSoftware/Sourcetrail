package com.sourcetrail;

import java.util.List;

import com.github.javaparser.ast.type.TypeParameter;
import com.github.javaparser.ast.NodeList;
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
		return m_decl.getNameAsString();
	}
	
	public NodeList<TypeParameter> getTypeParameters()
	{
		return m_decl.getTypeParameters();
	}
	
	public NodeList<Parameter> getParameters()
	{
		return m_decl.getParameters();
	}
	
	public Type getType()
	{
		return new UnknownType();
	}
}
