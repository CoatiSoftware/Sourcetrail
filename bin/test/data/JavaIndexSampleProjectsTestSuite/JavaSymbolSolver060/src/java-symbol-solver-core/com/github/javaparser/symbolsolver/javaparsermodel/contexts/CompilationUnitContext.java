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

package com.github.javaparser.symbolsolver.javaparsermodel.contexts;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.body.AnnotationDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.TypeDeclaration;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.Name;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFacade;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserAnnotationDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.ValueDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.typesystem.Type;
import com.github.javaparser.symbolsolver.resolution.MethodResolutionLogic;
import com.github.javaparser.symbolsolver.resolution.SymbolSolver;

import java.util.List;

/**
 * @author Federico Tomassetti
 */
public class CompilationUnitContext extends AbstractJavaParserContext<CompilationUnit> {

    ///
    /// Static methods
    ///

    private static boolean isQualifiedName(String name) {
        return name.contains(".");
    }

    ///
    /// Constructors
    ///

    public CompilationUnitContext(CompilationUnit wrappedNode, TypeSolver typeSolver) {
        super(wrappedNode, typeSolver);
    }

    ///
    /// Public methods
    ///

    @Override
    public SymbolReference<? extends ValueDeclaration> solveSymbol(String name, TypeSolver typeSolver) {

        // solve absolute references
        String itName = name;
        while (itName.contains(".")) {
            String typeName = getType(itName);
            String memberName = getMember(itName);
            SymbolReference<com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration> type = this.solveType(typeName, typeSolver);
            if (type.isSolved()) {
                return new SymbolSolver(typeSolver).solveSymbolInType(type.getCorrespondingDeclaration(), memberName);
            } else {
                itName = typeName;
            }
        }

        // Look among statically imported values
        if (wrappedNode.getImports() != null) {
            for (ImportDeclaration importDecl : wrappedNode.getImports()) {
                if(importDecl.isStatic()){
                    if(importDecl.isAsterisk()) {
                        String qName = importDecl.getNameAsString();
                        com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration importedType = typeSolver.solveType(qName);
                        SymbolReference<? extends ValueDeclaration> ref = new SymbolSolver(typeSolver).solveSymbolInType(importedType, name);
                        if (ref.isSolved()) {
                            return ref;
                        }
                    } else{
                        String whole = importDecl.getNameAsString();

                        // split in field/method name and type name
                        String memberName = getMember(whole);
                        String typeName = getType(whole);

                        if (memberName.equals(name)) {
                            com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration importedType = typeSolver.solveType(typeName);
                            return new SymbolSolver(typeSolver).solveSymbolInType(importedType, memberName);
                        }
                    }
                }
            }
        }

        return SymbolReference.unsolved(ValueDeclaration.class);
    }

    @Override
    public SymbolReference<com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration> solveType(String name, TypeSolver typeSolver) {
        if (wrappedNode.getTypes() != null) {
            for (TypeDeclaration<?> type : wrappedNode.getTypes()) {
                if (type.getName().getId().equals(name)) {
                    if (type instanceof ClassOrInterfaceDeclaration) {
                        return SymbolReference.solved(JavaParserFacade.get(typeSolver).getTypeDeclaration((ClassOrInterfaceDeclaration) type));
                    } else if (type instanceof AnnotationDeclaration) {
                        return SymbolReference.solved(new JavaParserAnnotationDeclaration((AnnotationDeclaration) type, typeSolver));
                    } else {
                        throw new UnsupportedOperationException(type.getClass().getCanonicalName());
                    }
                }
            }
        }

        if (wrappedNode.getImports() != null) {
            int dotPos = name.indexOf('.');
            String prefix = null;
            if (dotPos > -1) {
                prefix = name.substring(0, dotPos);
            }
            // look into type imports
            for (ImportDeclaration importDecl : wrappedNode.getImports()) {
                if (!importDecl.isAsterisk()) {
                    String qName = importDecl.getNameAsString();
                    boolean defaultPackage = !importDecl.getName().getQualifier().isPresent();
                    boolean found = !defaultPackage && importDecl.getName().getIdentifier().equals(name);
                    if (!found) {
                        if (prefix != null) {
                            found = qName.endsWith("." + prefix);
                            if (found) {
                                qName = qName + name.substring(dotPos);
                            }
                        }
                    }
                    if (found) {
                        SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration> ref = typeSolver.tryToSolveType(qName);
                        if (ref.isSolved()) {
                            return SymbolReference.adapt(ref, com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration.class);
                        }
                    }
                }
            }
            // look into type imports on demand
            for (ImportDeclaration importDecl : wrappedNode.getImports()) {
                if (importDecl.isAsterisk()) {
                    String qName = importDecl.getNameAsString() + "." + name;
                    SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration> ref = typeSolver.tryToSolveType(qName);
                    if (ref.isSolved()) {
                        return SymbolReference.adapt(ref, com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration.class);
                    }
                }
            }
        }

        // Look in current package
        if (this.wrappedNode.getPackageDeclaration().isPresent()) {
            String qName = this.wrappedNode.getPackageDeclaration().get().getName().toString() + "." + name;
            SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration> ref = typeSolver.tryToSolveType(qName);
            if (ref.isSolved()) {
                return SymbolReference.adapt(ref, com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration.class);
            }
        } else {
            // look for classes in the default package
            String qName = name;
            SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration> ref = typeSolver.tryToSolveType(qName);
            if (ref.isSolved()) {
                return SymbolReference.adapt(ref, com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration.class);
            }
        }

        // Look in the java.lang package
        SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration> ref = typeSolver.tryToSolveType("java.lang." + name);
        if (ref.isSolved()) {
            return SymbolReference.adapt(ref, com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration.class);
        }

        // DO NOT look for absolute name if this name is not qualified: you cannot import classes from the default package
        if (isQualifiedName(name)) {
            return SymbolReference.adapt(typeSolver.tryToSolveType(name), com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration.class);
        } else {
            return SymbolReference.unsolved(com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration.class);
        }
    }

