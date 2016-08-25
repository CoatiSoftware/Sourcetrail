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

package com.github.javaparser;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.comments.Comment;
import com.github.javaparser.ast.comments.CommentsCollection;
import com.github.javaparser.ast.comments.CommentsParser;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.utils.PositionUtils;

import java.io.IOException;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

/**
 * Assigns comments to nodes of the AST.
 * 
 * @author Sebastian Kuerten
 * @author Júlio Vilmar Gesser
 */
class CommentsInserter {
    private boolean doNotAssignCommentsPreceedingEmptyLines = true;
    private boolean doNotConsiderAnnotationsAsNodeStartForCodeAttribution = false;
    
    CommentsInserter() {
    }

    /**
     * Adds the comments found in the source code of a compilation unit to that compilation unit.
     * @param cu an already created compilation unit
     * @param cuSourceCode the source code of the compilation unit. It will be parsed to find comments.
     */
    public void insertComments(CompilationUnit cu, String cuSourceCode) throws IOException {
        CommentsParser commentsParser = new CommentsParser();
        CommentsCollection allComments = commentsParser.parse(cuSourceCode);

        insertCommentsInCu(cu, allComments);
    }

    public boolean getDoNotConsiderAnnotationsAsNodeStartForCodeAttribution() {
        return doNotConsiderAnnotationsAsNodeStartForCodeAttribution;
    }

    public void setDoNotConsiderAnnotationsAsNodeStartForCodeAttribution(boolean newValue) {
        this.doNotConsiderAnnotationsAsNodeStartForCodeAttribution = newValue;
    }

    public boolean getDoNotAssignCommentsPreceedingEmptyLines() {
        return doNotAssignCommentsPreceedingEmptyLines;
    }

    public void setDoNotAssignCommentsPreceedingEmptyLines(boolean newValue) {
        this.doNotAssignCommentsPreceedingEmptyLines = newValue;
    }

    /**
     * Comments are attributed to the thing the comment and are removed from
     * allComments.
     */
    private void insertCommentsInCu(CompilationUnit cu,
            CommentsCollection commentsCollection) {
        if (commentsCollection.size() == 0)
            return;

        // I should sort all the direct children and the comments, if a comment
        // is the first thing then it
        // a comment to the CompilationUnit
        // FIXME if there is no package it could be also a comment to the
        // following class...
        // so I could use some heuristics in these cases to distinguish the two
        // cases

        List<Comment> comments = commentsCollection.getAll();
        PositionUtils.sortByBeginPosition(comments);
        List<Node> children = cu.getChildrenNodes();
        PositionUtils.sortByBeginPosition(children);

        if (cu.getPackage() != null
                && (children.isEmpty() || PositionUtils.areInOrder(
                        comments.get(0), children.get(0)))) {
            cu.setComment(comments.get(0));
            comments.remove(0);
        }

        insertCommentsInNode(cu, comments);
    }

    /**
     * This method try to attributes the nodes received to child of the node. It
     * returns the node that were not attributed.
     */
    private void insertCommentsInNode(Node node,
            List<Comment> commentsToAttribute) {
        if (commentsToAttribute.isEmpty())
            return;

        // the comments can:
        // 1) Inside one of the child, then it is the child that have to
        // associate them
        // 2) If they are not inside a child they could be preceeding nothing, a
        // comment or a child
        // if they preceed a child they are assigned to it, otherweise they
        // remain "orphans"

        List<Node> children = node.getChildrenNodes();
        PositionUtils.sortByBeginPosition(children);

        for (Node child : children) {
            List<Comment> commentsInsideChild = new LinkedList<Comment>();
            for (Comment c : commentsToAttribute) {
                if (PositionUtils.nodeContains(child, c,
                        doNotConsiderAnnotationsAsNodeStartForCodeAttribution)) {
                    commentsInsideChild.add(c);
                }
            }
            commentsToAttribute.removeAll(commentsInsideChild);
            insertCommentsInNode(child, commentsInsideChild);
        }

        // I can attribute in line comments to elements preceeding them, if
        // there
        // is something contained in their line
        List<Comment> attributedComments = new LinkedList<Comment>();
        for (Comment comment : commentsToAttribute) {
            if (comment.isLineComment()) {
                for (Node child : children) {
                    if (child.getEnd().line == comment.getBegin().line
                        && attributeLineCommentToNodeOrChild(child,
                                comment.asLineComment())) {
                            attributedComments.add(comment);
                    }
                }
            }
        }

        // at this point I create an ordered list of all remaining comments and
        // children
        Comment previousComment = null;
        attributedComments = new LinkedList<Comment>();
        List<Node> childrenAndComments = new LinkedList<Node>();
        childrenAndComments.addAll(children);
        childrenAndComments.addAll(commentsToAttribute);
        PositionUtils.sortByBeginPosition(childrenAndComments,
                doNotConsiderAnnotationsAsNodeStartForCodeAttribution);

        for (Node thing : childrenAndComments) {
            if (thing instanceof Comment) {
                previousComment = (Comment) thing;
                if (!previousComment.isOrphan()) {
                    previousComment = null;
                }
            } else {
                if (previousComment != null && !thing.hasComment()) {
                    if (!doNotAssignCommentsPreceedingEmptyLines
                            || !thereAreLinesBetween(previousComment, thing)) {
                        thing.setComment(previousComment);
                        attributedComments.add(previousComment);
                        previousComment = null;
                    }
                }
            }
        }

        commentsToAttribute.removeAll(attributedComments);

        // all the remaining are orphan nodes
        for (Comment c : commentsToAttribute) {
            if (c.isOrphan()) {
                node.addOrphanComment(c);
            }
        }
    }

    private boolean attributeLineCommentToNodeOrChild(Node node,
            LineComment lineComment) {
        // The node start and end at the same line as the comment,
        // let's give to it the comment
        if (node.getBegin().line == lineComment.getBegin().line
                && !node.hasComment()) {
            if(!(node instanceof Comment)) {
                node.setComment(lineComment);
            }
            return true;
        } else {
            // try with all the children, sorted by reverse position (so the
            // first one is the nearest to the comment
            List<Node> children = new LinkedList<Node>();
            children.addAll(node.getChildrenNodes());
            PositionUtils.sortByBeginPosition(children);
            Collections.reverse(children);

            for (Node child : children) {
                if (attributeLineCommentToNodeOrChild(child, lineComment)) {
                    return true;
                }
            }

            return false;
        }
    }

    private boolean thereAreLinesBetween(Node a, Node b) {
        if (!PositionUtils.areInOrder(a, b)) {
            return thereAreLinesBetween(b, a);
        }
        int endOfA = a.getEnd().line;
        return b.getBegin().line > (endOfA + 1);
    }

}
