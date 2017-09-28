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

package com.github.javaparser.symbolsolver.model.typesystem;

import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;

import java.util.Map;

/**
 * From JLS 4.4: A type variable is introduced by the declaration of a type parameter of a generic class,
 * interface, method, or constructor (§8.1.2, §9.1.2, §8.4.4, §8.8.4).
 *
 * @author Federico Tomassetti
 */
public class TypeVariable implements Type {

    private TypeParameterDeclaration typeParameter;

    public TypeVariable(TypeParameterDeclaration typeParameter) {
        this.typeParameter = typeParameter;
    }

    @Override
    public String toString() {
        return "TypeVariable {" + typeParameter.getQualifiedName() + "}";
    }

    public String qualifiedName() {
        return this.typeParameter.getQualifiedName();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        TypeVariable that = (TypeVariable) o;

        if (!typeParameter.getName().equals(that.typeParameter.getName())) return false;
        if (typeParameter.declaredOnType() != that.typeParameter.declaredOnType()) return false;
        if (typeParameter.declaredOnMethod() != that.typeParameter.declaredOnMethod()) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return typeParameter.hashCode();
    }

    @Override
    public boolean isArray() {
        return false;
    }

    @Override
    public boolean isPrimitive() {
        return false;
    }

    @Override
    public Type replaceTypeVariables(TypeParameterDeclaration tpToBeReplaced, Type replaced, Map<TypeParameterDeclaration, Type> inferredTypes) {
        if(tpToBeReplaced.getName().equals(this.typeParameter.getName())){
            inferredTypes.put(this.asTypeParameter(), replaced);
            return replaced;
        } else {
            return this;
        }
    }

    @Override
    public boolean isReferenceType() {
        return false;
    }

    @Override
    public String describe() {
        return typeParameter.getName();
    }

    @Override
    public TypeParameterDeclaration asTypeParameter() {
        return typeParameter;
    }

    @Override
    public TypeVariable asTypeVariable() {
        return this;
    }

    @Override
    public boolean isTypeVariable() {
        return true;
    }

    @Override
    public boolean isAssignableBy(Type other) {
        if (other.isTypeVariable()) {
            return describe().equals(other.describe());
        } else {
            return true;
        }
    }

}
