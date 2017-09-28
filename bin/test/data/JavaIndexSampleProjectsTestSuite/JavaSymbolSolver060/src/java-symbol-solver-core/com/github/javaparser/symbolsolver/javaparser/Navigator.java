/*
 * Copyright 2016 Federico Tomassetti
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.github.javaparser.symbolsolver.javaparser;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.body.*;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.SimpleName;
import com.github.javaparser.ast.stmt.ReturnStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;

import java.util.LinkedList;
import java.util.List;
import java.util.Optional;

/**
 * This class can be used to easily retrieve nodes from a JavaParser AST.
 *
 * @author Federico Tomassetti
 */
public final class Navigator {

    private Navigator() {
        // prevent instantiation
    }

    public static Node getParentNode(Node node) {
        Node parent = node.getParentNode().orElse(null);
        return parent;
    }

    public static Optional<TypeDeclaration<?>> findType(CompilationUnit cu, String qualifiedName) {
        if (cu.getTypes().isEmpty()) {
            return Optional.empty();
        }

        final String typeName = getOuterTypeName(qualifiedName);
        Optional<TypeDeclaration<?>> type = cu.getTypes().stream().filter((t) -> t.getName().getId().equals(typeName)).findFirst();

        final String innerTypeName = getInnerTypeName(qualifiedName);
        if (type.isPresent() && !innerTypeName.isEmpty()) {
            return findType(type.get(), innerTypeName);
        }
        return type;
    }

    public static Optional<TypeDeclaration<?>> findType(TypeDeclaration<?> td, String qualifiedName) {
        final String typeName = getOuterTypeName(qualifiedName);

        Optional<TypeDeclaration<?>> type = Optional.empty();
        for (Node n : td.getMembers()) {
            if (n instanceof TypeDeclaration && ((TypeDeclaration<?>) n).getName().getId().equals(typeName)) {
                type = Optional.of((TypeDeclaration<?>) n);
                break;
            }
        }
        final String innerTypeName = getInnerTypeName(qualifiedName);
        if (type.isPresent() && !innerTypeName.isEmpty()) {
            return findType(type.get(), innerTypeName);
        }
        return type;
    }

    public static ClassOrInterfaceDeclaration demandClass(CompilationUnit cu, String qualifiedName) {
        ClassOrInterfaceDeclaration cd = demandClassOrInterface(cu, qualifiedName);
        if (cd.isInterface()) {
            throw new IllegalStateException("Type is not a class");
        }
        return cd;
    }

    public static EnumDeclaration demandEnum(CompilationUnit cu, String qualifiedName) {
        Optional<TypeDeclaration<?>> res = findType(cu, qualifiedName);
        if (!res.isPresent()) {
            throw new IllegalStateException("No type found");
        }
        if (!(res.get() instanceof EnumDeclaration)) {
            throw new IllegalStateException("Type is not an enum");
        }
        return (EnumDeclaration) res.get();
    }

    public static MethodDeclaration demandMethod(TypeDeclaration<?> cd, String name) {
        MethodDeclaration found = null;
        for (BodyDeclaration<?> bd : cd.getMembers()) {
            if (bd instanceof MethodDeclaration) {
                MethodDeclaration md = (MethodDeclaration) bd;
                if (md.getName().getId().equals(name)) {
                    if (found != null) {
                        throw new IllegalStateException("Ambiguous getName");
                    }
                    found = md;
                }
            }
        }
        if (found == null) {
            throw new IllegalStateException("No method with given name");
        }
        return found;
    }

    public static VariableDeclarator demandField(ClassOrInterfaceDeclaration cd, String name) {
        for (BodyDeclaration<?> bd : cd.getMembers()) {
            if (bd instanceof FieldDeclaration) {
                FieldDeclaration fd = (FieldDeclaration) bd;
                for (VariableDeclarator vd : fd.getVariables()) {
                    if (vd.getName().getId().equals(name)) {
                        return vd;
                    }
                }
            }
        }
        throw new IllegalStateException("No field with given name");
    }

    public static NameExpr findNameExpression(Node node, String name) {
        if (node instanceof NameExpr) {
            NameExpr nameExpr = (NameExpr) node;
            if (nameExpr.getName() != null && nameExpr.getName().getId().equals(name)) {
                return nameExpr;
            }
        }
        for (Node child : node.getChildNodes()) {
            NameExpr res = findNameExpression(child, name);
            if (res != null) {
                return res;
            }
        }
        return null;
    }

