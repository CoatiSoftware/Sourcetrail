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

package com.github.javaparser.symbolsolver.model.typesystem.parametrization;

import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.typesystem.Type;
import com.github.javaparser.symbolsolver.model.typesystem.TypeVariable;

import java.util.*;

/**
 * A map of values associated to TypeParameters.
 *
 * @author Federico Tomassetti
 */
public class TypeParametersMap {

    public static class Builder {
        private Map<String, Type> nameToValue;
        private Map<String, TypeParameterDeclaration> nameToDeclaration;

        public Builder() {
            nameToValue = new HashMap<>();
            nameToDeclaration = new HashMap<>();
        }

        private Builder(Map<String, Type> nameToValue, Map<String, TypeParameterDeclaration> nameToDeclaration) {
            this.nameToValue = new HashMap<>();
            this.nameToValue.putAll(nameToValue);
            this.nameToDeclaration = new HashMap<>();
            this.nameToDeclaration.putAll(nameToDeclaration);
        }

        public TypeParametersMap build() {
            return new TypeParametersMap(nameToValue, nameToDeclaration);
        }

        public Builder setValue(TypeParameterDeclaration typeParameter, Type value) {
            // TODO: we shouldn't just silently overwrite existing types!
            String qualifiedName = typeParameter.getQualifiedName();
            nameToValue.put(qualifiedName, value);
            nameToDeclaration.put(qualifiedName, typeParameter);
            return this;
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof TypeParametersMap)) return false;

        TypeParametersMap that = (TypeParametersMap) o;

        return nameToValue.equals(that.nameToValue) && nameToDeclaration.equals(that.nameToDeclaration);

    }

    @Override
    public int hashCode() {
        return nameToValue.hashCode();
    }

    @Override
    public String toString() {
        return "TypeParametersMap{" +
                "nameToValue=" + nameToValue +
                '}';
    }

    private Map<String, Type> nameToValue;
    private Map<String, TypeParameterDeclaration> nameToDeclaration;

    public static TypeParametersMap empty() {
        return new Builder().build();
    }

    private TypeParametersMap(Map<String, Type> nameToValue, Map<String, TypeParameterDeclaration> nameToDeclaration) {
        this.nameToValue = new HashMap<>();
        this.nameToValue.putAll(nameToValue);
        this.nameToDeclaration = new HashMap<>();
        this.nameToDeclaration.putAll(nameToDeclaration);
    }

    public Type getValue(TypeParameterDeclaration typeParameter) {
        String qualifiedName = typeParameter.getQualifiedName();
        if (nameToValue.containsKey(qualifiedName)) {
            return nameToValue.get(qualifiedName);
        } else {
            return new TypeVariable(typeParameter);
        }
    }

    public Optional<Type> getValueBySignature(String signature) {
        if (nameToValue.containsKey(signature)) {
            return Optional.of(nameToValue.get(signature));
        } else {
            return Optional.empty();
        }
    }

    public List<String> getNames(){
        return new ArrayList<>(nameToValue.keySet());
    }

    public List<Type> getTypes(){
        return new ArrayList<>(nameToValue.values());
    }

    public Builder toBuilder() {
        return new Builder(nameToValue, nameToDeclaration);
    }

    public boolean isEmpty() {
        return nameToValue.isEmpty();
    }

    public Type replaceAll(Type type) {
        Map<TypeParameterDeclaration, Type> inferredTypes = new HashMap<>();
        for (TypeParameterDeclaration typeParameterDeclaration : this.nameToDeclaration.values()) {
            type = type.replaceTypeVariables(typeParameterDeclaration, getValue(typeParameterDeclaration), inferredTypes);
        }
        return type;
    }
}
