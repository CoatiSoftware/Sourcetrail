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

import com.github.javaparser.symbolsolver.model.declarations.Declaration;

import java.util.Optional;

/**
 * A reference to a symbol. It can solved or not solved. If solved the corresponding
 * declaration will be provided.
 *
 * @author Federico Tomassetti
 */
public class SymbolReference<S extends Declaration> {

    private Optional<? extends S> correspondingDeclaration;

    private SymbolReference(Optional<? extends S> correspondingDeclaration) {
        this.correspondingDeclaration = correspondingDeclaration;
    }

    /**
     * Create a solve reference to the given symbol.
     */
    public static <S extends Declaration, S2 extends S> SymbolReference<S> solved(S2 symbolDeclaration) {
        return new SymbolReference<S>(Optional.of(symbolDeclaration));
    }

    /**
     * Create an unsolved reference specifying the type of the value expected.
     */
    public static <S extends Declaration, S2 extends S> SymbolReference<S> unsolved(Class<S2> clazz) {
        return new SymbolReference<S>(Optional.<S>empty());
    }

    @Override
    public String toString() {
        return "SymbolReference{" + correspondingDeclaration + "}";
    }

    /**
     * The corresponding declaration. If not solve this throws UnsupportedOperationException.
     */
    public S getCorrespondingDeclaration() {
        if (!isSolved()) {
            throw new UnsupportedOperationException();
        }
        return correspondingDeclaration.get();
    }

    /**
     * Is the reference solved?
     */
    public boolean isSolved() {
        return correspondingDeclaration.isPresent();
    }

    public static <O extends Declaration> SymbolReference<O> adapt(SymbolReference<? extends O> ref, Class<O> clazz) {
        if (ref.isSolved()) {
            return SymbolReference.solved(ref.getCorrespondingDeclaration());
        } else {
            return SymbolReference.unsolved(clazz);
        }
    }
}
