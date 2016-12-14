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

import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.AnnotationDeclaration;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EmptyMemberDeclaration;
import com.github.javaparser.ast.body.EmptyTypeDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.EnumDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.InitializerDeclaration;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.MultiTypeParameter;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.expr.*;
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
import com.github.javaparser.ast.type.*;

/**
 * A visitor that has a return value.
 * 
 * @author Julio Vilmar Gesser
 */
public interface GenericVisitor<R, A> {

	//- Compilation Unit ----------------------------------

	R visit(CompilationUnit n, A arg);

	R visit(PackageDeclaration n, A arg);

	R visit(ImportDeclaration n, A arg);

	R visit(TypeParameter n, A arg);

	R visit(LineComment n, A arg);

	R visit(BlockComment n, A arg);

	//- Body ----------------------------------------------

	R visit(ClassOrInterfaceDeclaration n, A arg);

	R visit(EnumDeclaration n, A arg);

	R visit(EmptyTypeDeclaration n, A arg);

	R visit(EnumConstantDeclaration n, A arg);

	R visit(AnnotationDeclaration n, A arg);

	R visit(AnnotationMemberDeclaration n, A arg);

	R visit(FieldDeclaration n, A arg);

	R visit(VariableDeclarator n, A arg);

	R visit(VariableDeclaratorId n, A arg);

	R visit(ConstructorDeclaration n, A arg);

	R visit(MethodDeclaration n, A arg);

	R visit(Parameter n, A arg);
	
	R visit(MultiTypeParameter n, A arg);

	R visit(EmptyMemberDeclaration n, A arg);

	R visit(InitializerDeclaration n, A arg);

	R visit(JavadocComment n, A arg);

	//- Type ----------------------------------------------

	R visit(ClassOrInterfaceType n, A arg);

	R visit(PrimitiveType n, A arg);

	R visit(ReferenceType n, A arg);

    R visit(IntersectionType n, A arg);

    R visit(UnionType n, A arg);

	R visit(VoidType n, A arg);

	R visit(WildcardType n, A arg);

	R visit(UnknownType n, A arg);

	//- Expression ----------------------------------------

	R visit(ArrayAccessExpr n, A arg);

	R visit(ArrayCreationExpr n, A arg);

	R visit(ArrayInitializerExpr n, A arg);

	R visit(AssignExpr n, A arg);

	R visit(BinaryExpr n, A arg);

	R visit(CastExpr n, A arg);

	R visit(ClassExpr n, A arg);

	R visit(ConditionalExpr n, A arg);

	R visit(EnclosedExpr n, A arg);

	R visit(FieldAccessExpr n, A arg);

	R visit(InstanceOfExpr n, A arg);

	R visit(StringLiteralExpr n, A arg);

	R visit(IntegerLiteralExpr n, A arg);

	R visit(LongLiteralExpr n, A arg);

	R visit(IntegerLiteralMinValueExpr n, A arg);

	R visit(LongLiteralMinValueExpr n, A arg);

	R visit(CharLiteralExpr n, A arg);

	R visit(DoubleLiteralExpr n, A arg);

	R visit(BooleanLiteralExpr n, A arg);

	R visit(NullLiteralExpr n, A arg);

	R visit(MethodCallExpr n, A arg);

	R visit(NameExpr n, A arg);

	R visit(ObjectCreationExpr n, A arg);

	R visit(QualifiedNameExpr n, A arg);

	R visit(ThisExpr n, A arg);

	R visit(SuperExpr n, A arg);

	R visit(UnaryExpr n, A arg);

	R visit(VariableDeclarationExpr n, A arg);

	R visit(MarkerAnnotationExpr n, A arg);

	R visit(SingleMemberAnnotationExpr n, A arg);

	R visit(NormalAnnotationExpr n, A arg);

	R visit(MemberValuePair n, A arg);

	//- Statements ----------------------------------------

	R visit(ExplicitConstructorInvocationStmt n, A arg);

	R visit(TypeDeclarationStmt n, A arg);

	R visit(AssertStmt n, A arg);

	R visit(BlockStmt n, A arg);

	R visit(LabeledStmt n, A arg);

	R visit(EmptyStmt n, A arg);

	R visit(ExpressionStmt n, A arg);

	R visit(SwitchStmt n, A arg);

	R visit(SwitchEntryStmt n, A arg);

	R visit(BreakStmt n, A arg);

	R visit(ReturnStmt n, A arg);

	R visit(IfStmt n, A arg);

	R visit(WhileStmt n, A arg);

	R visit(ContinueStmt n, A arg);

	R visit(DoStmt n, A arg);

	R visit(ForeachStmt n, A arg);

	R visit(ForStmt n, A arg);

	R visit(ThrowStmt n, A arg);

	R visit(SynchronizedStmt n, A arg);

	R visit(TryStmt n, A arg);

	R visit(CatchClause n, A arg);

    R visit(LambdaExpr n, A arg);

    R visit(MethodReferenceExpr n, A arg);

    R visit(TypeExpr n, A arg);
}
