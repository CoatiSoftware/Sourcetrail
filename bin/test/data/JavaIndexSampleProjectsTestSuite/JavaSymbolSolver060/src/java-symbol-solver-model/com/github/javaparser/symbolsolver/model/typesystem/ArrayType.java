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
 * Array Type.
 *
 * @author Federico Tomassetti
 */
public class ArrayType implements Type {

    private Type baseType;

    public ArrayType(Type baseType) {
        this.baseType = baseType;
    }

    ///
    /// Object methods
    ///

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        ArrayType that = (ArrayType) o;

        if (!baseType.equals(that.baseType)) return false;

        return true;
    }

    @Override
    public int hashCode() {
        return baseType.hashCode();
    }

    @Override
    public String toString() {
        return "ArrayTypeUsage{" + baseType + "}";
    }

    ///
    /// Type methods
    ///

    @Override
    public ArrayType asArrayType() {
        return this;
    }

    @Override
    public boolean isArray() {
        return true;
    }

    @Override
    public String describe() {
        return baseType.describe() + "[]";
    }

    public Type getComponentType() {
        return baseType;
    }

    @Override
    public boolean isAssignableBy(Type other) {
        if (other.isArray()) {
            if (baseType.isPrimitive() && other.asArrayType().getComponentType().isPrimitive()) {
              return baseType.equals(other.asArrayType().getComponentType());
            }
            return baseType.isAssignableBy(other.asArrayType().getComponentType());
        } else if (other.isNull()) {
            return true;
        }
        return false;
    }

    @Override
    public Type replaceTypeVariables(TypeParameterDeclaration tpToReplace, Type replaced, Map<TypeParameterDeclaration, Type> inferredTypes) {
        Type baseTypeReplaced = baseType.replaceTypeVariables(tpToReplace, replaced, inferredTypes);
        if (baseTypeReplaced == baseType) {
            return this;
        } else {
            return new ArrayType(baseTypeReplaced);
        }
    }

}
