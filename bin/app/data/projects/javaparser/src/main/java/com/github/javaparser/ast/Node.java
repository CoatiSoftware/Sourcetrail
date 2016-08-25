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

import com.github.javaparser.Position;
import com.github.javaparser.Range;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.Comment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.visitor.*;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

/**
 * Abstract class for all nodes of the AST.
 *
 * Each Node can have one associated comment which describe it and
 * a number of "orphan comments" which it contains but are not specifically
 * associated to any element.
 * 
 * @author Julio Vilmar Gesser
 */
public abstract class Node implements Cloneable {
    private Range range;

    private Node parentNode;

    private List<Node> childrenNodes = new LinkedList<>();
    private List<Comment> orphanComments = new LinkedList<>();

    /**
     * This attribute can store additional information from semantic analysis.
     */
    private Object data;

    private Comment comment;

    public Node() {
        this(Range.UNKNOWN);
    }

    public Node(Range range) {
        this.range = range;
    }

    /**
     * Accept method for visitor support.
     * 
     * @param <R>
     *            the type the return value of the visitor
     * @param <A>
     *            the type the argument passed to the visitor
     * @param v
     *            the visitor implementation
     * @param arg
     *            the argument passed to the visitor
     * @return the result of the visit
     */
    public abstract <R, A> R accept(GenericVisitor<R, A> v, A arg);

    /**
     * Accept method for visitor support.
     * 
     * @param <A>
     *            the type the argument passed for the visitor
     * @param v
     *            the visitor implementation
     * @param arg
     *            any value relevant for the visitor
     */
    public abstract <A> void accept(VoidVisitor<A> v, A arg);

    /**
     * This is a comment associated with this node.
     *
     * @return comment property
     */
    public final Comment getComment() {
        return comment;
    }

    /**
     * Use this to retrieve additional information associated to this node.
     *
     * @return data property
     */
    public final Object getData() {
        return data;
    }

    /**
     * The begin position of this node in the source file.
     */
    public Position getBegin() {
        return range.begin;
    }

    /**
     * The end position of this node in the source file.
     */
    public Position getEnd() {
        return range.end;
    }

    /**
     * Sets the begin position of this node in the source file.
     */
    public void setBegin(Position begin) {
        range = range.withBegin(begin);
    }

    /**
     * Sets the end position of this node in the source file.
     */
    public void setEnd(Position end) {
        range = range.withEnd(end);
    }

    /**
     * @return the range of characters in the source code that this node covers.
     */
    public Range getRange() {
        return range;
    }

    /**
     * @param range the range of characters in the source code that this node covers.
     */
    public void setRange(Range range) {
        this.range = range;
    }

    /**
     * Use this to store additional information to this node.
     *
     * @param comment to be set
     */
    public final void setComment(final Comment comment) {
        if (comment != null && (this instanceof Comment)) {
            throw new RuntimeException("A comment can not be commented");
        }
        if (this.comment != null) {
            this.comment.setCommentedNode(null);
        }
        this.comment = comment;
        if (comment != null) {
            this.comment.setCommentedNode(this);
        }
    }



    /**
     * Use this to store additional information to this node.
     *
     * @param comment to be set
     */
    public final void setLineComment(String comment) {
        setComment(new LineComment(comment));
    }

    /**
     * Use this to store additional information to this node.
     *
     * @param comment to be set
     */
    public final void setBlockComment(String comment) {
        setComment(new BlockComment(comment));
    }

    /**
     * Use this to store additional information to this node.
     *
     * @param data to be set
     */
    public final void setData(final Object data) {
        this.data = data;
    }

    /**
     * Return the String representation of this node.
     * 
     * @return the String representation of this node
     */
    @Override
    public final String toString() {
        final DumpVisitor visitor = new DumpVisitor();
        accept(visitor, null);
        return visitor.getSource();
    }

    public final String toStringWithoutComments() {
        final DumpVisitor visitor = new DumpVisitor(false);
        accept(visitor, null);
        return visitor.getSource();
    }

