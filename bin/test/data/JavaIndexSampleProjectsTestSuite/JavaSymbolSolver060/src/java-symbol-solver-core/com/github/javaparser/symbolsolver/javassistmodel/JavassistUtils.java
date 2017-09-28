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

package com.github.javaparser.symbolsolver.javassistmodel;

import com.github.javaparser.symbolsolver.core.resolution.Context;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParametrizable;
import com.github.javaparser.symbolsolver.model.methods.MethodUsage;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.resolution.UnsolvedSymbolException;
import com.github.javaparser.symbolsolver.model.typesystem.*;
import com.github.javaparser.symbolsolver.resolution.SymbolSolver;
import javassist.CtClass;
import javassist.CtMethod;
import javassist.NotFoundException;
import javassist.bytecode.BadBytecode;
import javassist.bytecode.SignatureAttribute;

import java.util.*;
import java.util.stream.Collectors;

/**
 * @author Federico Tomassetti
 */
class JavassistUtils {

    static Optional<MethodUsage> getMethodUsage(CtClass ctClass, String name, List<Type> argumentsTypes, TypeSolver typeSolver, Context invokationContext) {
        // TODO avoid bridge and synthetic methods
        for (CtMethod method : ctClass.getDeclaredMethods()) {
            if (method.getName().equals(name)) {
                // TODO check typeParametersValues
                MethodUsage methodUsage = new MethodUsage(new JavassistMethodDeclaration(method, typeSolver));
                if (argumentsTypes.size() < methodUsage.getNoParams()) {
                    // this method cannot be a good candidate (except if variadic ?)
                    continue;
                }
                try {
                    if (method.getGenericSignature() != null) {
                        SignatureAttribute.MethodSignature classSignature = SignatureAttribute.toMethodSignature(method.getGenericSignature());
                        List<Type> parametersOfReturnType = parseTypeParameters(classSignature.getReturnType().toString(), typeSolver, invokationContext);
                        Type newReturnType = methodUsage.returnType();
                        // consume one parametersOfReturnType at the time
                        if (!(newReturnType instanceof VoidType)) {
                            newReturnType = newReturnType.asReferenceType().transformTypeParameters(tp -> parametersOfReturnType.remove(0));
                        }
                        methodUsage = methodUsage.replaceReturnType(newReturnType);
                    }
                    return Optional.of(methodUsage);
                } catch (BadBytecode e) {
                    throw new RuntimeException(e);
                }
            }
        }

        try {
            CtClass superClass = ctClass.getSuperclass();
            if (superClass != null) {
                Optional<MethodUsage> ref = new JavassistClassDeclaration(superClass, typeSolver).solveMethodAsUsage(name, argumentsTypes, typeSolver, invokationContext, null);
                if (ref.isPresent()) {
                    return ref;
                }
            }
        } catch (NotFoundException e) {
            throw new RuntimeException(e);
        }

        try {
            for (CtClass interfaze : ctClass.getInterfaces()) {
                Optional<MethodUsage> ref = new JavassistInterfaceDeclaration(interfaze, typeSolver).solveMethodAsUsage(name, argumentsTypes, typeSolver, invokationContext, null);
                if (ref.isPresent()) {
                    return ref;
                }
            }
        } catch (NotFoundException e) {
            throw new RuntimeException(e);
        }

        return Optional.empty();
    }

    private static List<Type> parseTypeParameters(String signature, TypeSolver typeSolver, Context invokationContext) {
        String originalSignature = signature;
        if (signature.contains("<")) {
            signature = signature.substring(signature.indexOf('<') + 1);
            if (!signature.endsWith(">")) {
                throw new IllegalArgumentException();
            }
            signature = signature.substring(0, signature.length() - 1);
            if (signature.contains(",")) {
                throw new UnsupportedOperationException();
            }
            if (signature.contains("<")) {
                throw new UnsupportedOperationException(originalSignature);
            }
            if (signature.contains(">")) {
                throw new UnsupportedOperationException();
            }
            List<Type> types = new ArrayList<>();
            types.add(new SymbolSolver(typeSolver).solveTypeUsage(signature, invokationContext));
            return types;
        } else {
            return Collections.emptyList();
        }
    }

    static Type signatureTypeToType(SignatureAttribute.Type signatureType, TypeSolver typeSolver, TypeParametrizable typeParametrizable) {
        if (signatureType instanceof SignatureAttribute.ClassType) {
            SignatureAttribute.ClassType classType = (SignatureAttribute.ClassType) signatureType;
            List<Type> typeParameters = classType.getTypeArguments() == null ? Collections.emptyList() : Arrays.stream(classType.getTypeArguments()).map(ta -> typeArgumentToType(ta, typeSolver, typeParametrizable)).collect(Collectors.toList());
            final String typeName =
                    classType.getDeclaringClass() != null ?
                            classType.getDeclaringClass().getName() + "." + classType.getName() :
                            classType.getName();
            ReferenceTypeDeclaration typeDeclaration = typeSolver.solveType(
                    internalNameToCanonicalName(typeName));
            return new ReferenceTypeImpl(typeDeclaration, typeParameters, typeSolver);
        } else if (signatureType instanceof SignatureAttribute.TypeVariable) {
            SignatureAttribute.TypeVariable typeVariableSignature = (SignatureAttribute.TypeVariable)signatureType;
            Optional<TypeParameterDeclaration> typeParameterDeclarationOpt = typeParametrizable.findTypeParameter(typeVariableSignature.getName());
            if (!typeParameterDeclarationOpt.isPresent()) {
                throw new UnsolvedSymbolException("Unable to solve TypeVariable " + typeVariableSignature);
            }
            TypeParameterDeclaration typeParameterDeclaration = typeParameterDeclarationOpt.get();
            return new TypeVariable(typeParameterDeclaration);
        } else {
            throw new RuntimeException(signatureType.getClass().getCanonicalName());
        }
    }

    private static String internalNameToCanonicalName(String typeName) {
        return typeName.replaceAll("\\$", ".");
    }

    private static Type objectTypeArgumentToType(SignatureAttribute.ObjectType typeArgument, TypeSolver typeSolver, TypeParametrizable typeParametrizable) {
        String typeName = typeArgument.jvmTypeName();
        Optional<Type> type = getGenericParameterByName(typeName, typeParametrizable);
        return type.orElseGet(() -> new ReferenceTypeImpl(
            typeSolver.solveType(internalNameToCanonicalName(typeName)),
            typeSolver));
    }

    private static Optional<Type> getGenericParameterByName(String typeName, TypeParametrizable typeParametrizable) {
        Optional<TypeParameterDeclaration> tp = typeParametrizable.findTypeParameter(typeName);
        return tp.map(TypeVariable::new);
    }

    private static Type typeArgumentToType(SignatureAttribute.TypeArgument typeArgument, TypeSolver typeSolver, TypeParametrizable typeParametrizable) {
        if (typeArgument.isWildcard()) {
            if (typeArgument.getType() == null) {
                return Wildcard.UNBOUNDED;
            } else if (typeArgument.getKind() == '+') {
                return Wildcard.extendsBound(objectTypeArgumentToType(typeArgument.getType(), typeSolver, typeParametrizable));
            } else if (typeArgument.getKind() == '-') {
                return Wildcard.superBound(objectTypeArgumentToType(typeArgument.getType(), typeSolver, typeParametrizable));
            } else {
                throw new UnsupportedOperationException();
            }
        } else {
            return objectTypeArgumentToType(typeArgument.getType(), typeSolver, typeParametrizable);
        }
    }
}
