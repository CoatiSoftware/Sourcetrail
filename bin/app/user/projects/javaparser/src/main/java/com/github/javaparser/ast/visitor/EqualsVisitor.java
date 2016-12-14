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
 
package com.github.javaparser.ast.visitor;

import java.util.List;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.AnnotationDeclaration;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.BaseParameter;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EmptyMemberDeclaration;
import com.github.javaparser.ast.body.EmptyTypeDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.EnumDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.InitializerDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.MultiTypeParameter;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.expr.ArrayAccessExpr;
import com.github.javaparser.ast.expr.ArrayCreationExpr;
import com.github.javaparser.ast.expr.ArrayInitializerExpr;
import com.github.javaparser.ast.expr.AssignExpr;
import com.github.javaparser.ast.expr.BinaryExpr;
import com.github.javaparser.ast.expr.BooleanLiteralExpr;
import com.github.javaparser.ast.expr.CastExpr;
import com.github.javaparser.ast.expr.CharLiteralExpr;
import com.github.javaparser.ast.expr.ClassExpr;
import com.github.javaparser.ast.expr.ConditionalExpr;
import com.github.javaparser.ast.expr.DoubleLiteralExpr;
import com.github.javaparser.ast.expr.EnclosedExpr;
import com.github.javaparser.ast.expr.FieldAccessExpr;
import com.github.javaparser.ast.expr.InstanceOfExpr;
import com.github.javaparser.ast.expr.IntegerLiteralExpr;
import com.github.javaparser.ast.expr.IntegerLiteralMinValueExpr;
import com.github.javaparser.ast.expr.LambdaExpr;
import com.github.javaparser.ast.expr.LongLiteralExpr;
import com.github.javaparser.ast.expr.LongLiteralMinValueExpr;
import com.github.javaparser.ast.expr.MarkerAnnotationExpr;
import com.github.javaparser.ast.expr.MemberValuePair;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.MethodReferenceExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.NormalAnnotationExpr;
import com.github.javaparser.ast.expr.NullLiteralExpr;
import com.github.javaparser.ast.expr.ObjectCreationExpr;
import com.github.javaparser.ast.expr.QualifiedNameExpr;
import com.github.javaparser.ast.expr.SingleMemberAnnotationExpr;
import com.github.javaparser.ast.expr.StringLiteralExpr;
import com.github.javaparser.ast.expr.SuperExpr;
import com.github.javaparser.ast.expr.ThisExpr;
import com.github.javaparser.ast.expr.TypeExpr;
import com.github.javaparser.ast.expr.UnaryExpr;
import com.github.javaparser.ast.expr.VariableDeclarationExpr;
import com.github.javaparser.ast.stmt.AssertStmt;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.BreakStmt;
import com.github.javaparser.ast.stmt.CatchClause;
import com.github.javaparser.ast.stmt.ContinueStmt;
import com.github.javaparser.ast.stmt.DoStmt;
import com.github.javaparser.ast.stmt.EmptyStmt;
import com.github.javaparser.ast.stmt.ExplicitConstructorInvocationStmt;
import com.github.javaparser.ast.stmt.ExpressionStmt;
import com.github.javaparser.ast.stmt.ForStmt;
import com.github.javaparser.ast.stmt.ForeachStmt;
import com.github.javaparser.ast.stmt.IfStmt;
import com.github.javaparser.ast.stmt.LabeledStmt;
import com.github.javaparser.ast.stmt.ReturnStmt;
import com.github.javaparser.ast.stmt.SwitchEntryStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;
import com.github.javaparser.ast.stmt.SynchronizedStmt;
import com.github.javaparser.ast.stmt.ThrowStmt;
import com.github.javaparser.ast.stmt.TryStmt;
import com.github.javaparser.ast.stmt.TypeDeclarationStmt;
import com.github.javaparser.ast.stmt.WhileStmt;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.IntersectionType;
import com.github.javaparser.ast.type.PrimitiveType;
import com.github.javaparser.ast.type.ReferenceType;
import com.github.javaparser.ast.type.UnionType;
import com.github.javaparser.ast.type.UnknownType;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.ast.type.WildcardType;

/**
 * @author Julio Vilmar Gesser
 */
public class EqualsVisitor implements GenericVisitor<Boolean, Node> {

