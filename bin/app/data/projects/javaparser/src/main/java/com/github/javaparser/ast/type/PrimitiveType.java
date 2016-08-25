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
 
package com.github.javaparser.ast.type;

import java.util.HashMap;

import com.github.javaparser.Range;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

/**
 * @author Julio Vilmar Gesser
 */
public final class PrimitiveType extends Type {
	
	public static final PrimitiveType BYTE_TYPE = new PrimitiveType(Primitive.Byte);

	public static final PrimitiveType SHORT_TYPE = new PrimitiveType(Primitive.Short);

	public static final PrimitiveType INT_TYPE = new PrimitiveType(Primitive.Int);

	public static final PrimitiveType LONG_TYPE = new PrimitiveType(Primitive.Long);

	public static final PrimitiveType FLOAT_TYPE = new PrimitiveType(Primitive.Float);

	public static final PrimitiveType DOUBLE_TYPE = new PrimitiveType(Primitive.Double);

	public static final PrimitiveType BOOLEAN_TYPE = new PrimitiveType(Primitive.Boolean);

	public static final PrimitiveType CHAR_TYPE = new PrimitiveType(Primitive.Char);

	public enum Primitive {
		Boolean ("Boolean"),
		Char    ("Character"),
		Byte    ("Byte"),
		Short   ("Short"),
		Int     ("Integer"),
		Long    ("Long"),
		Float   ("Float"),
		Double  ("Double");

		final String nameOfBoxedType;

		public ClassOrInterfaceType toBoxedType() {
			return new ClassOrInterfaceType(nameOfBoxedType);
		}

		Primitive(String nameOfBoxedType) {
			this.nameOfBoxedType = nameOfBoxedType;
		}
	}

	static final HashMap<String, Primitive> unboxMap = new HashMap<String, Primitive>();
	static {
		for(Primitive unboxedType : Primitive.values()) {
			unboxMap.put(unboxedType.nameOfBoxedType, unboxedType);
		}
	}

	private Primitive type;

	public PrimitiveType() {
	}

	public PrimitiveType(final Primitive type) {
		this.type = type;
	}

	public PrimitiveType(Range range, final Primitive type) {
		super(range);
		this.type = type;
	}

	@Override public <R, A> R accept(final GenericVisitor<R, A> v, final A arg) {
		return v.visit(this, arg);
	}

	@Override public <A> void accept(final VoidVisitor<A> v, final A arg) {
		v.visit(this, arg);
	}

	public Primitive getType() {
		return type;
	}

	public ClassOrInterfaceType toBoxedType() {
		return type.toBoxedType();
	}

	public void setType(final Primitive type) {
		this.type = type;
	}

}
