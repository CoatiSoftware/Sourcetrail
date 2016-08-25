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
import static com.github.javaparser.utils.Utils.isNullOrEmpty;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import com.github.javaparser.Range;
import com.github.javaparser.ast.Modifier;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.nodeTypes.NodeWithJavaDoc;
import com.github.javaparser.ast.nodeTypes.NodeWithMembers;
import com.github.javaparser.ast.nodeTypes.NodeWithModifiers;
import com.github.javaparser.ast.nodeTypes.NodeWithName;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class TypeDeclaration<T> extends BodyDeclaration<T>
        implements NodeWithName<T>, NodeWithJavaDoc<T>, NodeWithModifiers<T>, NodeWithMembers<T> {

	private NameExpr name;

    private EnumSet<Modifier> modifiers = EnumSet.noneOf(Modifier.class);

    private List<BodyDeclaration<?>> members;

	public TypeDeclaration() {
	}

    public TypeDeclaration(EnumSet<Modifier> modifiers, String name) {
		setName(name);
		setModifiers(modifiers);
	}

	public TypeDeclaration(List<AnnotationExpr> annotations,
                           EnumSet<Modifier> modifiers, String name,
                           List<BodyDeclaration<?>> members) {
		super(annotations);
		setName(name);
		setModifiers(modifiers);
		setMembers(members);
	}

	public TypeDeclaration(Range range, List<AnnotationExpr> annotations,
                           EnumSet<Modifier> modifiers, String name,
                           List<BodyDeclaration<?>> members) {
		super(range, annotations);
		setName(name);
		setModifiers(modifiers);
		setMembers(members);
	}

	/**
	 * Adds the given declaration to the specified type. The list of members
	 * will be initialized if it is <code>null</code>.
	 *
	 * @param decl
	 *            member declaration
	 */
	public TypeDeclaration<T> addMember(BodyDeclaration<?> decl) {
		List<BodyDeclaration<?>> members = getMembers();
		if (isNullOrEmpty(members)) {
			members = new ArrayList<>();
			setMembers(members);
		}
		members.add(decl);
		decl.setParentNode(this);
		return this;
	}

    @Override
    public List<BodyDeclaration<?>> getMembers() {
        	members = ensureNotNull(members);
        	return members;
	}

	/**
     * Return the modifiers of this type declaration.
     * 
     * @see Modifier
     * @return modifiers
     */
	@Override
    public final EnumSet<Modifier> getModifiers() {
		return modifiers;
	}

	@Override
	public final String getName() {
		return name.getName();
	}

    @SuppressWarnings("unchecked")
    @Override
    public T setMembers(List<BodyDeclaration<?>> members) {
		this.members = members;
		setAsParentNodeOf(this.members);
        return (T) this;
	}

    @SuppressWarnings("unchecked")
    @Override
    public T setModifiers(EnumSet<Modifier> modifiers) {
		this.modifiers = modifiers;
        return (T) this;
	}

    @Override
    @SuppressWarnings("unchecked")
    public T setName(String name) {
		setNameExpr(new NameExpr(name));
        return (T) this;
	}

    @SuppressWarnings("unchecked")
    public T setNameExpr(NameExpr nameExpr) {
		this.name = nameExpr;
		setAsParentNodeOf(this.name);
        return (T) this;
	}

	public final NameExpr getNameExpr() {
		return name;
	}

	@Override
	public JavadocComment getJavaDoc() {
		if(getComment() instanceof JavadocComment){
			return (JavadocComment) getComment();
		}
		return null;
	}

}
