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

import com.github.javaparser.symbolsolver.model.typesystem.Type;

/**
 * A declaration of a method (either in an interface, a class, an enum or an annotation).
 *
 * @author Federico Tomassetti
 */
public interface MethodDeclaration extends MethodLikeDeclaration {

    /**
     * The type of the value returned by the current method. This method can also be invoked
     * for methods returning void.
     */
    Type getReturnType();

    /**
     * Is the method abstract? All interface methods not marked as default are abstract.
     */
    boolean isAbstract();

    /**
     * Is this a default method?
     */
    boolean isDefaultMethod();

    /*
     * Is this method static?
     */
    boolean isStatic();

}
