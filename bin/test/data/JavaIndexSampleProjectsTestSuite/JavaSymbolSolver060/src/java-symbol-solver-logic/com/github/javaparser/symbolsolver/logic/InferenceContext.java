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

import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.typesystem.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * @author Federico Tomassetti
 */
public class InferenceContext {

    private int nextInferenceVariableId = 0;
    private ObjectProvider objectProvider;
    private List<InferenceVariableType> inferenceVariableTypes = new ArrayList<>();

    public InferenceContext(ObjectProvider objectProvider) {
        this.objectProvider = objectProvider;
    }

    private Map<String, InferenceVariableType> inferenceVariableTypeMap = new HashMap<>();

    private InferenceVariableType inferenceVariableTypeForTp(TypeParameterDeclaration tp) {
        if (!inferenceVariableTypeMap.containsKey(tp.getName())) {
            InferenceVariableType inferenceVariableType = new InferenceVariableType(nextInferenceVariableId++, objectProvider);
            inferenceVariableTypes.add(inferenceVariableType);
            inferenceVariableType.setCorrespondingTp(tp);
            inferenceVariableTypeMap.put(tp.getName(), inferenceVariableType);
        }
        return inferenceVariableTypeMap.get(tp.getName());
    }

    /**
     *
     * @return the actual with the inference variable inserted
     */
    public Type addPair(Type target, Type actual) {
        target = placeInferenceVariables(target);
        actual = placeInferenceVariables(actual);
        registerCorrespondance(target, actual);
        return target;
    }

    public Type addSingle(Type actual) {
        return placeInferenceVariables(actual);
    }

    private void registerCorrespondance(Type formalType, Type actualType) {
        if (formalType.isReferenceType() && actualType.isReferenceType()) {
            ReferenceType formalTypeAsReference = formalType.asReferenceType();
            ReferenceType actualTypeAsReference = actualType.asReferenceType();

            if (!formalTypeAsReference.getQualifiedName().equals(actualTypeAsReference.getQualifiedName())) {
                List<ReferenceType> ancestors = actualTypeAsReference.getAllAncestors();
                final String formalParamTypeQName = formalTypeAsReference.getQualifiedName();
                List<Type> correspondingFormalType = ancestors.stream().filter((a) -> a.getQualifiedName().equals(formalParamTypeQName)).collect(Collectors.toList());
                if (correspondingFormalType.isEmpty()) {
                    ancestors = formalTypeAsReference.getAllAncestors();
                    final String actualParamTypeQname = actualTypeAsReference.getQualifiedName();
                    List<Type> correspondingActualType = ancestors.stream().filter(a -> a.getQualifiedName().equals(actualParamTypeQname)).collect(Collectors.toList());
                    if (correspondingActualType.isEmpty()){
                        throw new ConfilictingGenericTypesException(formalType, actualType);
                    }
                    correspondingFormalType = correspondingActualType;

                }
                actualTypeAsReference = correspondingFormalType.get(0).asReferenceType();
            }

            if (formalTypeAsReference.getQualifiedName().equals(actualTypeAsReference.getQualifiedName())) {
                if (!formalTypeAsReference.typeParametersValues().isEmpty()) {
                    if (actualTypeAsReference.isRawType()) {
                        // nothing to do
                    } else {
                        int i = 0;
                        for (Type formalTypeParameter : formalTypeAsReference.typeParametersValues()) {
                            registerCorrespondance(formalTypeParameter, actualTypeAsReference.typeParametersValues().get(i));
                            i++;
                        }
                    }
                }
            }
        } else if (formalType instanceof InferenceVariableType && !actualType.isPrimitive()) {
            ((InferenceVariableType) formalType).registerEquivalentType(actualType);
            if (actualType instanceof InferenceVariableType) {
                ((InferenceVariableType) actualType).registerEquivalentType(formalType);
            }
        } else if (actualType.isNull()) {
            // nothing to do
        } else if (actualType.equals(formalType)) {
            // nothing to do
        } else if (actualType.isArray() && formalType.isArray()) {
            registerCorrespondance(formalType.asArrayType().getComponentType(), actualType.asArrayType().getComponentType());
        } else if (formalType.isWildcard()) {
            // nothing to do
            if ((actualType instanceof InferenceVariableType) && formalType.asWildcard().isBounded()) {
                ((InferenceVariableType) actualType).registerEquivalentType(formalType.asWildcard().getBoundedType());
                if (formalType.asWildcard().getBoundedType() instanceof InferenceVariableType) {
                    ((InferenceVariableType) formalType.asWildcard().getBoundedType()).registerEquivalentType(actualType);
                }
            }
            if (actualType.isWildcard()) {
                Wildcard formalWildcard = formalType.asWildcard();
                Wildcard actualWildcard = actualType.asWildcard();
                if (formalWildcard.isBounded() && formalWildcard.getBoundedType() instanceof InferenceVariableType) {
                    if (formalWildcard.isSuper() && actualWildcard.isSuper()) {
                        ((InferenceVariableType) formalType.asWildcard().getBoundedType()).registerEquivalentType(actualWildcard.getBoundedType());
                    } else if (formalWildcard.isExtends() && actualWildcard.isExtends()) {
                        ((InferenceVariableType) formalType.asWildcard().getBoundedType()).registerEquivalentType(actualWildcard.getBoundedType());
                    }
                }
            }

            if (actualType.isReferenceType()){
                if (formalType.asWildcard().isBounded()){
                    registerCorrespondance(formalType.asWildcard().getBoundedType(), actualType);
                }
            }
        } else if (actualType instanceof InferenceVariableType){
            if (formalType instanceof ReferenceType){
                ((InferenceVariableType) actualType).registerEquivalentType(formalType);
            } else if (formalType instanceof InferenceVariableType){
                ((InferenceVariableType) actualType).registerEquivalentType(formalType);
            }
        } else if (actualType.isConstraint()){
            LambdaConstraintType constraintType = actualType.asConstraintType();
            if (constraintType.getBound() instanceof InferenceVariableType){
                ((InferenceVariableType) constraintType.getBound()).registerEquivalentType(formalType);
            }
        } else if (actualType.isPrimitive()) {
            if (formalType.isPrimitive()) {
                // nothing to do
            } else {
                registerCorrespondance(formalType, objectProvider.byName(actualType.asPrimitive().getBoxTypeQName()));
            }
        } else {
            throw new UnsupportedOperationException(formalType.describe() + " " + actualType.describe());
        }
    }

