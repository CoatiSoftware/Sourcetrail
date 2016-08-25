package com.github.javaparser.ast.nodeTypes;

import com.github.javaparser.ast.expr.AnnotationExpr;

import java.util.List;

/**
 * A node that has array brackets behind it [][][]
 */
public interface NodeWithArrays<T> {
	int getArrayCount();

	T setArrayCount(int arrayCount);

	List<List<AnnotationExpr>> getArraysAnnotations();

	T setArraysAnnotations(List<List<AnnotationExpr>> arraysAnnotations);
}
