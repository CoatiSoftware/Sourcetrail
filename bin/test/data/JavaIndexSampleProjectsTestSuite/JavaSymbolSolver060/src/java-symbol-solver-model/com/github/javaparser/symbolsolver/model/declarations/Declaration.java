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

package com.github.javaparser.symbolsolver.model.declarations;

/**
 * A generic declaration.
 *
 * @author Federico Tomassetti
 */
public interface Declaration {

    /**
     * Anonymous classes do not have a name, for example.
     */
    default boolean hasName() {
        return true;
    }

    /**
     * Should return the name or throw a RuntimeException if the name is not available.
     */
    String getName();

    /**
     * Does this declaration represents a class field?
     */
    default boolean isField() {
        return false;
    }

    /**
     * Does this declaration represents a method parameter?
     */
    default boolean isParameter() {
        return false;
    }

    /**
     * Does this declaration represents a type?
     */
    default boolean isType() {
        return false;
    }

    /**
     * Does this declaration represents a method?
     */
    default boolean isMethod() {
        return false;
    }

    /**
     * Return this as a FieldDeclaration or throw an UnsupportedOperationException
     */
    default FieldDeclaration asField() {
        throw new UnsupportedOperationException(String.format("%s is not a FieldDeclaration", this));
    }

    /**
     * Return this as a ParameterDeclaration or throw an UnsupportedOperationException
     */
    default ParameterDeclaration asParameter() {
        throw new UnsupportedOperationException(String.format("%s is not a ParameterDeclaration", this));
    }

    /**
     * Return this as a TypeDeclaration or throw an UnsupportedOperationException
     */
    default TypeDeclaration asType() {
        throw new UnsupportedOperationException(String.format("%s is not a TypeDeclaration", this));
    }

    /**
     * Return this as a MethodDeclaration or throw an UnsupportedOperationException
     */
    default MethodDeclaration asMethod() {
        throw new UnsupportedOperationException(String.format("%s is not a MethodDeclaration", this));
    }
}
