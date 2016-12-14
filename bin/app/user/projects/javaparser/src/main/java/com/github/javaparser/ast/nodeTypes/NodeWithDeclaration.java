/*
 * Copyright (C) 2007-2010 Júlio Vilmar Gesser.
 * Copyright (C) 2011, 2013-2015 The JavaParser Team.
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
 
package com.github.javaparser.ast.nodeTypes;

/**
 * Element with a declaration representable as a String.
 *
 * @author Federico Tomassetti
 * @since July 2014
 */
public interface NodeWithDeclaration {

    /**
     * As {@link NodeWithDeclaration#getDeclarationAsString(boolean, boolean, boolean)} including
     * the modifiers, the throws clause and the parameters with both type and name.
     * @return String representation of declaration
     */
    String getDeclarationAsString();

    /**
     * As {@link NodeWithDeclaration#getDeclarationAsString(boolean, boolean, boolean)} including
     * the parameters with both type and name.
     * @param includingModifiers flag to include the modifiers (if present) in the string produced
     * @param includingThrows flag to include the throws clause (if present) in the string produced
     * @return String representation of declaration based on parameter flags
     */
    String getDeclarationAsString(boolean includingModifiers, boolean includingThrows);

    /**
     * A simple representation of the element declaration.
     * It should fit one string.
     * @param includingModifiers flag to include the modifiers (if present) in the string produced
     * @param includingThrows flag to include the throws clause (if present) in the string produced
     * @param includingParameterName flag to include the parameter name (while the parameter type is always included) in the string produced
     * @return String representation of declaration based on parameter flags
     */
    String getDeclarationAsString(boolean includingModifiers, boolean includingThrows, boolean includingParameterName);
}
