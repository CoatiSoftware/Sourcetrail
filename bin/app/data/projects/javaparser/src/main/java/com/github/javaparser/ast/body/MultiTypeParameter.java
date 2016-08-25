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
 
package com.github.javaparser.ast.body;

import java.util.List;

import java.util.EnumSet;

import com.github.javaparser.Range;
import com.github.javaparser.ast.Modifier;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.type.UnionType;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

public class MultiTypeParameter extends BaseParameter<MultiTypeParameter> {
    private UnionType type;
	
    public MultiTypeParameter() {}

    public MultiTypeParameter(EnumSet<Modifier> modifiers, List<AnnotationExpr> annotations, UnionType type, VariableDeclaratorId id) {
        super(modifiers, annotations, id);
        this.type = type;
    }

    public MultiTypeParameter(Range range, EnumSet<Modifier> modifiers, List<AnnotationExpr> annotations, UnionType type, VariableDeclaratorId id) {
        super(range, modifiers, annotations, id);
        this.type = type;
	}

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }
    
    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    public UnionType getType() {
        return type;
    }

    public void setType(UnionType type) {
        this.type = type;
    }
}
