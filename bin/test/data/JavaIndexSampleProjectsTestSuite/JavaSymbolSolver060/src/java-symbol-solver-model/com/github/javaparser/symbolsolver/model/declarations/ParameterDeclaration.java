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
 * Declaration of a parameter.
 *
 * @author Federico Tomassetti
 */
public interface ParameterDeclaration extends ValueDeclaration {

    @Override
    default boolean isParameter() {
        return true;
    }

    @Override
    default ParameterDeclaration asParameter() {
        return this;
    }

    /**
     * Is this parameter declared as variadic?
     */
    boolean isVariadic();

    /**
     * Describe the type of the parameter. In practice add three dots to the type name
     * is the parameter is variadic.
     */
    default String describeType() {
        if (isVariadic()) {
            return getType().asArrayType().getComponentType().describe() + "...";
        } else {
            return getType().describe();
        }
    }
}
