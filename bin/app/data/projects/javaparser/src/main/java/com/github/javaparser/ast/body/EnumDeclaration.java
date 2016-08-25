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
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class EnumDeclaration extends TypeDeclaration<EnumDeclaration> {

    private List<ClassOrInterfaceType> implementsList;

    private List<EnumConstantDeclaration> entries;

    public EnumDeclaration() {
    }

    public EnumDeclaration(EnumSet<Modifier> modifiers, String name) {
        super(modifiers, name);
    }

    public EnumDeclaration(EnumSet<Modifier> modifiers, List<AnnotationExpr> annotations, String name,
                           List<ClassOrInterfaceType> implementsList, List<EnumConstantDeclaration> entries,
                           List<BodyDeclaration<?>> members) {
        super(annotations, modifiers, name, members);
        setImplements(implementsList);
        setEntries(entries);
    }

    public EnumDeclaration(Range range, EnumSet<Modifier> modifiers, List<AnnotationExpr> annotations, String name,
                           List<ClassOrInterfaceType> implementsList, List<EnumConstantDeclaration> entries,
                           List<BodyDeclaration<?>> members) {
        super(range, annotations, modifiers, name, members);
        setImplements(implementsList);
        setEntries(entries);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }


    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    public List<EnumConstantDeclaration> getEntries() {
        entries = ensureNotNull(entries);
        return entries;
    }

    public List<ClassOrInterfaceType> getImplements() {
        implementsList = ensureNotNull(implementsList);
        return implementsList;
    }

    public EnumDeclaration setEntries(List<EnumConstantDeclaration> entries) {
        this.entries = entries;
		setAsParentNodeOf(this.entries);
        return this;
    }

    public EnumDeclaration setImplements(List<ClassOrInterfaceType> implementsList) {
        this.implementsList = implementsList;
		setAsParentNodeOf(this.implementsList);
        return this;
    }

    /**
     * Add an implements to this enum
     * 
     * @param name the name of the type to extends from
     * @return this, the {@link EnumDeclaration}
     */
    public EnumDeclaration addImplements(String name) {
        ClassOrInterfaceType classOrInterfaceType = new ClassOrInterfaceType(name);
        getImplements().add(classOrInterfaceType);
        classOrInterfaceType.setParentNode(this);
        return this;
    }

    /**
     * Add an implements to this enum and automatically add the import
     * 
     * @param clazz the type to implements from
     * @return this, the {@link EnumDeclaration}
     */
    public EnumDeclaration addImplements(Class<?> clazz) {
        tryAddImportToParentCompilationUnit(clazz);
        return addImplements(clazz.getSimpleName());
    }

    public EnumConstantDeclaration addEnumConstant(String name) {
        EnumConstantDeclaration enumConstant = new EnumConstantDeclaration(name);
        getEntries().add(enumConstant);
        enumConstant.setParentNode(this);
        return enumConstant;
    }

}