    private Type placeInferenceVariables(Type type) {
        if (type.isWildcard()) {
            if (type.asWildcard().isExtends()) {
                return Wildcard.extendsBound(placeInferenceVariables(type.asWildcard().getBoundedType()));
            } else if (type.asWildcard().isSuper()) {
                return Wildcard.superBound(placeInferenceVariables(type.asWildcard().getBoundedType()));
            } else {
                return type;
            }
        } else if (type.isTypeVariable()) {
            return inferenceVariableTypeForTp(type.asTypeParameter());
        } else if (type.isReferenceType()) {
            return type.asReferenceType().transformTypeParameters(tp -> placeInferenceVariables(tp));
        } else if (type.isArray()) {
            return new ArrayType(placeInferenceVariables(type.asArrayType().getComponentType()));
        } else if (type.isNull() || type.isPrimitive() || type.isVoid()) {
            return type;
        } else if (type.isConstraint()){
            return LambdaConstraintType.bound(placeInferenceVariables(type.asConstraintType().getBound()));
        } else if (type instanceof InferenceVariableType) {
            return type;
        } else {
            throw new UnsupportedOperationException(type.describe());
        }
    }

    public Type resolve(Type type) {
        if (type instanceof InferenceVariableType) {
            InferenceVariableType inferenceVariableType = (InferenceVariableType) type;
            return inferenceVariableType.equivalentType();
        } else if (type.isReferenceType()) {
            return type.asReferenceType().transformTypeParameters(tp -> resolve(tp));
        } else if (type.isNull() || type.isPrimitive() || type.isVoid()) {
            return type;
        } else if (type.isArray()) {
            return new ArrayType(resolve(type.asArrayType().getComponentType()));
        } else if (type.isWildcard()) {
            if (type.asWildcard().isExtends()) {
                return Wildcard.extendsBound(resolve(type.asWildcard().getBoundedType()));
            } else if (type.asWildcard().isSuper()) {
                return Wildcard.superBound(resolve(type.asWildcard().getBoundedType()));
            } else {
                return type;
            }
        } else {
            throw new UnsupportedOperationException(type.describe());
        }
    }
}
