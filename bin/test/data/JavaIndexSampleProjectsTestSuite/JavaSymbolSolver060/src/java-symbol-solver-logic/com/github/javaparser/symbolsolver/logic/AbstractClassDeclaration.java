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

package com.github.javaparser.symbolsolver.logic;

import com.github.javaparser.symbolsolver.model.declarations.ClassDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration;
import com.github.javaparser.symbolsolver.model.typesystem.ReferenceType;

import java.util.ArrayList;
import java.util.List;

/**
 * A common ancestor for all ClassDeclarations.
 *
 * @author Federico Tomassetti
 */
public abstract class AbstractClassDeclaration extends AbstractTypeDeclaration implements ClassDeclaration {

    ///
    /// Public
    ///

    @Override
    public boolean hasName() {
        return getQualifiedName() != null;
    }

    @Override
    public final List<ReferenceType> getAllSuperClasses() {
        List<ReferenceType> superclasses = new ArrayList<>();
        ReferenceType superClass = getSuperClass();
        if (superClass != null) {
            superclasses.add(superClass);
            superclasses.addAll(superClass.getAllClassesAncestors());
        }

        if (superclasses.removeIf(s -> s.getQualifiedName().equals(Object.class.getCanonicalName()))) {
            superclasses.add(object());
        }
        return superclasses;
    }

    @Override
    public final List<ReferenceType> getAllInterfaces() {
        List<ReferenceType> interfaces = new ArrayList<>();
        for (ReferenceType interfaceDeclaration : getInterfaces()) {
            interfaces.add(interfaceDeclaration);
            interfaces.addAll(interfaceDeclaration.getAllInterfacesAncestors());
        }
        ReferenceType superClass = this.getSuperClass();
        if (superClass != null) {
            interfaces.addAll(superClass.getAllInterfacesAncestors());
        }
        return interfaces;
    }

    @Override
    public final ClassDeclaration asClass() {
        return this;
    }

    ///
    /// Protected
    ///

    /**
     * An implementation of the Object class.
     */
    protected abstract ReferenceType object();

}
