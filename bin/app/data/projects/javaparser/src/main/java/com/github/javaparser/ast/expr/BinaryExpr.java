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
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class BinaryExpr extends Expression {

    public enum Operator {
        or, // ||
        and, // &&
        binOr, // |
        binAnd, // &
        xor, // ^
        equals, // ==
        notEquals, // !=
        less, // <
        greater, // >
        lessEquals, // <=
        greaterEquals, // >=
        lShift, // <<
        rSignedShift, // >>
        rUnsignedShift, // >>>
        plus, // +
        minus, // -
        times, // *
        divide, // /
        remainder, // %
    }

    private Expression left;

    private Expression right;

    private Operator op;

    public BinaryExpr() {
    }

    public BinaryExpr(Expression left, Expression right, Operator op) {
    	setLeft(left);
    	setRight(right);
    	setOperator(op);
    }

    public BinaryExpr(Range range, Expression left, Expression right, Operator op) {
        super(range);
    	setLeft(left);
    	setRight(right);
    	setOperator(op);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    public Expression getLeft() {
        return left;
    }

    public Operator getOperator() {
        return op;
    }

    public Expression getRight() {
        return right;
    }

    public void setLeft(Expression left) {
        this.left = left;
		setAsParentNodeOf(this.left);
    }

    public void setOperator(Operator op) {
        this.op = op;
    }

    public void setRight(Expression right) {
        this.right = right;
		setAsParentNodeOf(this.right);
    }
}
