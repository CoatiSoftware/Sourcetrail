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

package com.github.javaparser.symbolsolver.model.typesystem;

import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration.Bound;
import com.github.javaparser.symbolsolver.model.methods.MethodUsage;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.typesystem.parametrization.TypeParameterValueProvider;
import com.github.javaparser.symbolsolver.model.typesystem.parametrization.TypeParametersMap;
import com.github.javaparser.symbolsolver.model.typesystem.parametrization.TypeParametrized;
import javaslang.Tuple2;

import java.util.*;
import java.util.stream.Collectors;

/**
 * A ReferenceType like a class, an interface or an enum. Note that this type can contain also the values
 * specified for the type parameters.
 *
 * @author Federico Tomassetti
 */
public abstract class ReferenceType implements Type, TypeParametrized, TypeParameterValueProvider {

    //
    // Fields
    //

    protected ReferenceTypeDeclaration typeDeclaration;
    protected TypeSolver typeSolver;
    protected TypeParametersMap typeParametersMap;

    //
    // Constructors
    //

    public ReferenceType(ReferenceTypeDeclaration typeDeclaration, TypeSolver typeSolver) {
        this(typeDeclaration, deriveParams(typeDeclaration), typeSolver);
    }

    public ReferenceType(ReferenceTypeDeclaration typeDeclaration, List<Type> typeParameters, TypeSolver typeSolver) {
        if (typeSolver == null) {
            throw new IllegalArgumentException("typeSolver should not be null");
        }
        if (typeDeclaration.isTypeParameter()) {
            throw new IllegalArgumentException("You should use only Classes, Interfaces and enums");
        }
        if (typeParameters.size() > 0 && typeParameters.size() != typeDeclaration.getTypeParameters().size()) {
            throw new IllegalArgumentException(String.format("expected either zero type parameters or has many as defined in the declaration (%d). Found %d",
                    typeDeclaration.getTypeParameters().size(), typeParameters.size()));
        }
        TypeParametersMap.Builder typeParametersMapBuilder = new TypeParametersMap.Builder();
        for (int i = 0; i < typeParameters.size(); i++) {
            typeParametersMapBuilder.setValue(typeDeclaration.getTypeParameters().get(i), typeParameters.get(i));
        }
        this.typeParametersMap = typeParametersMapBuilder.build();
        this.typeDeclaration = typeDeclaration;
        this.typeSolver = typeSolver;
    }

    //
    // Public Object methods
    //

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        ReferenceType that = (ReferenceType) o;

        if (!typeDeclaration.equals(that.typeDeclaration)) return false;
        if (!typeParametersMap.equals(that.typeParametersMap)) return false;

