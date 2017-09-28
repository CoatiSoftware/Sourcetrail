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

package com.github.javaparser.symbolsolver.logic;

import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.methods.MethodUsage;
import com.github.javaparser.symbolsolver.model.typesystem.Type;

import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

/**
 * @author Federico Tomassetti
 */
public final class FunctionalInterfaceLogic {

    private FunctionalInterfaceLogic() {
        // prevent instantiation
    }

    /**
     * Get the functional method defined by the type, if any.
     */
    public static Optional<MethodUsage> getFunctionalMethod(Type type) {
        if (type.isReferenceType() && type.asReferenceType().getTypeDeclaration().isInterface()) {
            return getFunctionalMethod(type.asReferenceType().getTypeDeclaration());
        } else {
            return Optional.empty();
        }
    }

    /**
     * Get the functional method defined by the type, if any.
     */
    public static Optional<MethodUsage> getFunctionalMethod(ReferenceTypeDeclaration typeDeclaration) {
        //We need to find all abstract methods
        Set<MethodUsage> methods = typeDeclaration.getAllMethods().stream()
                .filter(m -> m.getDeclaration().isAbstract())
                // Remove methods inherited by Object:
                // Consider the case of Comparator which define equals. It would be considered a functional method.
                .filter(m -> !declaredOnObject(m))
                .collect(Collectors.toSet());

        if (methods.size() == 1) {
            return Optional.of(methods.iterator().next());
        } else {
            return Optional.empty();
        }
    }

    private static String getSignature(Method m) {
        return String.format("%s(%s)", m.getName(), String.join(", ", Arrays.stream(m.getParameters()).map(p -> toSignature(p)).collect(Collectors.toList())));
    }

    private static String toSignature(Parameter p) {
        return p.getType().getCanonicalName();
    }

    private static List<String> OBJECT_METHODS_SIGNATURES = Arrays.stream(Object.class.getDeclaredMethods())
            .map(method -> getSignature(method))
            .collect(Collectors.toList());

    private static boolean declaredOnObject(MethodUsage m) {
        return OBJECT_METHODS_SIGNATURES.contains(m.getDeclaration().getSignature());
    }
}
