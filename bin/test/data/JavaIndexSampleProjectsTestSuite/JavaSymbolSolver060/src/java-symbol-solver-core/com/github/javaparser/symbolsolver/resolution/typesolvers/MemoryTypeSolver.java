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

import java.util.HashMap;
import java.util.Map;

/**
 * A TypeSolver which only consider the TypeDeclarations provided to it.
 *
 * @author Federico Tomassetti
 */
public class MemoryTypeSolver implements TypeSolver {

    private TypeSolver parent;
    private Map<String, ReferenceTypeDeclaration> declarationMap = new HashMap<>();

    @Override
    public String toString() {
        return "MemoryTypeSolver{" +
                "parent=" + parent +
                ", declarationMap=" + declarationMap +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof MemoryTypeSolver)) return false;

        MemoryTypeSolver that = (MemoryTypeSolver) o;

        if (parent != null ? !parent.equals(that.parent) : that.parent != null) return false;
        return !(declarationMap != null ? !declarationMap.equals(that.declarationMap) : that.declarationMap != null);

    }

    @Override
    public int hashCode() {
        int result = parent != null ? parent.hashCode() : 0;
        result = 31 * result + (declarationMap != null ? declarationMap.hashCode() : 0);
        return result;
    }

    @Override
    public TypeSolver getParent() {
        return parent;
    }

    @Override
    public void setParent(TypeSolver parent) {
        this.parent = parent;
    }

    public void addDeclaration(String name, ReferenceTypeDeclaration typeDeclaration) {
        this.declarationMap.put(name, typeDeclaration);
    }

    @Override
    public SymbolReference<ReferenceTypeDeclaration> tryToSolveType(String name) {
        if (declarationMap.containsKey(name)) {
            return SymbolReference.solved(declarationMap.get(name));
        } else {
            return SymbolReference.unsolved(ReferenceTypeDeclaration.class);
        }
    }

}
