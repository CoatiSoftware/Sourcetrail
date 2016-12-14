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
 
package com.github.javaparser.ast;

import com.github.javaparser.Range;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.utils.Utils;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * <p>
 * This class represents the package declaration. The package declaration is
 * optional for the {@link CompilationUnit}.
 * </p>
 * The PackageDeclaration is constructed following the syntax:<br>
 * <pre>
 * {@code
 * PackageDeclaration ::= ( }{@link AnnotationExpr}{@code )* "package" }{@link NameExpr}{@code ) ";"
 * }
 * </pre>
 * @author Julio Vilmar Gesser
 */
public final class PackageDeclaration extends Node {

    private List<AnnotationExpr> annotations;

    private NameExpr name;

    public PackageDeclaration() {
    }

    public PackageDeclaration(NameExpr name) {
        setName(name);
    }

    public PackageDeclaration(List<AnnotationExpr> annotations, NameExpr name) {
        setAnnotations(annotations);
        setName(name);
    }

    public PackageDeclaration(Range range, List<AnnotationExpr> annotations, NameExpr name) {
        super(range);
        setAnnotations(annotations);
        setName(name);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    /**
     * Retrieves the list of annotations declared before the package
     * declaration. Return <code>null</code> if there are no annotations.
     * 
     * @return list of annotations or <code>null</code>
     */
    public List<AnnotationExpr> getAnnotations() {
        annotations = Utils.ensureNotNull(annotations);
        return annotations;
    }

    /**
     * Return the name expression of the package.
     *
     * @return the name of the package
     */
    public NameExpr getName() {
        return name;
    }

    /**
     * Get full package name.
     */
    public String getPackageName() {
        return name.toString();
    }

    /**
     * @param annotations
     *            the annotations to set
     */
    public void setAnnotations(List<AnnotationExpr> annotations) {
        this.annotations = annotations;
        setAsParentNodeOf(this.annotations);
    }

    /**
     * Sets the name of this package declaration.
     * 
     * @param name
     *            the name to set
     */
    public void setName(NameExpr name) {
        this.name = name;
        setAsParentNodeOf(this.name);
    }

}
