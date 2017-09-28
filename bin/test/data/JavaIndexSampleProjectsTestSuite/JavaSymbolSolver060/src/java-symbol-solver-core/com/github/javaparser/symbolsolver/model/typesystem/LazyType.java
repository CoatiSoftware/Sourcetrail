package com.github.javaparser.symbolsolver.model.typesystem;

import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;

import java.util.Map;
import java.util.function.Function;

public class LazyType implements Type {
    private Type concrete;
    private Function<Void, Type> provider;

    public LazyType(Function<Void, Type> provider) {
        this.provider = provider;
    }

    private Type getType() {
        if (concrete == null) {
            concrete = provider.apply(null);
        }
        return concrete;
    }

    @Override
    public boolean isArray() {
        return getType().isArray();
    }

    @Override
    public int arrayLevel() {
        return getType().arrayLevel();
    }

    @Override
    public boolean isPrimitive() {
        return getType().isPrimitive();
    }

    @Override
    public boolean isNull() {
        return getType().isNull();
    }

    @Override
    public boolean isReference() {
        return getType().isReference();
    }

    @Override
    public boolean isReferenceType() {
        return getType().isReferenceType();
    }

    @Override
    public boolean isVoid() {
        return getType().isVoid();
    }

    @Override
    public boolean isTypeVariable() {
        return getType().isTypeVariable();
    }

    @Override
    public boolean isWildcard() {
        return getType().isArray();
    }

    @Override
    public ArrayType asArrayType() {
        return getType().asArrayType();
    }

    @Override
    public ReferenceType asReferenceType() {
        return getType().asReferenceType();
    }

    @Override
    public TypeParameterDeclaration asTypeParameter() {
        return getType().asTypeParameter();
    }

    @Override
    public TypeVariable asTypeVariable() {
        return getType().asTypeVariable();
    }

    @Override
    public PrimitiveType asPrimitive() {
        return getType().asPrimitive();
    }

    @Override
    public Wildcard asWildcard() {
        return getType().asWildcard();
    }

    @Override
    public String describe() {
        return getType().describe();
    }

    @Override
    public Type replaceTypeVariables(TypeParameterDeclaration tp, Type replaced, Map<TypeParameterDeclaration, Type> inferredTypes) {
        return getType().replaceTypeVariables(tp, replaced, inferredTypes);
    }

    @Override
    public Type replaceTypeVariables(TypeParameterDeclaration tp, Type replaced) {
        return getType().replaceTypeVariables(tp, replaced);
    }

    @Override
    public boolean isAssignableBy(Type other) {
        return getType().isAssignableBy(other);
    }
}
