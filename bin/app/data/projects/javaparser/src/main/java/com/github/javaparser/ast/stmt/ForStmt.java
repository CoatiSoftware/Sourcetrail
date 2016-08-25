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
 
package com.github.javaparser.ast.stmt;

import com.github.javaparser.Range;
import com.github.javaparser.ast.expr.Expression;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

import java.util.List;

import static com.github.javaparser.utils.Utils.*;

/**
 * @author Julio Vilmar Gesser
 */
public final class ForStmt extends Statement {

	private List<Expression> init;

	private Expression compare;

	private List<Expression> update;

	private Statement body;

	public ForStmt() {
	}

	public ForStmt(final List<Expression> init, final Expression compare,
			final List<Expression> update, final Statement body) {
		setCompare(compare);
		setInit(init);
		setUpdate(update);
		setBody(body);
	}

	public ForStmt(Range range,
	               final List<Expression> init, final Expression compare,
	               final List<Expression> update, final Statement body) {
		super(range);
		setCompare(compare);
		setInit(init);
		setUpdate(update);
		setBody(body);
	}

	@Override
	public <R, A> R accept(final GenericVisitor<R, A> v, final A arg) {
		return v.visit(this, arg);
	}

	@Override
	public <A> void accept(final VoidVisitor<A> v, final A arg) {
		v.visit(this, arg);
	}

	public Statement getBody() {
		return body;
	}

	public Expression getCompare() {
		return compare;
	}

	public List<Expression> getInit() {
        init = ensureNotNull(init);
        return init;
	}

	public List<Expression> getUpdate() {
        update = ensureNotNull(update);
        return update;
	}

	public void setBody(final Statement body) {
		this.body = body;
		setAsParentNodeOf(this.body);
	}

	public void setCompare(final Expression compare) {
		this.compare = compare;
		setAsParentNodeOf(this.compare);
	}

	public void setInit(final List<Expression> init) {
		this.init = init;
		setAsParentNodeOf(this.init);
	}

	public void setUpdate(final List<Expression> update) {
		this.update = update;
		setAsParentNodeOf(this.update);
	}
}
