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

import java.util.HashMap;
import java.util.Map;

/**
 * A usage of a type. It could be a primitive type or a reference type (enum, class, interface). In the later case it
 * could take type typeParametersValues (other TypeUsages). It could also be a TypeVariable, like in:
 * <p>
 * class A&lt;Bgt; { }
 * <p>
 * where B is a TypeVariable. It could also be Wildcard Type, possibly with constraints.
 *
 * @author Federico Tomassetti
 */
public interface Type {

    ///
    /// Relation with other types
    ///

    /**
     * Does this type represent an array?
     */
    default boolean isArray() {
        return false;
    }

    default int arrayLevel() {
        if (isArray()) {
            return 1 + this.asArrayType().getComponentType().arrayLevel();
        } else {
            return 0;
        }
    }

    /**
     * Is this a primitive type?
     */
    default boolean isPrimitive() {
        return false;
    }

    /**
     * Is this the null type?
     */
    default boolean isNull() {
        return false;
    }

    /**
     * Is this a non primitive value?
     */
    default boolean isReference() {
        return isReferenceType() || isArray() || isTypeVariable() || isNull() || isWildcard();
    }

    /**
     * Is this a lambda constraint type?
     */
    default boolean isConstraint() { return false; }

    /**
     * Can this be seen as a ReferenceTypeUsage?
     * In other words: is this a reference to a class, an interface or an enum?
     */
    default boolean isReferenceType() {
        return false;
    }

    default boolean isVoid() {
        return false;
    }

    default boolean isTypeVariable() {
        return false;
    }

    default boolean isWildcard() {
        return false;
    }

    ///
    /// Downcasting
    ///

    default ArrayType asArrayType() {
        throw new UnsupportedOperationException(String.format("%s is not an Array", this));
    }

    default ReferenceType asReferenceType() {
        throw new UnsupportedOperationException(String.format("%s is not a Reference Type", this));
    }

    default TypeParameterDeclaration asTypeParameter() {
        throw new UnsupportedOperationException(String.format("%s is not a Type parameter", this));
    }

    default TypeVariable asTypeVariable() {
        throw new UnsupportedOperationException(String.format("%s is not a Type variable", this));
    }

    default PrimitiveType asPrimitive() {
        throw new UnsupportedOperationException(String.format("%s is not a Primitive type", this));
    }

    default Wildcard asWildcard() {
        throw new UnsupportedOperationException(String.format("%s is not a Wildcard", this));
    }

    default LambdaConstraintType asConstraintType() {
        throw new UnsupportedOperationException(String.format("%s is not a constraint type", this));
    }

    ///
    /// Naming
    ///

    String describe();

    ///
    /// TypeParameters
    ///

    /**
     * Replace all variables referring to the given TypeParameter with the given value.
     * By replacing these values I could also infer some type equivalence.
     * Those would be collected in the given map.
     */
    default Type replaceTypeVariables(TypeParameterDeclaration tp, Type replaced, Map<TypeParameterDeclaration, Type> inferredTypes) {
        return this;
    }

    /**
     * This is like ({@link #replaceTypeVariables(TypeParameterDeclaration, Type, Map)} but ignores the inferred values.
     */
    default Type replaceTypeVariables(TypeParameterDeclaration tp, Type replaced) {
        return replaceTypeVariables(tp, replaced, new HashMap<>());
    }

    ///
    /// Assignability
    ///

    /**
     * This method checks if ThisType t = new OtherType() would compile.
     */
    boolean isAssignableBy(Type other);

}
