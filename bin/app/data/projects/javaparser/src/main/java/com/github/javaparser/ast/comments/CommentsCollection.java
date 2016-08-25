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

package com.github.javaparser.ast.comments;

import java.util.LinkedList;
import java.util.List;

/**
 * Set of comments produced by CommentsParser.
 */
public class CommentsCollection {
    private List<LineComment> lineComments = new LinkedList<LineComment>();
    private List<BlockComment> blockComments = new LinkedList<BlockComment>();
    private List<JavadocComment> javadocComments = new LinkedList<JavadocComment>();

    public List<LineComment> getLineComments(){
        return lineComments;
    }

    public List<BlockComment> getBlockComments(){
        return blockComments;
    }

    public List<JavadocComment> getJavadocComments(){
        return javadocComments;
    }

    public void addComment(LineComment lineComment){
        this.lineComments.add(lineComment);
    }

    public void addComment(BlockComment blockComment){
        this.blockComments.add(blockComment);
    }

    public void addComment(JavadocComment javadocComment){
        this.javadocComments.add(javadocComment);
    }

    public boolean contains(Comment comment){
        for (Comment c : getAll()){
            // we tolerate a difference of one element in the end column:
            // it depends how \r and \n are calculated...
            if ( c.getBegin().line==comment.getBegin().line &&
                 c.getBegin().column==comment.getBegin().column &&
                 c.getEnd().line==comment.getEnd().line &&
                 Math.abs(c.getEnd().column-comment.getEnd().column)<2 ){
                return true;
            }
        }
        return false;
    }

    public List<Comment> getAll(){
        List<Comment> comments = new LinkedList<Comment>();
        comments.addAll(lineComments);
        comments.addAll(blockComments);
        comments.addAll(javadocComments);
        return comments;
    }

    public int size(){
        return lineComments.size()+blockComments.size()+javadocComments.size();
    }

    public CommentsCollection minus(CommentsCollection other){
        CommentsCollection result = new CommentsCollection();
        for (LineComment comment : lineComments){
            if (!other.contains(comment)){
                result.lineComments.add(comment);
            }
        }
        for (BlockComment comment : blockComments){
            if (!other.contains(comment)){
                result.blockComments.add(comment);
            }
        }
        for (JavadocComment comment : javadocComments){
            if (!other.contains(comment)){
                result.javadocComments.add(comment);
            }
        }
        return result;
    }
}
