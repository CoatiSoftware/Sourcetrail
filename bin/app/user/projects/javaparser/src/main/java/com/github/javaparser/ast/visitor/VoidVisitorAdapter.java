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

import static com.github.javaparser.utils.Utils.isNullOrEmpty;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.AnnotationDeclaration;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.BodyDeclaration;
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
import com.github.javaparser.ast.body.TypeDeclaration;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.Comment;
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
import com.github.javaparser.ast.expr.Expression;
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
import com.github.javaparser.ast.stmt.Statement;
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
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.UnionType;
import com.github.javaparser.ast.type.UnknownType;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.ast.type.WildcardType;

/**
 * @author Julio Vilmar Gesser
 */
public abstract class VoidVisitorAdapter<A> implements VoidVisitor<A> {

	@Override public void visit(final AnnotationDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getNameExpr().accept(this, arg);
		if (n.getMembers() != null) {
            for (final BodyDeclaration<?> member : n.getMembers()) {
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(final AnnotationMemberDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
		if (n.getDefaultValue() != null) {
			n.getDefaultValue().accept(this, arg);
		}
	}

	@Override public void visit(final ArrayAccessExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
		n.getIndex().accept(this, arg);
	}

	@Override public void visit(final ArrayCreationExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getType().accept(this, arg);
		if (!isNullOrEmpty(n.getDimensions())) {
			for (final Expression dim : n.getDimensions()) {
				dim.accept(this, arg);
			}
		}
		if (n.getInitializer() != null) {
			n.getInitializer().accept(this, arg);
		}
	}

	@Override public void visit(final ArrayInitializerExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getValues() != null) {
			for (final Expression expr : n.getValues()) {
				expr.accept(this, arg);
			}
		}
	}

	@Override public void visit(final AssertStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getCheck().accept(this, arg);
		if (n.getMessage() != null) {
			n.getMessage().accept(this, arg);
		}
	}

	@Override public void visit(final AssignExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getTarget().accept(this, arg);
		n.getValue().accept(this, arg);
	}

	@Override public void visit(final BinaryExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getLeft().accept(this, arg);
		n.getRight().accept(this, arg);
	}

	@Override public void visit(final BlockComment n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final BlockStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getStmts() != null) {
			for (final Statement s : n.getStmts()) {
				s.accept(this, arg);
			}
		}
	}

	@Override public void visit(final BooleanLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final BreakStmt n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final CastExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getType().accept(this, arg);
		n.getExpr().accept(this, arg);
	}

	@Override public void visit(final CatchClause n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getParam().accept(this, arg);
		n.getCatchBlock().accept(this, arg);
	}

	@Override public void visit(final CharLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final ClassExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getType().accept(this, arg);
	}

	@Override public void visit(final ClassOrInterfaceDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		for (final AnnotationExpr a : n.getAnnotations()) {
			a.accept(this, arg);
		}
		n.getNameExpr().accept(this, arg);
		for (final TypeParameter t : n.getTypeParameters()) {
			t.accept(this, arg);
		}
		for (final ClassOrInterfaceType c : n.getExtends()) {
			c.accept(this, arg);
		}
		for (final ClassOrInterfaceType c : n.getImplements()) {
			c.accept(this, arg);
		}
        for (final BodyDeclaration<?> member : n.getMembers()) {
			member.accept(this, arg);
		}
	}

	@Override public void visit(final ClassOrInterfaceType n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getScope() != null) {
			n.getScope().accept(this, arg);
		}
		if (n.getTypeArgs() != null) {
			for (final Type t : n.getTypeArgs()) {
				t.accept(this, arg);
			}
		}
	}

