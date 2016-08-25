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

import static com.github.javaparser.utils.Utils.ensureNotNull;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import com.github.javaparser.Range;
import com.github.javaparser.ast.Modifier;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.nodeTypes.NodeWithAnnotations;
import com.github.javaparser.ast.nodeTypes.NodeWithModifiers;
import com.github.javaparser.ast.nodeTypes.NodeWithType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class VariableDeclarationExpr extends Expression
        implements NodeWithType<VariableDeclarationExpr>, NodeWithModifiers<VariableDeclarationExpr>,
        NodeWithAnnotations<VariableDeclarationExpr> {

    private EnumSet<Modifier> modifiers = EnumSet.noneOf(Modifier.class);

	private List<AnnotationExpr> annotations;

	private Type type;

	private List<VariableDeclarator> vars;

	public VariableDeclarationExpr() {
	}

	public VariableDeclarationExpr(final Type type, final List<VariableDeclarator> vars) {
		setType(type);
		setVars(vars);
	}

    public VariableDeclarationExpr(final EnumSet<Modifier> modifiers, final Type type,
                                   final List<VariableDeclarator> vars) {
		setModifiers(modifiers);
		setType(type);
		setVars(vars);
	}

	public VariableDeclarationExpr(final Range range,
                                   final EnumSet<Modifier> modifiers, final List<AnnotationExpr> annotations,
                                   final Type type,
			final List<VariableDeclarator> vars) {
		super(range);
		setModifiers(modifiers);
		setAnnotations(annotations);
		setType(type);
		setVars(vars);
	}

	/**
	 * Creates a {@link VariableDeclarationExpr}.
	 *
	 * @return instance of {@link VariableDeclarationExpr}
	 */
	public static VariableDeclarationExpr create(Type type, String name) {
		List<VariableDeclarator> vars = new ArrayList<>();
		vars.add(new VariableDeclarator(new VariableDeclaratorId(name)));
		return new VariableDeclarationExpr(type, vars);
	}

	@Override public <R, A> R accept(final GenericVisitor<R, A> v, final A arg) {
		return v.visit(this, arg);
	}

	@Override public <A> void accept(final VoidVisitor<A> v, final A arg) {
		v.visit(this, arg);
	}

	@Override
    public List<AnnotationExpr> getAnnotations() {
        annotations = ensureNotNull(annotations);
        return annotations;
	}

	/**
     * Return the modifiers of this variable declaration.
     * 
     * @see Modifier
     * @return modifiers
     */
	@Override
    public EnumSet<Modifier> getModifiers() {
		return modifiers;
	}

	@Override
	public Type getType() {
		return type;
	}

	public List<VariableDeclarator> getVars() {
        vars = ensureNotNull(vars);
        return vars;
	}

	@Override
    public VariableDeclarationExpr setAnnotations(final List<AnnotationExpr> annotations) {
        this.annotations = annotations;
		setAsParentNodeOf(this.annotations);
        return this;
	}

    @Override
    public VariableDeclarationExpr setModifiers(final EnumSet<Modifier> modifiers) {
		this.modifiers = modifiers;
        return this;
	}

	@Override
    public VariableDeclarationExpr setType(final Type type) {
		this.type = type;
		setAsParentNodeOf(this.type);
        return this;
	}

	public void setVars(final List<VariableDeclarator> vars) {
		this.vars = vars;
		setAsParentNodeOf(this.vars);
	}
}
