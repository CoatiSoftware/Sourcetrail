package io.coati;

import java.util.List;

import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.type.Type;

public interface CallableDecl
{
	public BodyDeclaration getWrappedNode();
	public String getName();
	public List<TypeParameter> getTypeParameters();
	public List<Parameter> getParameters();
	public Type getType();
}