	@Override public void visit(final CompilationUnit n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getPackage() != null) {
			n.getPackage().accept(this, arg);
		}
		if (n.getImports() != null) {
			for (final ImportDeclaration i : n.getImports()) {
				i.accept(this, arg);
			}
		}
		if (n.getTypes() != null) {
            for (final TypeDeclaration<?> typeDeclaration : n.getTypes()) {
				typeDeclaration.accept(this, arg);
			}
		}
	}

	@Override public void visit(final ConditionalExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getCondition().accept(this, arg);
		n.getThenExpr().accept(this, arg);
		n.getElseExpr().accept(this, arg);
	}

	@Override public void visit(final ConstructorDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		if (n.getTypeParameters() != null) {
			for (final TypeParameter t : n.getTypeParameters()) {
				t.accept(this, arg);
			}
		}
		n.getNameExpr().accept(this, arg);
		if (n.getParameters() != null) {
			for (final Parameter p : n.getParameters()) {
				p.accept(this, arg);
			}
		}
		if (n.getThrows() != null) {
			for (final ReferenceType name : n.getThrows()) {
				name.accept(this, arg);
			}
		}
		n.getBody().accept(this, arg);
	}

	@Override public void visit(final ContinueStmt n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final DoStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getBody().accept(this, arg);
		n.getCondition().accept(this, arg);
	}

	@Override public void visit(final DoubleLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final EmptyMemberDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
	}

	@Override public void visit(final EmptyStmt n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final EmptyTypeDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		n.getNameExpr().accept(this, arg);
	}

	@Override public void visit(final EnclosedExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getInner().accept(this, arg);
	}

	@Override public void visit(final EnumConstantDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		if (n.getArgs() != null) {
			for (final Expression e : n.getArgs()) {
				e.accept(this, arg);
			}
		}
		if (n.getClassBody() != null) {
            for (final BodyDeclaration<?> member : n.getClassBody()) {
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(final EnumDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getNameExpr().accept(this, arg);
		if (n.getImplements() != null) {
			for (final ClassOrInterfaceType c : n.getImplements()) {
				c.accept(this, arg);
			}
		}
		if (n.getEntries() != null) {
			for (final EnumConstantDeclaration e : n.getEntries()) {
				e.accept(this, arg);
			}
		}
		if (n.getMembers() != null) {
            for (final BodyDeclaration<?> member : n.getMembers()) {
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(final ExplicitConstructorInvocationStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		if (!n.isThis() && n.getExpr() != null) {
			n.getExpr().accept(this, arg);
		}
		if (n.getTypeArgs() != null) {
			for (final Type t : n.getTypeArgs()) {
				t.accept(this, arg);
			}
		}
		if (n.getArgs() != null) {
			for (final Expression e : n.getArgs()) {
				e.accept(this, arg);
			}
		}
	}

	@Override public void visit(final ExpressionStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getExpression().accept(this, arg);
	}

	@Override public void visit(final FieldAccessExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getScope().accept(this, arg);
		n.getFieldExpr().accept(this, arg);
	}

	@Override public void visit(final FieldDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
		for (final VariableDeclarator var : n.getVariables()) {
			var.accept(this, arg);
		}
	}

	@Override public void visit(final ForeachStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getVariable().accept(this, arg);
		n.getIterable().accept(this, arg);
		n.getBody().accept(this, arg);
	}

	@Override public void visit(final ForStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getInit() != null) {
			for (final Expression e : n.getInit()) {
				e.accept(this, arg);
			}
		}
		if (n.getCompare() != null) {
			n.getCompare().accept(this, arg);
		}
		if (n.getUpdate() != null) {
			for (final Expression e : n.getUpdate()) {
				e.accept(this, arg);
			}
		}
		n.getBody().accept(this, arg);
	}

	@Override public void visit(final IfStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getCondition().accept(this, arg);
		n.getThenStmt().accept(this, arg);
		if (n.getElseStmt() != null) {
			n.getElseStmt().accept(this, arg);
		}
	}

	@Override public void visit(final ImportDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
	}

	@Override public void visit(final InitializerDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		n.getBlock().accept(this, arg);
	}

	@Override public void visit(final InstanceOfExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
		n.getType().accept(this, arg);
	}

	@Override public void visit(final IntegerLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final IntegerLiteralMinValueExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final JavadocComment n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final LabeledStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getStmt().accept(this, arg);
	}

	@Override public void visit(final LineComment n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final LongLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final LongLiteralMinValueExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final MarkerAnnotationExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
	}

	@Override public void visit(final MemberValuePair n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getValue().accept(this, arg);
	}

	@Override public void visit(final MethodCallExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getScope() != null) {
			n.getScope().accept(this, arg);
		}
		if (n.getTypeArgs() != null) {
			for (final Type t : n.getTypeArgs()) {
				t.accept(this, arg);
			}
		}
		n.getNameExpr().accept(this, arg);
		if (n.getArgs() != null) {
			for (final Expression e : n.getArgs()) {
				e.accept(this, arg);
			}
		}
	}

	@Override public void visit(final MethodDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getJavaDoc() != null) {
			n.getJavaDoc().accept(this, arg);
		}
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		if (n.getTypeParameters() != null) {
			for (final TypeParameter t : n.getTypeParameters()) {
				t.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
		n.getNameExpr().accept(this, arg);
		if (n.getParameters() != null) {
			for (final Parameter p : n.getParameters()) {
				p.accept(this, arg);
			}
		}
		if (n.getThrows() != null) {
			for (final ReferenceType name : n.getThrows()) {
				name.accept(this, arg);
			}
		}
		if (n.getBody() != null) {
			n.getBody().accept(this, arg);
		}
	}

	@Override public void visit(final NameExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final NormalAnnotationExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
		if (n.getPairs() != null) {
			for (final MemberValuePair m : n.getPairs()) {
				m.accept(this, arg);
			}
		}
	}

	@Override public void visit(final NullLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final ObjectCreationExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getScope() != null) {
			n.getScope().accept(this, arg);
		}
		if (n.getTypeArgs() != null) {
			for (final Type t : n.getTypeArgs()) {
				t.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
		if (n.getArgs() != null) {
			for (final Expression e : n.getArgs()) {
				e.accept(this, arg);
			}
		}
		if (n.getAnonymousClassBody() != null) {
            for (final BodyDeclaration<?> member : n.getAnonymousClassBody()) {
				member.accept(this, arg);
			}
		}
	}

	@Override public void visit(final PackageDeclaration n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getName().accept(this, arg);
	}

	@Override public void visit(final Parameter n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
		n.getId().accept(this, arg);
	}
	
	@Override public void visit(final MultiTypeParameter n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
        if (n.getType() != null) {
			n.getType().accept(this, arg);
		}
		n.getId().accept(this, arg);
	}

	@Override public void visit(final PrimitiveType n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final QualifiedNameExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getQualifier().accept(this, arg);
	}

	@Override public void visit(final ReferenceType n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getType().accept(this, arg);
	}

    @Override public void visit(final IntersectionType n, final A arg) {
        visitComment(n.getComment(), arg);
        for (ReferenceType element : n.getElements()) {
            element.accept(this, arg);
        }
    }

    @Override public void visit(final UnionType n, final A arg) {
        visitComment(n.getComment(), arg);
        for (ReferenceType element : n.getElements()) {
            element.accept(this, arg);
        }
    }

	@Override public void visit(final ReturnStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getExpr() != null) {
			n.getExpr().accept(this, arg);
		}
	}

	@Override public void visit(final SingleMemberAnnotationExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getName().accept(this, arg);
		n.getMemberValue().accept(this, arg);
	}

	@Override public void visit(final StringLiteralExpr n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final SuperExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getClassExpr() != null) {
			n.getClassExpr().accept(this, arg);
		}
	}

	@Override public void visit(final SwitchEntryStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getLabel() != null) {
			n.getLabel().accept(this, arg);
		}
		if (n.getStmts() != null) {
			for (final Statement s : n.getStmts()) {
				s.accept(this, arg);
			}
		}
	}

	@Override public void visit(final SwitchStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getSelector().accept(this, arg);
		if (n.getEntries() != null) {
			for (final SwitchEntryStmt e : n.getEntries()) {
				e.accept(this, arg);
			}
		}
	}

	@Override public void visit(final SynchronizedStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
		n.getBlock().accept(this, arg);
	}

	@Override public void visit(final ThisExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getClassExpr() != null) {
			n.getClassExpr().accept(this, arg);
		}
	}

	@Override public void visit(final ThrowStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
	}

	@Override public void visit(final TryStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getResources() != null) {
			for (final VariableDeclarationExpr v : n.getResources()) {
				v.accept(this, arg);
			}
		}
		n.getTryBlock().accept(this, arg);
		if (n.getCatchs() != null) {
			for (final CatchClause c : n.getCatchs()) {
				c.accept(this, arg);
			}
		}
		if (n.getFinallyBlock() != null) {
			n.getFinallyBlock().accept(this, arg);
		}
	}

	@Override public void visit(final TypeDeclarationStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getTypeDeclaration().accept(this, arg);
	}

	@Override public void visit(final TypeParameter n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getTypeBound() != null) {
			for (final ClassOrInterfaceType c : n.getTypeBound()) {
				c.accept(this, arg);
			}
		}
	}

	@Override public void visit(final UnaryExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getExpr().accept(this, arg);
	}

	@Override public void visit(final UnknownType n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final VariableDeclarationExpr n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getAnnotations() != null) {
			for (final AnnotationExpr a : n.getAnnotations()) {
				a.accept(this, arg);
			}
		}
		n.getType().accept(this, arg);
		for (final VariableDeclarator v : n.getVars()) {
			v.accept(this, arg);
		}
	}

	@Override public void visit(final VariableDeclarator n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getId().accept(this, arg);
		if (n.getInit() != null) {
			n.getInit().accept(this, arg);
		}
	}

	@Override public void visit(final VariableDeclaratorId n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final VoidType n, final A arg) {
		visitComment(n.getComment(), arg);
	}

	@Override public void visit(final WhileStmt n, final A arg) {
		visitComment(n.getComment(), arg);
		n.getCondition().accept(this, arg);
		n.getBody().accept(this, arg);
	}

	@Override public void visit(final WildcardType n, final A arg) {
		visitComment(n.getComment(), arg);
		if (n.getExtends() != null) {
			n.getExtends().accept(this, arg);
		}
		if (n.getSuper() != null) {
			n.getSuper().accept(this, arg);
		}
	}

    @Override
    public void visit(LambdaExpr n, final A arg) {
        if (n.getParameters() != null) {
            for (final Parameter a : n.getParameters()) {
                a.accept(this, arg);
            }
        }
        if (n.getBody() != null) {
            n.getBody().accept(this, arg);
        }
    }

    @Override
    public void visit(MethodReferenceExpr n, final A arg) {
	    if (n.getTypeArguments().getTypeArguments() != null) {
		    for (final Type t : n.getTypeArguments().getTypeArguments()) {
			    t.accept(this, arg);
		    }
	    }
        if (n.getScope() != null) {
            n.getScope().accept(this, arg);
        }
    }

    @Override
    public void visit(TypeExpr n, final A arg) {
        if (n.getType() != null) {
            n.getType().accept(this, arg);
        }
    }

    private void visitComment(final Comment n, final A arg) {
		if (n != null) {
			n.accept(this, arg);
		}
	}
}
