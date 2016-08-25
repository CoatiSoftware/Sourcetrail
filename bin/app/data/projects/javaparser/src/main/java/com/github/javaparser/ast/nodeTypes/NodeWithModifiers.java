package com.github.javaparser.ast.nodeTypes;

import java.util.Arrays;
import java.util.EnumSet;
import java.util.stream.Collectors;

import com.github.javaparser.ast.Modifier;

/**
 * A Node with Modifiers.
 */
public interface NodeWithModifiers<T> {
    /**
     * Return the modifiers of this variable declaration.
     *
     * @see Modifier
     * @return modifiers
     */
    EnumSet<Modifier> getModifiers();

    T setModifiers(EnumSet<Modifier> modifiers);

    @SuppressWarnings("unchecked")
    default T addModifier(Modifier... modifiers) {
        getModifiers().addAll(Arrays.stream(modifiers)
                .collect(Collectors.toCollection(() -> EnumSet.noneOf(Modifier.class))));
        return (T) this;
    }

    default boolean isStatic() {
        return getModifiers().contains(Modifier.STATIC);
    }

    default boolean isAbstract() {
        return getModifiers().contains(Modifier.ABSTRACT);
    }

    default boolean isFinal() {
        return getModifiers().contains(Modifier.FINAL);
    }

    default boolean isNative() {
        return getModifiers().contains(Modifier.NATIVE);
    }

    default boolean isPrivate() {
        return getModifiers().contains(Modifier.PRIVATE);
    }

    default boolean isProtected() {
        return getModifiers().contains(Modifier.PROTECTED);
    }

    default boolean isPublic() {
        return getModifiers().contains(Modifier.PUBLIC);
    }

    default boolean isStrictfp() {
        return getModifiers().contains(Modifier.STRICTFP);
    }

    default boolean isSynchronized() {
        return getModifiers().contains(Modifier.SYNCHRONIZED);
    }

    default boolean isTransient() {
        return getModifiers().contains(Modifier.TRANSIENT);
    }

    default boolean isVolatile() {
        return getModifiers().contains(Modifier.VOLATILE);
    }
}