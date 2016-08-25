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

import static com.github.javaparser.utils.Utils.ensureNotNull;

import java.util.List;

import com.github.javaparser.Range;
import com.github.javaparser.ast.expr.Expression;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class BlockStmt extends Statement {

	private List<Statement> stmts;

	public BlockStmt() {
	}

	public BlockStmt(final List<Statement> stmts) {
		setStmts(stmts);
	}

	public BlockStmt(final Range range, final List<Statement> stmts) {
		super(range);
		setStmts(stmts);
	}
	
	@Override
	public <R, A> R accept(final GenericVisitor<R, A> v, final A arg) {
		return v.visit(this, arg);
	}

	@Override
	public <A> void accept(final VoidVisitor<A> v, final A arg) {
		v.visit(this, arg);
	}

	public List<Statement> getStmts() {
        stmts = ensureNotNull(stmts);
        return stmts;
	}

	public void setStmts(final List<Statement> stmts) {
		this.stmts = stmts;
		setAsParentNodeOf(this.stmts);
	}

    // TODO move to a nodeType + addAndGetStatement like methods ?
    public BlockStmt addStatement(Statement statement) {
        getStmts().add(statement);
        statement.setParentNode(this);
        return this;
    }
	
    public BlockStmt addStatement(String statement) {
        return addStatement(new ExpressionStmt(new NameExpr(statement)));
    }

	public BlockStmt addStatement(Expression expr) {
		return addStatement(new ExpressionStmt(expr));
	}


}
