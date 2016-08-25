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

import static com.github.javaparser.utils.Utils.ensureNotNull;

import java.util.List;

import com.github.javaparser.Range;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.expr.Expression;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.nodeTypes.NodeWithJavaDoc;
import com.github.javaparser.ast.nodeTypes.NodeWithName;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class EnumConstantDeclaration extends BodyDeclaration<EnumConstantDeclaration>
        implements NodeWithJavaDoc<EnumConstantDeclaration>, NodeWithName<EnumConstantDeclaration> {

    private String name;

    private List<Expression> args;

    private List<BodyDeclaration<?>> classBody;

    public EnumConstantDeclaration() {
    }

    public EnumConstantDeclaration(String name) {
        setName(name);
    }

    public EnumConstantDeclaration(List<AnnotationExpr> annotations, String name, List<Expression> args,
                                   List<BodyDeclaration<?>> classBody) {
        super(annotations);
        setName(name);
        setArgs(args);
        setClassBody(classBody);
    }

    public EnumConstantDeclaration(Range range, List<AnnotationExpr> annotations, String name, List<Expression> args,
                                   List<BodyDeclaration<?>> classBody) {
        super(range, annotations);
        setName(name);
        setArgs(args);
        setClassBody(classBody);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    public List<Expression> getArgs() {
        args = ensureNotNull(args);
        return args;
    }

    public List<BodyDeclaration<?>> getClassBody() {
        classBody = ensureNotNull(classBody);
        return classBody;
    }

    @Override
    public String getName() {
        return name;
    }

    public void setArgs(List<Expression> args) {
        this.args = args;
		setAsParentNodeOf(this.args);
    }

    public void setClassBody(List<BodyDeclaration<?>> classBody) {
        this.classBody = classBody;
		setAsParentNodeOf(this.classBody);
    }

    @Override
    public EnumConstantDeclaration setName(String name) {
        this.name = name;
        return this;
    }

    @Override
    public JavadocComment getJavaDoc() {
        if(getComment() instanceof JavadocComment){
            return (JavadocComment) getComment();
        }
        return null;
    }

    public EnumConstantDeclaration addArgument(String valueExpr) {
        getArgs().add(NameExpr.create(valueExpr));
        return this;
    }
}
