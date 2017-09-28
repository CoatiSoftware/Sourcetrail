package com.github.javaparser.symbolsolver.javaparsermodel;

import com.github.javaparser.ast.*;
import com.github.javaparser.ast.body.*;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.expr.*;
import com.github.javaparser.ast.modules.*;
import com.github.javaparser.ast.stmt.*;
import com.github.javaparser.ast.type.*;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.symbolsolver.model.typesystem.Type;

public class DefaultVisitorAdapter implements GenericVisitor<Type, Boolean> {
    @Override
    public Type visit(CompilationUnit node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(PackageDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(TypeParameter node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(LineComment node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(BlockComment node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ClassOrInterfaceDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(EnumDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(EnumConstantDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(AnnotationDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(AnnotationMemberDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(FieldDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(VariableDeclarator node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ConstructorDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(MethodDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(Parameter node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(InitializerDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(JavadocComment node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ClassOrInterfaceType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(PrimitiveType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ArrayType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ArrayCreationLevel node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(IntersectionType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(UnionType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(VoidType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(WildcardType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(UnknownType node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ArrayAccessExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ArrayCreationExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ArrayInitializerExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(AssignExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(BinaryExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(CastExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ClassExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ConditionalExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(EnclosedExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(FieldAccessExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(InstanceOfExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(StringLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(IntegerLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(LongLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(CharLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(DoubleLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(BooleanLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(NullLiteralExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(MethodCallExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(NameExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ObjectCreationExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ThisExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(SuperExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(UnaryExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(VariableDeclarationExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(MarkerAnnotationExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(SingleMemberAnnotationExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(NormalAnnotationExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(MemberValuePair node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ExplicitConstructorInvocationStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(LocalClassDeclarationStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(AssertStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(BlockStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(LabeledStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(EmptyStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ExpressionStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(SwitchStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(SwitchEntryStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(BreakStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ReturnStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(IfStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(WhileStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ContinueStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(DoStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ForeachStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ForStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ThrowStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(SynchronizedStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(TryStmt node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(CatchClause node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(LambdaExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(MethodReferenceExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(TypeExpr node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(NodeList node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(Name node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(SimpleName node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ImportDeclaration node, Boolean aBoolean) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ModuleDeclaration node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ModuleRequiresStmt node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ModuleExportsStmt node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ModuleProvidesStmt node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ModuleUsesStmt node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(ModuleOpensStmt node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(UnparsableStmt node, Boolean arg) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }
}