    public static SimpleName findSimpleName(Node node, String name) {
        if (node instanceof SimpleName) {
            SimpleName nameExpr = (SimpleName) node;
            if (nameExpr.getId() != null && nameExpr.getId().equals(name)) {
                return nameExpr;
            }
        }
        for (Node child : node.getChildNodes()) {
            SimpleName res = findSimpleName(child, name);
            if (res != null) {
                return res;
            }
        }
        return null;
    }

    public static MethodCallExpr findMethodCall(Node node, String methodName) {
        if (node instanceof MethodCallExpr) {
            MethodCallExpr methodCallExpr = (MethodCallExpr) node;
            if (methodCallExpr.getName().getId().equals(methodName)) {
                return methodCallExpr;
            }
        }
        for (Node child : node.getChildNodes()) {
            MethodCallExpr res = findMethodCall(child, methodName);
            if (res != null) {
                return res;
            }
        }
        return null;
    }

    public static VariableDeclarator demandVariableDeclaration(Node node, String name) {
        if (node instanceof VariableDeclarator) {
            VariableDeclarator variableDeclarator = (VariableDeclarator) node;
            if (variableDeclarator.getName().getId().equals(name)) {
                return variableDeclarator;
            }
        }
        for (Node child : node.getChildNodes()) {
            VariableDeclarator res = demandVariableDeclaration(child, name);
            if (res != null) {
                return res;
            }
        }
        return null;
    }

    public static ClassOrInterfaceDeclaration demandClassOrInterface(CompilationUnit compilationUnit, String qualifiedName) {
        Optional<TypeDeclaration<?>> res = findType(compilationUnit, qualifiedName);
        if (!res.isPresent()) {
            throw new IllegalStateException("No type named '" + qualifiedName + "'found");
        }
        if (!(res.get() instanceof ClassOrInterfaceDeclaration)) {
            throw new IllegalStateException("Type is not a class or an interface, it is " + res.get().getClass().getCanonicalName());
        }
        ClassOrInterfaceDeclaration cd = (ClassOrInterfaceDeclaration) res.get();
        return cd;
    }

    public static SwitchStmt findSwitch(Node node) {
        SwitchStmt res = findSwitchHelper(node);
        if (res == null) {
            throw new IllegalArgumentException();
        } else {
            return res;
        }
    }

    public static <N> N findNodeOfGivenClass(Node node, Class<N> clazz) {
        N res = findNodeOfGivenClassHelper(node, clazz);
        if (res == null) {
            throw new IllegalArgumentException();
        } else {
            return res;
        }
    }

    public static <N> List<N> findAllNodesOfGivenClass(Node node, Class<N> clazz) {
        List<N> res = new LinkedList<>();
        findAllNodesOfGivenClassHelper(node, clazz, res);
        return res;
    }

    public static ReturnStmt findReturnStmt(MethodDeclaration method) {
        return findNodeOfGivenClass(method, ReturnStmt.class);
    }

    public static <N extends Node> Optional<N> findAncestor(Node node, Class<N> clazz) {
        if (!node.getParentNode().isPresent()) {
            return Optional.empty();
        } else if (clazz.isInstance(node.getParentNode().get())) {
            return Optional.of(clazz.cast(node.getParentNode().get()));
        } else {
            return findAncestor(node.getParentNode().get(), clazz);
        }
    }

    ///
    /// Private methods
    ///

    private static String getOuterTypeName(String qualifiedName) {
        return qualifiedName.split("\\.", 2)[0];
    }

    private static String getInnerTypeName(String qualifiedName) {
        if (qualifiedName.contains(".")) {
            return qualifiedName.split("\\.", 2)[1];
        }
        return "";
    }

    private static SwitchStmt findSwitchHelper(Node node) {
        if (node instanceof SwitchStmt) {
            return (SwitchStmt) node;
        }
        for (Node child : node.getChildNodes()) {
            SwitchStmt resChild = findSwitchHelper(child);
            if (resChild != null) {
                return resChild;
            }
        }
        return null;
    }

    private static <N> N findNodeOfGivenClassHelper(Node node, Class<N> clazz) {
        if (clazz.isInstance(node)) {
            return clazz.cast(node);
        }
        for (Node child : node.getChildNodes()) {
            N resChild = findNodeOfGivenClassHelper(child, clazz);
            if (resChild != null) {
                return resChild;
            }
        }
        return null;
    }

    private static <N> void findAllNodesOfGivenClassHelper(Node node, Class<N> clazz, List<N> collector) {
        if (clazz.isInstance(node)) {
            collector.add(clazz.cast(node));
        }
        for (Node child : node.getChildNodes()) {
            findAllNodesOfGivenClassHelper(child, clazz, collector);
        }
    }
}
