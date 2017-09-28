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

package com.github.javaparser.symbolsolver.model.resolution;

/**
 * This exception is thrown when a symbol cannot be resolved.
 *
 * @author Federico Tomassetti
 */
public class UnsolvedSymbolException extends RuntimeException {

    private String context;
    private String name;
    private TypeSolver typeSolver;

    public UnsolvedSymbolException(String name, TypeSolver typeSolver) {
        super("Unsolved symbol : " + name + " using TypeSolver " + typeSolver);
        this.typeSolver = typeSolver;
        this.name = name;
    }

    public UnsolvedSymbolException(String name, String context) {
        super("Unsolved symbol in " + context + " : " + name);
        this.context = context;
        this.name = name;
    }

    public UnsolvedSymbolException(String name) {
        super("Unsolved symbol : " + name);
        this.context = "unknown";
        this.name = name;
    }

    @Override
    public String toString() {
        return "UnsolvedSymbolException{" +
                "context='" + context + '\'' +
                ", name='" + name + '\'' +
                ", typeSolver=" + typeSolver +
                '}';
    }
}
