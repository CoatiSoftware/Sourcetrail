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

import java.util.List;

import com.github.javaparser.Range;
import com.github.javaparser.ast.nodeTypes.NodeWithArrays;
import com.github.javaparser.ast.nodeTypes.NodeWithType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

import static com.github.javaparser.utils.Utils.ensureNotNull;

/**
 * @author Julio Vilmar Gesser
 */
public final class ArrayCreationExpr extends Expression implements NodeWithType<ArrayCreationExpr>, NodeWithArrays<ArrayCreationExpr> {

    private Type type;

    private int arrayCount;

    private ArrayInitializerExpr initializer;

    private List<Expression> dimensions;

    private List<List<AnnotationExpr>> arraysAnnotations;

    public ArrayCreationExpr() {
    }

    public ArrayCreationExpr(Type type, int arrayCount, ArrayInitializerExpr initializer) {
        setType(type);
        setArrayCount(arrayCount);
        setInitializer(initializer);
        setDimensions(null);
    }

    public ArrayCreationExpr(Range range, Type type, int arrayCount, ArrayInitializerExpr initializer) {
        super(range);
        setType(type);
        setArrayCount(arrayCount);
        setInitializer(initializer);
        setDimensions(null);
    }

    public ArrayCreationExpr(Type type, List<Expression> dimensions, int arrayCount) {
        setType(type);
        setArrayCount(arrayCount);
        setDimensions(dimensions);
        setInitializer(null);
    }

    public ArrayCreationExpr(Range range, Type type, List<Expression> dimensions, int arrayCount) {
        super(range);
        setType(type);
        setArrayCount(arrayCount);
        setDimensions(dimensions);
        setInitializer(null);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    @Override
    public int getArrayCount() {
        return arrayCount;
    }

    public List<Expression> getDimensions() {
        dimensions = ensureNotNull(dimensions);
        return dimensions;
    }

    public ArrayInitializerExpr getInitializer() {
        return initializer;
    }

    @Override
    public Type getType() {
        return type;
    }

    @Override
    public ArrayCreationExpr setArrayCount(int arrayCount) {
        this.arrayCount = arrayCount;
        return this;
    }

    public void setDimensions(List<Expression> dimensions) {
        this.dimensions = dimensions;
		setAsParentNodeOf(this.dimensions);
    }

    public void setInitializer(ArrayInitializerExpr initializer) {
        this.initializer = initializer;
		setAsParentNodeOf(this.initializer);
    }

    @Override
    public ArrayCreationExpr setType(Type type) {
        this.type = type;
		setAsParentNodeOf(this.type);
        return this;
    }

    @Override
    public List<List<AnnotationExpr>> getArraysAnnotations() {
        arraysAnnotations = ensureNotNull(arraysAnnotations);
        return arraysAnnotations;
    }

    @Override
    public ArrayCreationExpr setArraysAnnotations(
            List<List<AnnotationExpr>> arraysAnnotations) {
        this.arraysAnnotations = arraysAnnotations;
        return this;
    }
}
