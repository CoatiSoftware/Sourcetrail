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

import java.util.List;
import java.util.Optional;

/**
 * An entity which has type parameter.
 *
 * @author Federico Tomassetti
 */
public interface TypeParametrizable {

    /**
     * The list of type parameters defined on this element.
     */
    List<TypeParameterDeclaration> getTypeParameters();

    /**
     * Find the closest TypeParameterDeclaration with the given name.
     * It first look on this element itself and then on the containers.
     */
    Optional<TypeParameterDeclaration> findTypeParameter(String name);

}