    private String qName(ClassOrInterfaceType type) {
        if (type.getScope().isPresent()) {
            return qName(type.getScope().get()) + "." + type.getName().getId();
        } else {
            return type.getName().getId();
        }
    }

    private String qName(Name name) {
        if (name.getQualifier().isPresent()) {
            return qName(name.getQualifier().get()) + "." + name.getId();
        } else {
            return name.getId();
        }
    }

    private String toSimpleName(String qName) {
        String[] parts = qName.split("\\.");
        return parts[parts.length - 1];
    }

    private String packageName(String qName) {
        int lastDot = qName.lastIndexOf('.');
        if (lastDot == -1) {
            throw new UnsupportedOperationException();
        } else {
            return qName.substring(0, lastDot);
        }
    }

    @Override
    public SymbolReference<MethodDeclaration> solveMethod(String name, List<Type> argumentsTypes, boolean staticOnly, TypeSolver typeSolver) {
        for (ImportDeclaration importDecl : wrappedNode.getImports()) {
            if(importDecl.isStatic()){
                if(importDecl.isAsterisk()){
                    String importString = importDecl.getNameAsString();

                    if (this.wrappedNode.getPackageDeclaration().isPresent()
                            && this.wrappedNode.getPackageDeclaration().get().getName().getIdentifier().equals(packageName(importString))
                            && this.wrappedNode.getTypes().stream().anyMatch(it -> it.getName().getIdentifier().equals(toSimpleName(importString)))) {
                        // We are using a static import on a type defined in this file. It means the value was not found at
                        // a lower level so this will fail
                        return SymbolReference.unsolved(MethodDeclaration.class);
                    }

                    com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration ref = typeSolver.solveType(importString);
                    SymbolReference<MethodDeclaration> method = MethodResolutionLogic.solveMethodInType(ref, name, argumentsTypes, true, typeSolver);

                    if (method.isSolved()) {
                        return method;
                    }
                } else{
                    String qName = importDecl.getNameAsString();

                    if (qName.equals(name) || qName.endsWith("." + name)) {
                        String typeName = getType(qName);
                        com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration ref = typeSolver.solveType(typeName);
                        SymbolReference<MethodDeclaration> method = MethodResolutionLogic.solveMethodInType(ref, name, argumentsTypes, true, typeSolver);
                        if (method.isSolved()) {
                            return method;
                        }
                    }
                }
            }
        }
        return SymbolReference.unsolved(MethodDeclaration.class);
    }

    ///
    /// Private methods
    ///

    private String getType(String qName) {
        int index = qName.lastIndexOf('.');
        if (index == -1) {
            throw new UnsupportedOperationException();
        }
        String typeName = qName.substring(0, index);
        return typeName;
    }

    private String getMember(String qName) {
        int index = qName.lastIndexOf('.');
        if (index == -1) {
            throw new UnsupportedOperationException();
        }
        String memberName = qName.substring(index + 1);
        return memberName;
    }
}
