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
 * A visitor that does not return anything.
 * 
 * @author Julio Vilmar Gesser
 */
public interface VoidVisitor<A> {

	//- Compilation Unit ----------------------------------

	void visit(CompilationUnit n, A arg);

	void visit(PackageDeclaration n, A arg);

	void visit(ImportDeclaration n, A arg);

	void visit(TypeParameter n, A arg);

	void visit(LineComment n, A arg);

	void visit(BlockComment n, A arg);

	//- Body ----------------------------------------------

	void visit(ClassOrInterfaceDeclaration n, A arg);

	void visit(EnumDeclaration n, A arg);

	void visit(EmptyTypeDeclaration n, A arg);

	void visit(EnumConstantDeclaration n, A arg);

	void visit(AnnotationDeclaration n, A arg);

	void visit(AnnotationMemberDeclaration n, A arg);

	void visit(FieldDeclaration n, A arg);

	void visit(VariableDeclarator n, A arg);

	void visit(VariableDeclaratorId n, A arg);

	void visit(ConstructorDeclaration n, A arg);

	void visit(MethodDeclaration n, A arg);

	void visit(Parameter n, A arg);
	
	void visit(MultiTypeParameter n, A arg);

	void visit(EmptyMemberDeclaration n, A arg);

	void visit(InitializerDeclaration n, A arg);

	void visit(JavadocComment n, A arg);

	//- Type ----------------------------------------------

	void visit(ClassOrInterfaceType n, A arg);

	void visit(PrimitiveType n, A arg);

	void visit(ReferenceType n, A arg);

    void visit(IntersectionType n, A arg);

    void visit(UnionType n, A arg);

	void visit(VoidType n, A arg);

	void visit(WildcardType n, A arg);

	void visit(UnknownType n, A arg);

	//- Expression ----------------------------------------

	void visit(ArrayAccessExpr n, A arg);

	void visit(ArrayCreationExpr n, A arg);

	void visit(ArrayInitializerExpr n, A arg);

	void visit(AssignExpr n, A arg);

	void visit(BinaryExpr n, A arg);

	void visit(CastExpr n, A arg);

	void visit(ClassExpr n, A arg);

	void visit(ConditionalExpr n, A arg);

	void visit(EnclosedExpr n, A arg);

	void visit(FieldAccessExpr n, A arg);

	void visit(InstanceOfExpr n, A arg);

	void visit(StringLiteralExpr n, A arg);

	void visit(IntegerLiteralExpr n, A arg);

	void visit(LongLiteralExpr n, A arg);

	void visit(IntegerLiteralMinValueExpr n, A arg);

	void visit(LongLiteralMinValueExpr n, A arg);

	void visit(CharLiteralExpr n, A arg);

	void visit(DoubleLiteralExpr n, A arg);

	void visit(BooleanLiteralExpr n, A arg);

	void visit(NullLiteralExpr n, A arg);

	void visit(MethodCallExpr n, A arg);

	void visit(NameExpr n, A arg);

	void visit(ObjectCreationExpr n, A arg);

	void visit(QualifiedNameExpr n, A arg);

	void visit(ThisExpr n, A arg);

	void visit(SuperExpr n, A arg);

	void visit(UnaryExpr n, A arg);

	void visit(VariableDeclarationExpr n, A arg);

	void visit(MarkerAnnotationExpr n, A arg);

	void visit(SingleMemberAnnotationExpr n, A arg);

	void visit(NormalAnnotationExpr n, A arg);

	void visit(MemberValuePair n, A arg);

	//- Statements ----------------------------------------

	void visit(ExplicitConstructorInvocationStmt n, A arg);

	void visit(TypeDeclarationStmt n, A arg);

	void visit(AssertStmt n, A arg);

	void visit(BlockStmt n, A arg);

	void visit(LabeledStmt n, A arg);

	void visit(EmptyStmt n, A arg);

	void visit(ExpressionStmt n, A arg);

	void visit(SwitchStmt n, A arg);

	void visit(SwitchEntryStmt n, A arg);

	void visit(BreakStmt n, A arg);

	void visit(ReturnStmt n, A arg);

	void visit(IfStmt n, A arg);

	void visit(WhileStmt n, A arg);

	void visit(ContinueStmt n, A arg);

	void visit(DoStmt n, A arg);

	void visit(ForeachStmt n, A arg);

	void visit(ForStmt n, A arg);

	void visit(ThrowStmt n, A arg);

	void visit(SynchronizedStmt n, A arg);

	void visit(TryStmt n, A arg);

	void visit(CatchClause n, A arg);

    void visit(LambdaExpr n, A arg);

    void visit(MethodReferenceExpr n, A arg);

    void visit(TypeExpr n, A arg);
}