	private static final EqualsVisitor SINGLETON = new EqualsVisitor();

	public static boolean equals(final Node n1, final Node n2) {
		return SINGLETON.nodeEquals(n1, n2);
	}

	private EqualsVisitor() {
		// hide constructor
	}

    /**
     * Check for equality that can be applied to each kind of node,
     * to not repeat it in every method we store that here.
     */
    private boolean commonNodeEquality(Node n1, Node n2) {
        if (!nodeEquals(n1.getComment(), n2.getComment())) {
            return false;
        }
		return nodesEquals(n1.getOrphanComments(), n2.getOrphanComments());
	}

	private <T extends Node> boolean nodesEquals(final List<T> nodes1, final List<T> nodes2) {
		if (nodes1 == null) {
			return nodes2 == null;
		} else if (nodes2 == null) {
			return false;
		}
		if (nodes1.size() != nodes2.size()) {
			return false;
		}
		for (int i = 0; i < nodes1.size(); i++) {
			if (!nodeEquals(nodes1.get(i), nodes2.get(i))) {
				return false;
			}
		}
		return true;
	}

	private <T extends Node> boolean nodeEquals(final T n1, final T n2) {
		if (n1 == n2) {
			return true;
		}
		if (n1 == null || n2 == null) {
			return false;
		}
		if (n1.getClass() != n2.getClass()) {
			return false;
		}
        if (!commonNodeEquality(n1, n2)){
            return false;
        }
		return n1.accept(this, n2);
	}

	private boolean objEquals(final Object n1, final Object n2) {
		if (n1 == n2) {
			return true;
		}
		if (n1 == null || n2 == null) {
			return false;
		}
		return n1.equals(n2);
	}

