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

import java.util.EnumSet;
import java.util.List;

import com.github.javaparser.Range;
import com.github.javaparser.ast.Modifier;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class ClassOrInterfaceDeclaration extends TypeDeclaration<ClassOrInterfaceDeclaration> {

    private boolean interface_;

    private List<TypeParameter> typeParameters;

    // Can contain more than one item if this is an interface
    private List<ClassOrInterfaceType> extendsList;

    private List<ClassOrInterfaceType> implementsList;

    public ClassOrInterfaceDeclaration() {
    }

    public ClassOrInterfaceDeclaration(final EnumSet<Modifier> modifiers, final boolean isInterface,
                                       final String name) {
        super(modifiers, name);
        setInterface(isInterface);
    }

    public ClassOrInterfaceDeclaration(final EnumSet<Modifier> modifiers,
                                       final List<AnnotationExpr> annotations, final boolean isInterface,
                                       final String name,
                                       final List<TypeParameter> typeParameters,
                                       final List<ClassOrInterfaceType> extendsList,
                                       final List<ClassOrInterfaceType> implementsList,
                                       final List<BodyDeclaration<?>> members) {
        super(annotations, modifiers, name, members);
        setInterface(isInterface);
        setTypeParameters(typeParameters);
        setExtends(extendsList);
        setImplements(implementsList);
    }

    public ClassOrInterfaceDeclaration(Range range, final EnumSet<Modifier> modifiers,
                                       final List<AnnotationExpr> annotations, final boolean isInterface,
                                       final String name,
                                       final List<TypeParameter> typeParameters,
                                       final List<ClassOrInterfaceType> extendsList,
                                       final List<ClassOrInterfaceType> implementsList,
                                       final List<BodyDeclaration<?>> members) {
        super(range, annotations, modifiers, name, members);
        setInterface(isInterface);
        setTypeParameters(typeParameters);
        setExtends(extendsList);
        setImplements(implementsList);
    }

    @Override
    public <R, A> R accept(final GenericVisitor<R, A> v, final A arg) {
        return v.visit(this, arg);
    }

    @Override
    public <A> void accept(final VoidVisitor<A> v, final A arg) {
        v.visit(this, arg);
    }

    public List<ClassOrInterfaceType> getExtends() {
        extendsList = ensureNotNull(extendsList);
        return extendsList;
    }

    public List<ClassOrInterfaceType> getImplements() {
        implementsList = ensureNotNull(implementsList);
        return implementsList;
    }

    public List<TypeParameter> getTypeParameters() {
        typeParameters = ensureNotNull(typeParameters);
        return typeParameters;
    }

    public boolean isInterface() {
        return interface_;
    }

    /**
     * 
     * @param extendsList a null value is currently treated as an empty list. This behavior could change
     *            in the future, so please avoid passing null
     */
    public void setExtends(final List<ClassOrInterfaceType> extendsList) {
        this.extendsList = extendsList;
        setAsParentNodeOf(this.extendsList);
    }

    /**
     * 
     * @param implementsList a null value is currently treated as an empty list. This behavior could change
     *            in the future, so please avoid passing null
     */
    public void setImplements(final List<ClassOrInterfaceType> implementsList) {
        this.implementsList = implementsList;
        setAsParentNodeOf(this.implementsList);
    }

    public void setInterface(final boolean interface_) {
        this.interface_ = interface_;
    }

    /**
     *
     * @param typeParameters a null value is currently treated as an empty list. This behavior could change
     *            in the future, so please avoid passing null
     */
    public void setTypeParameters(final List<TypeParameter> typeParameters) {
        this.typeParameters = typeParameters;
        setAsParentNodeOf(this.typeParameters);
    }

    /**
     * Add an extends to this class or interface and automatically add the import
     * 
     * @param clazz the class to extand from
     * @return this, the {@link ClassOrInterfaceDeclaration}
     */
    public ClassOrInterfaceDeclaration addExtends(Class<?> clazz) {
        tryAddImportToParentCompilationUnit(clazz);
        return addExtends(clazz.getSimpleName());
    }

    /**
     * Add an extends to this class or interface
     * 
     * @param name the name of the type to extends from
     * @return this, the {@link ClassOrInterfaceDeclaration}
     */
    public ClassOrInterfaceDeclaration addExtends(String name) {
        ClassOrInterfaceType classOrInterfaceType = new ClassOrInterfaceType(name);
        getExtends().add(classOrInterfaceType);
        classOrInterfaceType.setParentNode(this);
        return this;
    }

    /**
     * Add an implements to this class or interface
     * 
     * @param name the name of the type to extends from
     * @return this, the {@link ClassOrInterfaceDeclaration}
     */
    public ClassOrInterfaceDeclaration addImplements(String name) {
        ClassOrInterfaceType classOrInterfaceType = new ClassOrInterfaceType(name);
        getImplements().add(classOrInterfaceType);
        classOrInterfaceType.setParentNode(this);
        return this;
    }

    /**
     * Add an implements to this class or interface and automatically add the import
     * 
     * @param clazz the type to implements from
     * @return this, the {@link ClassOrInterfaceDeclaration}
     */
    public ClassOrInterfaceDeclaration addImplements(Class<?> clazz) {
        tryAddImportToParentCompilationUnit(clazz);
        return addImplements(clazz.getSimpleName());
    }


}
