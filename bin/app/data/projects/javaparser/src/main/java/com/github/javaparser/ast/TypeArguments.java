/*
 * Copyright (C) 2016 The JavaParser Team.
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

import com.github.javaparser.ast.type.Type;

import java.util.Collections;
import java.util.List;

import static com.github.javaparser.utils.Utils.ensureNotNull;

public class TypeArguments {
    public static final TypeArguments EMPTY = withArguments(Collections.<Type>emptyList());

    private final List<Type> typeArguments;
    private final boolean usesDiamondOperator;

    private TypeArguments(List<Type> typeArguments, boolean usesDiamondOperator) {
        this.typeArguments = ensureNotNull(typeArguments);
        this.usesDiamondOperator = usesDiamondOperator;
    }

    public List<Type> getTypeArguments() {
        return typeArguments;
    }

    public boolean isUsingDiamondOperator() {
        return usesDiamondOperator;
    }

    public static TypeArguments withDiamondOperator() {
        return new TypeArguments(Collections.<Type>emptyList(), true);
    }

    public static TypeArguments withArguments(List<Type> typeArguments) {
        return new TypeArguments(typeArguments, false);
    }
}