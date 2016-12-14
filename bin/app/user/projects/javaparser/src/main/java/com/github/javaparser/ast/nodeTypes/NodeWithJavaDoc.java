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

import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.comments.JavadocComment;

/**
 * Node which can be documented through a Javadoc comment.
 */
public interface NodeWithJavaDoc<T> {
    /**
     * Gets the JavaDoc for this node. You can set the JavaDoc by calling setComment with a JavadocComment.
     *
     * @return The JavaDoc for this node if it exists, null if it doesn't.
     */
    JavadocComment getJavaDoc();

    /**
     * Use this to store additional information to this node.
     *
     * @param comment to be set
     */
    @SuppressWarnings("unchecked")
    public default T setJavaDocComment(String comment) {
        ((Node) this).setComment(new JavadocComment(comment));
        return (T) this;
    }
}