	@Override public Boolean visit(final CompilationUnit n1, final Node arg) {
		final CompilationUnit n2 = (CompilationUnit) arg;

		if (!nodeEquals(n1.getPackage(), n2.getPackage())) {
			return false;
		}

		if (!nodesEquals(n1.getImports(), n2.getImports())) {
			return false;
		}

		if (!nodesEquals(n1.getTypes(), n2.getTypes())) {
			return false;
		}

		if (!nodesEquals(n1.getComments(), n2.getComments())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final PackageDeclaration n1, final Node arg) {
		final PackageDeclaration n2 = (PackageDeclaration) arg;

		if (!nodeEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ImportDeclaration n1, final Node arg) {
		final ImportDeclaration n2 = (ImportDeclaration) arg;

		if (!nodeEquals(n1.getName(), n2.getName())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final TypeParameter n1, final Node arg) {
		final TypeParameter n2 = (TypeParameter) arg;

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeBound(), n2.getTypeBound())) {
			return false;
		}
		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final LineComment n1, final Node arg) {
		final LineComment n2 = (LineComment) arg;

		if (!objEquals(n1.getContent(), n2.getContent())) {
			return false;
		}

        if (!objEquals(n1.getBegin().line, n2.getBegin().line)) {
      		return false;
      	}

		return true;
	}

	@Override public Boolean visit(final BlockComment n1, final Node arg) {
		final BlockComment n2 = (BlockComment) arg;

		if (!objEquals(n1.getContent(), n2.getContent())) {
			return false;
		}

        if (!objEquals(n1.getBegin().line, n2.getBegin().line)) {
      			return false;
      	}

		return true;
	}

	@Override public Boolean visit(final ClassOrInterfaceDeclaration n1, final Node arg) {
		final ClassOrInterfaceDeclaration n2 = (ClassOrInterfaceDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (n1.isInterface() != n2.isInterface()) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeParameters(), n2.getTypeParameters())) {
			return false;
		}

		if (!nodesEquals(n1.getExtends(), n2.getExtends())) {
			return false;
		}

		if (!nodesEquals(n1.getImplements(), n2.getImplements())) {
			return false;
		}

		if (!nodesEquals(n1.getMembers(), n2.getMembers())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final EnumDeclaration n1, final Node arg) {
		final EnumDeclaration n2 = (EnumDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodesEquals(n1.getImplements(), n2.getImplements())) {
			return false;
		}

		if (!nodesEquals(n1.getEntries(), n2.getEntries())) {
			return false;
		}

		if (!nodesEquals(n1.getMembers(), n2.getMembers())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final EmptyTypeDeclaration n1, final Node arg) {
		return true;
	}

	@Override public Boolean visit(final EnumConstantDeclaration n1, final Node arg) {
		final EnumConstantDeclaration n2 = (EnumConstantDeclaration) arg;

		// javadoc are checked at CompilationUnit

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodesEquals(n1.getArgs(), n2.getArgs())) {
			return false;
		}

		if (!nodesEquals(n1.getClassBody(), n2.getClassBody())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final AnnotationDeclaration n1, final Node arg) {
		final AnnotationDeclaration n2 = (AnnotationDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodesEquals(n1.getMembers(), n2.getMembers())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final AnnotationMemberDeclaration n1, final Node arg) {
		final AnnotationMemberDeclaration n2 = (AnnotationMemberDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodeEquals(n1.getDefaultValue(), n2.getDefaultValue())) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final FieldDeclaration n1, final Node arg) {
		final FieldDeclaration n2 = (FieldDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		if (!nodesEquals(n1.getVariables(), n2.getVariables())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final VariableDeclarator n1, final Node arg) {
		final VariableDeclarator n2 = (VariableDeclarator) arg;

		if (!nodeEquals(n1.getId(), n2.getId())) {
			return false;
		}

		if (!nodeEquals(n1.getInit(), n2.getInit())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final VariableDeclaratorId n1, final Node arg) {
		final VariableDeclaratorId n2 = (VariableDeclaratorId) arg;

		if (n1.getArrayCount() != n2.getArrayCount()) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ConstructorDeclaration n1, final Node arg) {
		final ConstructorDeclaration n2 = (ConstructorDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodeEquals(n1.getBody(), n2.getBody())) {
			return false;
		}

		if (!nodesEquals(n1.getParameters(), n2.getParameters())) {
			return false;
		}

		if (!nodesEquals(n1.getThrows(), n2.getThrows())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeParameters(), n2.getTypeParameters())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final MethodDeclaration n1, final Node arg) {
		final MethodDeclaration n2 = (MethodDeclaration) arg;

		// javadoc are checked at CompilationUnit

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (n1.getArrayCount() != n2.getArrayCount()) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodeEquals(n1.getBody(), n2.getBody())) {
			return false;
		}

		if (!nodesEquals(n1.getParameters(), n2.getParameters())) {
			return false;
		}

		if (!nodesEquals(n1.getThrows(), n2.getThrows())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeParameters(), n2.getTypeParameters())) {
			return false;
		}
		if(n1.isDefault() != n2.isDefault()){
			return false;
		}
		return true;
	}
	
	@Override public Boolean visit(final Parameter n1, final Node arg) {
		final Parameter n2 = (Parameter) arg;
		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}
        return visit((BaseParameter<?>) n1, arg);
	}
	
	@Override public Boolean visit(MultiTypeParameter n1, Node arg) {
		MultiTypeParameter n2 = (MultiTypeParameter) arg;
        if (!nodeEquals(n1.getType(), n2.getType())) {
            return false;
        }
        return visit((BaseParameter<?>) n1, arg);
	}

    protected Boolean visit(final BaseParameter<?> n1, final Node arg) {
        final BaseParameter<?> n2 = (BaseParameter<?>) arg;

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!nodeEquals(n1.getId(), n2.getId())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		return true;
	}
	
	@Override public Boolean visit(final EmptyMemberDeclaration n1, final Node arg) {
		return true;
	}

	@Override public Boolean visit(final InitializerDeclaration n1, final Node arg) {
		final InitializerDeclaration n2 = (InitializerDeclaration) arg;

		if (!nodeEquals(n1.getBlock(), n2.getBlock())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final JavadocComment n1, final Node arg) {
		final JavadocComment n2 = (JavadocComment) arg;

		if (!objEquals(n1.getContent(), n2.getContent())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ClassOrInterfaceType n1, final Node arg) {
		final ClassOrInterfaceType n2 = (ClassOrInterfaceType) arg;

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodeEquals(n1.getScope(), n2.getScope())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeArgs(), n2.getTypeArgs())) {
			return false;
		}
		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final PrimitiveType n1, final Node arg) {
		final PrimitiveType n2 = (PrimitiveType) arg;

		if (n1.getType() != n2.getType()) {
			return false;
		}
		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final ReferenceType n1, final Node arg) {
		final ReferenceType n2 = (ReferenceType) arg;

		if (n1.getArrayCount() != n2.getArrayCount()) {
			return false;
		}
		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}
		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		List<List<AnnotationExpr>> n1a = n1.getArraysAnnotations();
		List<List<AnnotationExpr>> n2a = n2.getArraysAnnotations();

		if (n1a !=null && n2a!= null) {
			if(n1a.size() != n2a.size()){
				return false;
			}
			else{
				int i = 0;
				for(List<AnnotationExpr> aux: n1a){
					if(!nodesEquals(aux, n2a.get(i))){
						return false;
					}
					i++;
				}
			}
		}
		else if (n1a != n2a){
			return false;
		}
		return true;
	}

    @Override public Boolean visit(final IntersectionType n1, final Node arg) {
        final IntersectionType n2 = (IntersectionType) arg;

        List<ReferenceType> n1Elements = n1.getElements();
        List<ReferenceType> n2Elements = n2.getElements();

        if (n1Elements !=null && n2Elements != null) {
            if(n1Elements.size() != n2Elements.size()){
                return false;
            }
            else{
                int i = 0;
                for(ReferenceType aux: n1Elements){
                    if(aux.accept(this, n2Elements.get(i))) {
                        return false;
                    }
                    i++;
                }
            }
        }  else if (n1Elements != n2Elements){
            return false;
        }
        return true;
    }

    @Override public Boolean visit(final UnionType n1, final Node arg) {
        final UnionType n2 = (UnionType) arg;

        List<ReferenceType> n1Elements = n1.getElements();
        List<ReferenceType> n2Elements = n2.getElements();

        if (n1Elements !=null && n2Elements != null) {
            if(n1Elements.size() != n2Elements.size()){
                return false;
            }
            else{
                int i = 0;
                for(ReferenceType aux: n1Elements){
                    if(aux.accept(this, n2Elements.get(i))) {
                        return false;
                    }
                    i++;
                }
            }
        }  else if (n1Elements != n2Elements){
            return false;
        }
        return true;
    }

	@Override
	public Boolean visit(VoidType n1, Node arg) {
		VoidType n2 = (VoidType) arg;
		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final WildcardType n1, final Node arg) {
		final WildcardType n2 = (WildcardType) arg;

		if (!nodeEquals(n1.getExtends(), n2.getExtends())) {
			return false;
		}

		if (!nodeEquals(n1.getSuper(), n2.getSuper())) {
			return false;
		}
		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final UnknownType n1, final Node arg) {
		final UnknownType n2 = (UnknownType) arg;

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final ArrayAccessExpr n1, final Node arg) {
		final ArrayAccessExpr n2 = (ArrayAccessExpr) arg;

		if (!nodeEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodeEquals(n1.getIndex(), n2.getIndex())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ArrayCreationExpr n1, final Node arg) {
		final ArrayCreationExpr n2 = (ArrayCreationExpr) arg;

		if (n1.getArrayCount() != n2.getArrayCount()) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		if (!nodeEquals(n1.getInitializer(), n2.getInitializer())) {
			return false;
		}

		if (!nodesEquals(n1.getDimensions(), n2.getDimensions())) {
			return false;
		}
		List<List<AnnotationExpr>> n1a = n1.getArraysAnnotations();
		List<List<AnnotationExpr>> n2a = n2.getArraysAnnotations();

		if (n1a !=null && n2a!= null) {
			if(n1a.size() != n2a.size()){
				return false;
			}
			else{
				int i = 0;
				for(List<AnnotationExpr> aux: n1a){
					if(!nodesEquals(aux, n2a.get(i))){
						return false;
					}
					i++;
				}
			}
		}
		else if (n1a != n2a){
			return false;
		}
		return true;
	}

	@Override public Boolean visit(final ArrayInitializerExpr n1, final Node arg) {
		final ArrayInitializerExpr n2 = (ArrayInitializerExpr) arg;

		if (!nodesEquals(n1.getValues(), n2.getValues())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final AssignExpr n1, final Node arg) {
		final AssignExpr n2 = (AssignExpr) arg;

		if (n1.getOperator() != n2.getOperator()) {
			return false;
		}

		if (!nodeEquals(n1.getTarget(), n2.getTarget())) {
			return false;
		}

		if (!nodeEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final BinaryExpr n1, final Node arg) {
		final BinaryExpr n2 = (BinaryExpr) arg;

		if (n1.getOperator() != n2.getOperator()) {
			return false;
		}

		if (!nodeEquals(n1.getLeft(), n2.getLeft())) {
			return false;
		}

		if (!nodeEquals(n1.getRight(), n2.getRight())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final CastExpr n1, final Node arg) {
		final CastExpr n2 = (CastExpr) arg;

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ClassExpr n1, final Node arg) {
		final ClassExpr n2 = (ClassExpr) arg;

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ConditionalExpr n1, final Node arg) {
		final ConditionalExpr n2 = (ConditionalExpr) arg;

		if (!nodeEquals(n1.getCondition(), n2.getCondition())) {
			return false;
		}

		if (!nodeEquals(n1.getThenExpr(), n2.getThenExpr())) {
			return false;
		}

		if (!nodeEquals(n1.getElseExpr(), n2.getElseExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final EnclosedExpr n1, final Node arg) {
		final EnclosedExpr n2 = (EnclosedExpr) arg;

		if (!nodeEquals(n1.getInner(), n2.getInner())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final FieldAccessExpr n1, final Node arg) {
		final FieldAccessExpr n2 = (FieldAccessExpr) arg;

		if (!nodeEquals(n1.getScope(), n2.getScope())) {
			return false;
		}

		if (!objEquals(n1.getField(), n2.getField())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeArgs(), n2.getTypeArgs())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final InstanceOfExpr n1, final Node arg) {
		final InstanceOfExpr n2 = (InstanceOfExpr) arg;

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final StringLiteralExpr n1, final Node arg) {
		final StringLiteralExpr n2 = (StringLiteralExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final IntegerLiteralExpr n1, final Node arg) {
		final IntegerLiteralExpr n2 = (IntegerLiteralExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final LongLiteralExpr n1, final Node arg) {
		final LongLiteralExpr n2 = (LongLiteralExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final IntegerLiteralMinValueExpr n1, final Node arg) {
		final IntegerLiteralMinValueExpr n2 = (IntegerLiteralMinValueExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final LongLiteralMinValueExpr n1, final Node arg) {
		final LongLiteralMinValueExpr n2 = (LongLiteralMinValueExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final CharLiteralExpr n1, final Node arg) {
		final CharLiteralExpr n2 = (CharLiteralExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final DoubleLiteralExpr n1, final Node arg) {
		final DoubleLiteralExpr n2 = (DoubleLiteralExpr) arg;

		if (!objEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final BooleanLiteralExpr n1, final Node arg) {
		final BooleanLiteralExpr n2 = (BooleanLiteralExpr) arg;

		if (n1.getValue() != n2.getValue()) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final NullLiteralExpr n1, final Node arg) {
		return true;
	}

	@Override public Boolean visit(final MethodCallExpr n1, final Node arg) {
		final MethodCallExpr n2 = (MethodCallExpr) arg;

		if (!nodeEquals(n1.getScope(), n2.getScope())) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getArgs(), n2.getArgs())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeArgs(), n2.getTypeArgs())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final NameExpr n1, final Node arg) {
		final NameExpr n2 = (NameExpr) arg;

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ObjectCreationExpr n1, final Node arg) {
		final ObjectCreationExpr n2 = (ObjectCreationExpr) arg;

		if (!nodeEquals(n1.getScope(), n2.getScope())) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		if (!nodesEquals(n1.getAnonymousClassBody(), n2.getAnonymousClassBody())) {
			return false;
		}

		if (!nodesEquals(n1.getArgs(), n2.getArgs())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeArgs(), n2.getTypeArgs())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final QualifiedNameExpr n1, final Node arg) {
		final QualifiedNameExpr n2 = (QualifiedNameExpr) arg;

		if (!nodeEquals(n1.getQualifier(), n2.getQualifier())) {
			return false;
		}

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ThisExpr n1, final Node arg) {
		final ThisExpr n2 = (ThisExpr) arg;

		if (!nodeEquals(n1.getClassExpr(), n2.getClassExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final SuperExpr n1, final Node arg) {
		final SuperExpr n2 = (SuperExpr) arg;

		if (!nodeEquals(n1.getClassExpr(), n2.getClassExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final UnaryExpr n1, final Node arg) {
		final UnaryExpr n2 = (UnaryExpr) arg;

		if (n1.getOperator() != n2.getOperator()) {
			return false;
		}

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final VariableDeclarationExpr n1, final Node arg) {
		final VariableDeclarationExpr n2 = (VariableDeclarationExpr) arg;

        if (!n1.getModifiers().equals(n2.getModifiers())) {
			return false;
		}

		if (!nodesEquals(n1.getAnnotations(), n2.getAnnotations())) {
			return false;
		}

		if (!nodeEquals(n1.getType(), n2.getType())) {
			return false;
		}

		if (!nodesEquals(n1.getVars(), n2.getVars())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final MarkerAnnotationExpr n1, final Node arg) {
		final MarkerAnnotationExpr n2 = (MarkerAnnotationExpr) arg;

		if (!nodeEquals(n1.getName(), n2.getName())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final SingleMemberAnnotationExpr n1, final Node arg) {
		final SingleMemberAnnotationExpr n2 = (SingleMemberAnnotationExpr) arg;

		if (!nodeEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodeEquals(n1.getMemberValue(), n2.getMemberValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final NormalAnnotationExpr n1, final Node arg) {
		final NormalAnnotationExpr n2 = (NormalAnnotationExpr) arg;

		if (!nodeEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodesEquals(n1.getPairs(), n2.getPairs())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final MemberValuePair n1, final Node arg) {
		final MemberValuePair n2 = (MemberValuePair) arg;

		if (!objEquals(n1.getName(), n2.getName())) {
			return false;
		}

		if (!nodeEquals(n1.getValue(), n2.getValue())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ExplicitConstructorInvocationStmt n1, final Node arg) {
		final ExplicitConstructorInvocationStmt n2 = (ExplicitConstructorInvocationStmt) arg;

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		if (!nodesEquals(n1.getArgs(), n2.getArgs())) {
			return false;
		}

		if (!nodesEquals(n1.getTypeArgs(), n2.getTypeArgs())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final TypeDeclarationStmt n1, final Node arg) {
		final TypeDeclarationStmt n2 = (TypeDeclarationStmt) arg;

		if (!nodeEquals(n1.getTypeDeclaration(), n2.getTypeDeclaration())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final AssertStmt n1, final Node arg) {
		final AssertStmt n2 = (AssertStmt) arg;

		if (!nodeEquals(n1.getCheck(), n2.getCheck())) {
			return false;
		}

		if (!nodeEquals(n1.getMessage(), n2.getMessage())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final BlockStmt n1, final Node arg) {
		final BlockStmt n2 = (BlockStmt) arg;

		if (!nodesEquals(n1.getStmts(), n2.getStmts())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final LabeledStmt n1, final Node arg) {
		final LabeledStmt n2 = (LabeledStmt) arg;

		if (!nodeEquals(n1.getStmt(), n2.getStmt())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final EmptyStmt n1, final Node arg) {
		return true;
	}

	@Override public Boolean visit(final ExpressionStmt n1, final Node arg) {
		final ExpressionStmt n2 = (ExpressionStmt) arg;

		if (!nodeEquals(n1.getExpression(), n2.getExpression())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final SwitchStmt n1, final Node arg) {
		final SwitchStmt n2 = (SwitchStmt) arg;

		if (!nodeEquals(n1.getSelector(), n2.getSelector())) {
			return false;
		}

		if (!nodesEquals(n1.getEntries(), n2.getEntries())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final SwitchEntryStmt n1, final Node arg) {
		final SwitchEntryStmt n2 = (SwitchEntryStmt) arg;

		if (!nodeEquals(n1.getLabel(), n2.getLabel())) {
			return false;
		}

		if (!nodesEquals(n1.getStmts(), n2.getStmts())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final BreakStmt n1, final Node arg) {
		final BreakStmt n2 = (BreakStmt) arg;

		if (!objEquals(n1.getId(), n2.getId())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ReturnStmt n1, final Node arg) {
		final ReturnStmt n2 = (ReturnStmt) arg;

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final IfStmt n1, final Node arg) {
		final IfStmt n2 = (IfStmt) arg;

		if (!nodeEquals(n1.getCondition(), n2.getCondition())) {
			return false;
		}

		if (!nodeEquals(n1.getThenStmt(), n2.getThenStmt())) {
			return false;
		}

		if (!nodeEquals(n1.getElseStmt(), n2.getElseStmt())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final WhileStmt n1, final Node arg) {
		final WhileStmt n2 = (WhileStmt) arg;

		if (!nodeEquals(n1.getCondition(), n2.getCondition())) {
			return false;
		}

		if (!nodeEquals(n1.getBody(), n2.getBody())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ContinueStmt n1, final Node arg) {
		final ContinueStmt n2 = (ContinueStmt) arg;

		if (!objEquals(n1.getId(), n2.getId())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final DoStmt n1, final Node arg) {
		final DoStmt n2 = (DoStmt) arg;

		if (!nodeEquals(n1.getBody(), n2.getBody())) {
			return false;
		}

		if (!nodeEquals(n1.getCondition(), n2.getCondition())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ForeachStmt n1, final Node arg) {
		final ForeachStmt n2 = (ForeachStmt) arg;

		if (!nodeEquals(n1.getVariable(), n2.getVariable())) {
			return false;
		}

		if (!nodeEquals(n1.getIterable(), n2.getIterable())) {
			return false;
		}

		if (!nodeEquals(n1.getBody(), n2.getBody())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ForStmt n1, final Node arg) {
		final ForStmt n2 = (ForStmt) arg;

		if (!nodesEquals(n1.getInit(), n2.getInit())) {
			return false;
		}

		if (!nodeEquals(n1.getCompare(), n2.getCompare())) {
			return false;
		}

		if (!nodesEquals(n1.getUpdate(), n2.getUpdate())) {
			return false;
		}

		if (!nodeEquals(n1.getBody(), n2.getBody())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final ThrowStmt n1, final Node arg) {
		final ThrowStmt n2 = (ThrowStmt) arg;

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final SynchronizedStmt n1, final Node arg) {
		final SynchronizedStmt n2 = (SynchronizedStmt) arg;

		if (!nodeEquals(n1.getExpr(), n2.getExpr())) {
			return false;
		}

		if (!nodeEquals(n1.getBlock(), n2.getBlock())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final TryStmt n1, final Node arg) {
		final TryStmt n2 = (TryStmt) arg;

		if (!nodeEquals(n1.getTryBlock(), n2.getTryBlock())) {
			return false;
		}

		if (!nodesEquals(n1.getCatchs(), n2.getCatchs())) {
			return false;
		}

		if (!nodeEquals(n1.getFinallyBlock(), n2.getFinallyBlock())) {
			return false;
		}

		return true;
	}

	@Override public Boolean visit(final CatchClause n1, final Node arg) {
		final CatchClause n2 = (CatchClause) arg;

		if (!nodeEquals(n1.getParam(), n2.getParam())) {
			return false;
		}

		if (!nodeEquals(n1.getCatchBlock(), n2.getCatchBlock())) {
			return false;
		}

		return true;
	}

    @Override
    public Boolean visit(LambdaExpr n1, Node arg) {
        LambdaExpr n2 = (LambdaExpr) arg;
        if (!nodesEquals(n1.getParameters(), n2.getParameters())) {
            return false;
        }
        if(n1.isParametersEnclosed() != n2.isParametersEnclosed()){
            return false;
        }
        if (!nodeEquals(n1.getBody(), n2.getBody())) {
            return false;
        }
        return true;
    }

    @Override
    public Boolean visit(MethodReferenceExpr n1, Node arg) {
        MethodReferenceExpr n2 = (MethodReferenceExpr) arg;
        if (!nodeEquals(n1.getScope(), n2.getScope())) {
            return false;
        }
	    if (!nodesEquals(n1.getTypeArguments().getTypeArguments(), n2.getTypeArguments().getTypeArguments())) {
            return false;
        }
        if (!objEquals(n1.getIdentifier(), n2.getIdentifier())) {
            return false;
        }
        return true;
    }

    @Override
    public Boolean visit(TypeExpr n, Node arg) {
        TypeExpr n2 = (TypeExpr) arg;
        if (!nodeEquals(n.getType(), n2.getType())) {
            return false;
        }
        return true;
    }
}
