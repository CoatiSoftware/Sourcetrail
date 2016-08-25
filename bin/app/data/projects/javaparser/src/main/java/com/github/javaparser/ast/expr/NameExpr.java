/*
 * Copyright (C) 2007-2010 Júlio Vilmar Gesser.
 * Copyright (C) 2011, 2013-2016 The JavaParser Team.
 *
 * This file is part of JavaParser.
 * 
 * JavaParser can be used either under the terms of
 * a) the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * b) the terms of the Apache License 
 *
 * You should have received a copy of both licenses in LICENCE.LGPL and
 * LICENCE.APACHE. Please refer to those files for details.
 *
 * JavaParser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */
 
package com.github.javaparser.ast.expr;

import com.github.javaparser.Range;
import com.github.javaparser.ast.nodeTypes.NodeWithName;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public class NameExpr extends Expression implements NodeWithName<NameExpr> {

	private String name;

	public NameExpr() {
	}

	public NameExpr(final String name) {
		this.name = name;
	}

	public NameExpr(Range range, final String name) {
		super(range);
		this.name = name;
	}

	@Override public <R, A> R accept(final GenericVisitor<R, A> v, final A arg) {
		return v.visit(this, arg);
	}

	@Override public <A> void accept(final VoidVisitor<A> v, final A arg) {
		v.visit(this, arg);
	}

	@Override
	public final String getName() {
		return name;
	}

    @Override
    public NameExpr setName(final String name) {
		this.name = name;
        return this;
	}


	/**
	 * Creates a new {@link NameExpr} from a qualified name.<br>
	 * The qualified name can contains "." (dot) characters.
	 *
	 * @param qualifiedName
	 *            qualified name
	 * @return instanceof {@link NameExpr}
	 */
	public static NameExpr create(String qualifiedName) {
		String[] split = qualifiedName.split("\\.");
		NameExpr ret = new NameExpr(split[0]);
		for (int i = 1; i < split.length; i++) {
			ret = new QualifiedNameExpr(ret, split[i]);
		}
		return ret;
	}

}
