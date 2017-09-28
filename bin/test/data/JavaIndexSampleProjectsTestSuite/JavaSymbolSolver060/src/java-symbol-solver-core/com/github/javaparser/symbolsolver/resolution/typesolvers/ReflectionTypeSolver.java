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

package com.github.javaparser.symbolsolver.resolution.typesolvers;

import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.reflectionmodel.ReflectionFactory;

import java.util.Optional;

/**
 * @author Federico Tomassetti
 */
public class ReflectionTypeSolver implements TypeSolver {

    private TypeSolver parent;

    public ReflectionTypeSolver(boolean jreOnly) {
        this.jreOnly = jreOnly;
    }

    public ReflectionTypeSolver() {
        this(true);
    }

    private boolean jreOnly;

    @Override
    public TypeSolver getParent() {
        return parent;
    }

    @Override
    public void setParent(TypeSolver parent) {
        this.parent = parent;
    }

    @Override
    public SymbolReference<ReferenceTypeDeclaration> tryToSolveType(String name) {
        if (!jreOnly || (name.startsWith("java.") || name.startsWith("javax."))) {
            try {
                ClassLoader classLoader = ReflectionTypeSolver.class.getClassLoader();

                // Some implementations could return null when the class was loaded through the bootstrap classloader
                // see https://docs.oracle.com/javase/8/docs/api/java/lang/Class.html#getClassLoader--
                if (classLoader == null) {
                    throw new RuntimeException("The ReflectionTypeSolver has been probably loaded through the bootstrap class loader. This usage is not supported by the JavaSymbolSolver");
                }

                Class<?> clazz = classLoader.loadClass(name);
                return SymbolReference.solved(ReflectionFactory.typeDeclarationFor(clazz, getRoot()));
            } catch (ClassNotFoundException e) {
                // it could be an inner class
                int lastDot = name.lastIndexOf('.');
                if (lastDot == -1) {
                    return SymbolReference.unsolved(ReferenceTypeDeclaration.class);
                } else {
                    String parentName = name.substring(0, lastDot);
                    String childName = name.substring(lastDot + 1);
                    SymbolReference<ReferenceTypeDeclaration> parent = tryToSolveType(parentName);
                    if (parent.isSolved()) {
                        Optional<ReferenceTypeDeclaration> innerClass = parent.getCorrespondingDeclaration().internalTypes()
                                .stream().filter(it -> it.getName().equals(childName)).findFirst();
                        if (innerClass.isPresent()) {
                            return SymbolReference.solved(innerClass.get());
                        } else {
                            return SymbolReference.unsolved(ReferenceTypeDeclaration.class);
                        }
                    } else {
                        return SymbolReference.unsolved(ReferenceTypeDeclaration.class);
                    }
                }
            }
        } else {
            return SymbolReference.unsolved(ReferenceTypeDeclaration.class);
        }
    }

}