        return true;
    }

    @Override
    public int hashCode() {
        int result = typeDeclaration.hashCode();
        result = 31 * result + typeParametersMap.hashCode();
        return result;
    }

    @Override
    public String toString() {
        return "ReferenceType{" + getQualifiedName() +
                ", typeParametersMap=" + typeParametersMap +
                '}';
    }

    ///
    /// Relation with other types
    ///

    @Override
    public final boolean isReferenceType() {
        return true;
    }

    ///
    /// Downcasting
    ///

    @Override
    public ReferenceType asReferenceType() {
        return this;
    }

    ///
    /// Naming
    ///

    @Override
    public String describe() {
        StringBuilder sb = new StringBuilder();
        if (hasName()) {
            sb.append(typeDeclaration.getQualifiedName());
        } else {
            sb.append("<anonymous class>");
        }
        if (!typeParametersMap().isEmpty()) {
            sb.append("<");
            sb.append(String.join(", ", typeDeclaration.getTypeParameters().stream()
                    .map(tp -> typeParametersMap().getValue(tp).describe())
                    .collect(Collectors.toList())));
            sb.append(">");
        }
        return sb.toString();
    }

    ///
    /// TypeParameters
    ///

    /**
     * Execute a transformation on all the type parameters of this element.
     */
    public Type transformTypeParameters(TypeTransformer transformer) {
        Type result = this;
        int i = 0;
        for (Type tp : this.typeParametersValues()) {
            Type transformedTp = transformer.transform(tp);
            // Identity comparison on purpose
            if (transformedTp != tp) {
                List<Type> typeParametersCorrected = result.asReferenceType().typeParametersValues();
                typeParametersCorrected.set(i, transformedTp);
                result = create(typeDeclaration, typeParametersCorrected, typeSolver);
            }
            i++;
        }
        return result;
    }

    @Override
    public Type replaceTypeVariables(TypeParameterDeclaration tpToReplace, Type replaced, Map<TypeParameterDeclaration, Type> inferredTypes) {
        if (replaced == null) {
            throw new IllegalArgumentException();
        }

        ReferenceType result = this;
        int i = 0;
        for (Type tp : this.typeParametersValues()) {
            Type transformedTp = tp.replaceTypeVariables(tpToReplace, replaced, inferredTypes);
            // Identity comparison on purpose
            if (tp.isTypeVariable() && tp.asTypeVariable().describe().equals(tpToReplace.getName())) {
                inferredTypes.put(tp.asTypeParameter(), replaced);
            }
            if (true) {
                List<Type> typeParametersCorrected = result.asReferenceType().typeParametersValues();
                typeParametersCorrected.set(i, transformedTp);
                result = create(typeDeclaration, typeParametersCorrected, typeSolver);
            }
            i++;
        }

        List<Type> values = result.typeParametersValues();
        if(values.contains(tpToReplace)){
            int index = values.indexOf(tpToReplace);
            values.set(index, replaced);
            return create(result.getTypeDeclaration(), values, typeSolver);
        }


        return result;
    }

    ///
    /// Assignability
    ///

    /**
     * This method checks if ThisType t = new OtherType() would compile.
     */
    @Override
    public abstract boolean isAssignableBy(Type other);

    ///
    /// Ancestors
    ///

    /**
     * Return all ancestors, that means all superclasses and interfaces.
     * This list should always include Object (unless this is a reference to Object).
     * The type typeParametersValues should be expressed in terms of this type typeParametersValues.
     * <p>
     * For example, given:
     * <p>
     * class Foo&lt;A, B&gt; {}
     * class Bar&lt;C&gt; extends Foo&lt;C, String&gt; {}
     * <p>
     * a call to getAllAncestors on a reference to Bar having type parameter Boolean should include
     * Foo&lt;Boolean, String&gt;.
     */
    public List<ReferenceType> getAllAncestors() {
        // We need to go through the inheritance line and propagate the type parametes

        List<ReferenceType> ancestors = typeDeclaration.getAllAncestors();

        ancestors = ancestors.stream()
                .map(a -> typeParametersMap().replaceAll(a).asReferenceType())
                .collect(Collectors.toList());

        // Avoid repetitions of Object
        ancestors.removeIf(a -> a.getQualifiedName().equals(Object.class.getCanonicalName()));
        ReferenceTypeDeclaration objectType = typeSolver.solveType(Object.class.getCanonicalName());
        ReferenceType objectRef = create(objectType, typeSolver);
        ancestors.add(objectRef);
        return ancestors;
    }

    public final List<ReferenceType> getAllInterfacesAncestors() {
        return getAllAncestors().stream()
                .filter(it -> it.getTypeDeclaration().isInterface())
                .collect(Collectors.toList());
    }

    public final List<ReferenceType> getAllClassesAncestors() {
        return getAllAncestors().stream()
                .filter(it -> it.getTypeDeclaration().isClass())
                .collect(Collectors.toList());
    }

    ///
    /// Type parameters
    ///

    /**
     * Get the type associated with the type parameter with the given name.
     * It returns Optional.empty unless the type declaration declares a type parameter with the given name.
     */
    public Optional<Type> getGenericParameterByName(String name) {
        for (TypeParameterDeclaration tp : typeDeclaration.getTypeParameters()) {
            if (tp.getName().equals(name)) {
                return Optional.of(this.typeParametersMap().getValue(tp));
            }
        }
        return Optional.empty();
    }

    /**
     * Get the values for all type parameters declared on this type.
     * The list can be empty for raw types.
     */
    public List<Type> typeParametersValues() {
        return this.typeParametersMap.isEmpty() ? Collections.emptyList() : typeDeclaration.getTypeParameters().stream().map(tp -> typeParametersMap.getValue(tp)).collect(Collectors.toList());
    }

    /**
     * Get the values for all type parameters declared on this type.
     * In case of raw types the values correspond to TypeVariables.
     */
    public List<Tuple2<TypeParameterDeclaration, Type>> getTypeParametersMap() {
        List<Tuple2<TypeParameterDeclaration, Type>> typeParametersMap = new ArrayList<>();
        if (!isRawType()) {
	        for (int i = 0; i < typeDeclaration.getTypeParameters().size(); i++) {
	            typeParametersMap.add(new Tuple2<>(typeDeclaration.getTypeParameters().get(0), typeParametersValues().get(i)));
	        }
        }
        return typeParametersMap;
    }

    @Override
    public TypeParametersMap typeParametersMap() {
        return typeParametersMap;
    }

    ///
    /// Other methods introduced by ReferenceType
    ///

    /**
     * Corresponding TypeDeclaration
     */
    public final ReferenceTypeDeclaration getTypeDeclaration() {
        return typeDeclaration;
    }

    /**
     * The type of the field could be different from the one in the corresponding FieldDeclaration because
     * type variables would be solved.
     */
    public Optional<Type> getFieldType(String name) {
        if (!typeDeclaration.hasField(name)) {
            return Optional.empty();
        }
        Type type = typeDeclaration.getField(name).getType();
        type = useThisTypeParametersOnTheGivenType(type);
        return Optional.of(type);
    }

    /**
     * Has the TypeDeclaration a name? Anonymous classes do not have one.
     */
    public boolean hasName() {
        return typeDeclaration.hasName();
    }

    /**
     * Qualified name of the declaration.
     */
    public String getQualifiedName() {
        return typeDeclaration.getQualifiedName();
    }

    /**
     * Id of the declaration. It corresponds to the qualified name, unless for local classes.
     */
    public String getId() {
        return typeDeclaration.getId();
    }

    /**
     * Methods declared on this type.
     */
    public abstract Set<MethodUsage> getDeclaredMethods();

    public boolean isRawType() {
        if (!typeDeclaration.getTypeParameters().isEmpty()) {
            if (typeParametersMap().isEmpty()) {
                return true;
            }
            for (String name : typeParametersMap().getNames()) {
                Optional<Type> value = typeParametersMap().getValueBySignature(name);
                if (value.isPresent() && value.get().isTypeVariable() && value.get().asTypeVariable().qualifiedName().equals(name)) {
                    // nothing to do
                } else {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    public Optional<Type> typeParamValue(TypeParameterDeclaration typeParameterDeclaration) {
        if (typeParameterDeclaration.declaredOnMethod()) {
            throw new IllegalArgumentException();
        }
        String typeId = this.getTypeDeclaration().getId();
        if (typeId.equals(typeParameterDeclaration.getContainerId())) {
            return Optional.of(this.typeParametersMap().getValue(typeParameterDeclaration));
        }
        for (ReferenceType ancestor : this.getAllAncestors()) {
            if (ancestor.getId().equals(typeParameterDeclaration.getContainerId())) {
                return Optional.of(ancestor.typeParametersMap().getValue(typeParameterDeclaration));
            }
        }
        return Optional.empty();
    }

    //
    // Protected methods
    //

    protected abstract ReferenceType create(ReferenceTypeDeclaration typeDeclaration, List<Type> typeParameters, TypeSolver typeSolver);

    protected ReferenceType create(ReferenceTypeDeclaration typeDeclaration, TypeParametersMap typeParametersMap, TypeSolver typeSolver) {
        return create(typeDeclaration, typeDeclaration.getTypeParameters().stream()
                .map(tp -> typeParametersMap.getValue(tp))
                .collect(Collectors.toList()), typeSolver);
    }

    protected abstract ReferenceType create(ReferenceTypeDeclaration typeDeclaration, TypeSolver typeSolver);

    protected boolean isCorrespondingBoxingType(String typeName) {
        switch (typeName) {
            case "boolean":
                return getQualifiedName().equals(Boolean.class.getCanonicalName());
            case "char":
                return getQualifiedName().equals(Character.class.getCanonicalName());
            case "byte":
                return getQualifiedName().equals(Byte.class.getCanonicalName());
            case "short":
                return getQualifiedName().equals(Short.class.getCanonicalName());
            case "int":
                return getQualifiedName().equals(Integer.class.getCanonicalName());
            case "long":
                return getQualifiedName().equals(Long.class.getCanonicalName());
            case "float":
                return getQualifiedName().equals(Float.class.getCanonicalName());
            case "double":
                return getQualifiedName().equals(Double.class.getCanonicalName());
            default:
                throw new UnsupportedOperationException(typeName);
        }
    }

    protected boolean compareConsideringTypeParameters(ReferenceType other) {
        if (other.equals(this)) {
            return true;
        }
        if (this.getQualifiedName().equals(other.getQualifiedName())) {
            if (this.isRawType() || other.isRawType()) {
                return true;
            }
            if (this.typeParametersValues().size() != other.typeParametersValues().size()) {
                throw new IllegalStateException();
            }
            for (int i = 0; i < typeParametersValues().size(); i++) {
                Type thisParam = typeParametersValues().get(i);
                Type otherParam = other.typeParametersValues().get(i);
                if (!thisParam.equals(otherParam)) {
                    if (thisParam instanceof Wildcard) {
                        Wildcard thisParamAsWildcard = (Wildcard) thisParam;
                        if (thisParamAsWildcard.isSuper() && otherParam.isAssignableBy(thisParamAsWildcard.getBoundedType())) {
                            // ok
                        } else if (thisParamAsWildcard.isExtends() && thisParamAsWildcard.getBoundedType().isAssignableBy(otherParam)) {
                            // ok
                        } else if (!thisParamAsWildcard.isBounded()) {
                            // ok
                        } else {
                            return false;
                        }
                    } else {
                        if (thisParam instanceof TypeVariable && otherParam instanceof TypeVariable) {
                            List<Type> thisBounds = thisParam.asTypeVariable().asTypeParameter().getBounds(this.typeSolver).stream().map(bound -> bound.getType()).collect(Collectors.toList());
                            List<Type> otherBounds = otherParam.asTypeVariable().asTypeParameter().getBounds(other.typeSolver).stream().map(bound -> bound.getType()).collect(Collectors.toList());
                            if (thisBounds.size() == otherBounds.size() && otherBounds.containsAll(thisBounds)) {
                                return true;
                            }
                        }
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    //
    // Private methods
    //

    private static List<Type> deriveParams(ReferenceTypeDeclaration typeDeclaration) {
        return typeDeclaration.getTypeParameters().stream().map((tp) -> new TypeVariable(tp)).collect(Collectors.toList());
    }

    public ReferenceType deriveTypeParameters(TypeParametersMap typeParametersMap) {
        return create(typeDeclaration, typeParametersMap, typeSolver);
    }
}
