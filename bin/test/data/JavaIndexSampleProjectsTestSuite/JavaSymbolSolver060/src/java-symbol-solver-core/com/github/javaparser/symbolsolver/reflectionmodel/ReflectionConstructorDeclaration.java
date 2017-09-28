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

package com.github.javaparser.symbolsolver.reflectionmodel;

import com.github.javaparser.symbolsolver.model.declarations.*;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;

import java.lang.reflect.Constructor;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

/**
 * @author Fred Lefévère-Laoide
 */
public class ReflectionConstructorDeclaration implements ConstructorDeclaration {

    private Constructor<?> constructor;
    private TypeSolver typeSolver;

    public ReflectionConstructorDeclaration(Constructor<?> constructor,
                                            TypeSolver typeSolver) {
        this.constructor = constructor;
        this.typeSolver = typeSolver;
    }

    @Override
    public ClassDeclaration declaringType() {
        return new ReflectionClassDeclaration(constructor.getDeclaringClass(), typeSolver);
    }

    @Override
    public int getNumberOfParams() {
        return constructor.getParameterCount();
    }

    @Override
    public ParameterDeclaration getParam(int i) {
        if (i < 0 || i >= getNumberOfParams()) {
            throw new IllegalArgumentException(String.format("No param with index %d. Number of params: %d", i, getNumberOfParams()));
        }
        boolean variadic = false;
        if (constructor.isVarArgs()) {
            variadic = i == (constructor.getParameterCount() - 1);
        }
        return new ReflectionParameterDeclaration(constructor.getParameterTypes()[i], constructor.getGenericParameterTypes()[i], typeSolver, variadic);
    }

    @Override
    public String getName() {
        return constructor.getName();
    }

    @Override
    public AccessLevel accessLevel() {
        return ReflectionFactory.modifiersToAccessLevel(constructor.getModifiers());
    }

    @Override
    public List<TypeParameterDeclaration> getTypeParameters() {
        return Arrays.stream(constructor.getTypeParameters()).map((refTp) -> new ReflectionTypeParameter(refTp, false, typeSolver)).collect(Collectors.toList());
    }
}