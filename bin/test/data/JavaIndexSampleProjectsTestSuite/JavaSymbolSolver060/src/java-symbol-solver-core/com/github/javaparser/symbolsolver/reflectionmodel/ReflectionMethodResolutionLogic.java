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

import com.github.javaparser.symbolsolver.core.resolution.Context;
import com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.methods.MethodUsage;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.typesystem.ReferenceType;
import com.github.javaparser.symbolsolver.model.typesystem.ReferenceTypeImpl;
import com.github.javaparser.symbolsolver.model.typesystem.Type;
import com.github.javaparser.symbolsolver.model.typesystem.TypeVariable;
import com.github.javaparser.symbolsolver.resolution.MethodResolutionLogic;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.function.Predicate;
import java.util.stream.Collectors;

/**
 * @author Federico Tomassetti
 */
class ReflectionMethodResolutionLogic {

    static SymbolReference<MethodDeclaration> solveMethod(String name, List<Type> parameterTypes, boolean staticOnly,
                                                          TypeSolver typeSolver, ReferenceTypeDeclaration scopeType,
                                                          Class clazz){
        List<MethodDeclaration> methods = new ArrayList<>();
        Predicate<Method> staticOnlyCheck = m -> !staticOnly || (staticOnly && Modifier.isStatic(m.getModifiers()));
        for (Method method : clazz.getMethods()) {
            if (method.isBridge() || method.isSynthetic() || !method.getName().equals(name)|| !staticOnlyCheck.test(method)) continue;
            MethodDeclaration methodDeclaration = new ReflectionMethodDeclaration(method, typeSolver);
            methods.add(methodDeclaration);
        }

        for (ReferenceType ancestor : scopeType.getAncestors()) {
            SymbolReference<MethodDeclaration> ref = MethodResolutionLogic.solveMethodInType(ancestor.getTypeDeclaration(), name, parameterTypes, staticOnly, typeSolver);
            if (ref.isSolved()) {
                methods.add(ref.getCorrespondingDeclaration());
            }
        }

        if (scopeType.getAncestors().isEmpty()){
            ReferenceTypeImpl objectClass = new ReferenceTypeImpl(new ReflectionClassDeclaration(Object.class, typeSolver), typeSolver);
            SymbolReference<MethodDeclaration> ref = MethodResolutionLogic.solveMethodInType(objectClass.getTypeDeclaration(), name, parameterTypes, staticOnly, typeSolver);
            if (ref.isSolved()) {
                methods.add(ref.getCorrespondingDeclaration());
            }
        }
        return MethodResolutionLogic.findMostApplicable(methods, name, parameterTypes, typeSolver);
    }

    static Optional<MethodUsage> solveMethodAsUsage(String name, List<Type> argumentsTypes, TypeSolver typeSolver,
                                                    Context invokationContext, List<Type> typeParameterValues,
                                                    ReferenceTypeDeclaration scopeType, Class clazz) {
        if (typeParameterValues.size() != scopeType.getTypeParameters().size()) {
            // if it is zero we are going to ignore them
            if (!scopeType.getTypeParameters().isEmpty()) {
                // Parameters not specified, so default to Object
                typeParameterValues = new ArrayList<>();
                for (int i = 0; i < scopeType.getTypeParameters().size(); i++) {
                    typeParameterValues.add(new ReferenceTypeImpl(new ReflectionClassDeclaration(Object.class, typeSolver), typeSolver));
                }
            }
        }
        List<MethodUsage> methods = new ArrayList<>();
        for (Method method : clazz.getMethods()) {
            if (method.getName().equals(name) && !method.isBridge() && !method.isSynthetic()) {
                MethodDeclaration methodDeclaration = new ReflectionMethodDeclaration(method, typeSolver);
                MethodUsage methodUsage = replaceParams(typeParameterValues, scopeType, methodDeclaration);
                methods.add(methodUsage);
            }

        }

        for(ReferenceType ancestor : scopeType.getAncestors()){
            SymbolReference<MethodDeclaration> ref = MethodResolutionLogic.solveMethodInType(ancestor.getTypeDeclaration(), name, argumentsTypes, typeSolver);
            if (ref.isSolved()){
                MethodDeclaration correspondingDeclaration = ref.getCorrespondingDeclaration();
                MethodUsage methodUsage = replaceParams(typeParameterValues, ancestor.getTypeDeclaration(), correspondingDeclaration);
                methods.add(methodUsage);
            }
        }

        if (scopeType.getAncestors().isEmpty()){
            ReferenceTypeImpl objectClass = new ReferenceTypeImpl(new ReflectionClassDeclaration(Object.class, typeSolver), typeSolver);
            SymbolReference<MethodDeclaration> ref = MethodResolutionLogic.solveMethodInType(objectClass.getTypeDeclaration(), name, argumentsTypes, typeSolver);
            if (ref.isSolved()) {
                MethodUsage usage = replaceParams(typeParameterValues, objectClass.getTypeDeclaration(), ref.getCorrespondingDeclaration());
                methods.add(usage);
            }
        }

        final List<Type> finalTypeParameterValues = typeParameterValues;
        argumentsTypes = argumentsTypes.stream().map((pt) -> {
            int i = 0;
            for (TypeParameterDeclaration tp : scopeType.getTypeParameters()) {
                pt = pt.replaceTypeVariables(tp, finalTypeParameterValues.get(i));
                i++;
            }
            return pt;
        }).collect(Collectors.toList());
        return MethodResolutionLogic.findMostApplicableUsage(methods, name, argumentsTypes, typeSolver);
    }

    private static MethodUsage replaceParams(List<Type> typeParameterValues, ReferenceTypeDeclaration typeParametrizable, MethodDeclaration methodDeclaration) {
        MethodUsage methodUsage = new MethodUsage(methodDeclaration);
        int i = 0;

        // Only replace if we have enough values provided
        if (typeParameterValues.size() == typeParametrizable.getTypeParameters().size()){
            for (TypeParameterDeclaration tp : typeParametrizable.getTypeParameters()) {
                methodUsage = methodUsage.replaceTypeParameter(tp, typeParameterValues.get(i));
                i++;
            }
        }

        for (TypeParameterDeclaration methodTypeParameter : methodDeclaration.getTypeParameters()) {
            methodUsage = methodUsage.replaceTypeParameter(methodTypeParameter, new TypeVariable(methodTypeParameter));
        }

        return methodUsage;
    }
}