    @Override
    public final int hashCode() {
        return toString().hashCode();
    }

    @Override
    public boolean equals(final Object obj) {
        if (obj == null || !(obj instanceof Node)) {
            return false;
        }
        return EqualsVisitor.equals(this, (Node) obj);
    }

    @Override
    public Node clone() {
        return this.accept(new CloneVisitor(), null);
    }

    public Node getParentNode() {
        return parentNode;
    }

    @SuppressWarnings("unchecked")
    public <T> T getParentNodeOfType(Class<T> classType) {
        Node parent = parentNode;
        while (parent != null) {
            if (classType.isAssignableFrom(parent.getClass()))
                return (T) parent;
            parent = parent.parentNode;
        }
        return null;
    }

    public List<Node> getChildrenNodes() {
        return childrenNodes;
    }

    public boolean contains(Node other) {
        return range.contains(other.range);
    }

    public void addOrphanComment(Comment comment) {
        orphanComments.add(comment);
        comment.setParentNode(this);
    }

    /**
     * This is a list of Comment which are inside the node and are not associated
     * with any meaningful AST Node.
     *
     * For example, comments at the end of methods (immediately before the parenthesis)
     * or at the end of CompilationUnit are orphan comments.
     *
     * When more than one comment preceeds a statement, the one immediately preceding it
     * it is associated with the statements, while the others are orphans.
     * 
     * @return all comments that cannot be attributed to a concept
     */
    public List<Comment> getOrphanComments() {
        return orphanComments;
    }

    /**
     * This is the list of Comment which are contained in the Node either because
     * they are properly associated to one of its children or because they are floating
     * around inside the Node
     * 
     * @return all Comments within the node as a list
     */
    public List<Comment> getAllContainedComments() {
        List<Comment> comments = new LinkedList<>();
        comments.addAll(getOrphanComments());

        for (Node child : getChildrenNodes()) {
            if (child.getComment() != null) {
                comments.add(child.getComment());
            }
            comments.addAll(child.getAllContainedComments());
        }

        return comments;
    }

    /**
     * Assign a new parent to this node, removing it
     * from the list of children of the previous parent, if any.
     *
     * @param parentNode node to be set as parent
     */
    public void setParentNode(Node parentNode) {
        // remove from old parent, if any
        if (this.parentNode != null) {
            this.parentNode.childrenNodes.remove(this);
        }
        this.parentNode = parentNode;
        // add to new parent, if any
        if (this.parentNode != null) {
            this.parentNode.childrenNodes.add(this);
        }
    }

    protected void setAsParentNodeOf(List<? extends Node> childNodes) {
        if (childNodes != null) {
            for (Node current : childNodes) {
                current.setParentNode(this);
            }
        }
    }

    protected void setAsParentNodeOf(Node childNode) {
        if (childNode != null) {
            childNode.setParentNode(this);
        }
    }

    public static final int ABSOLUTE_BEGIN_LINE = -1;
    public static final int ABSOLUTE_END_LINE = -2;

    public boolean isPositionedAfter(Position position) {
        return range.isAfter(position);
    }

    public boolean isPositionedBefore(Position position) {
        return range.isBefore(position);
    }

    public boolean hasComment() {
        return comment != null;
    }

    public void tryAddImportToParentCompilationUnit(Class<?> clazz) {
        CompilationUnit parentNode = getParentNodeOfType(CompilationUnit.class);
        if (parentNode != null) {
            parentNode.addImport(clazz);
        }
    }

    /**
     * Recursively finds all nodes of a certain type.
     *
     * @param clazz the type of node to find.
     */
    public <N extends Node> List<N> getNodesByType(Class<N> clazz) {
        List<N> nodes = new ArrayList<>();
        for (Node child : getChildrenNodes()) {
            if (clazz.isInstance(child)) {
                nodes.add(clazz.cast(child));
            }
            nodes.addAll(child.getNodesByType(clazz));
        }
        return nodes;
    }

}
