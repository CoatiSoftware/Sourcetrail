package com.github.javaparser.symbolsolver.logic;

import com.github.javaparser.symbolsolver.model.typesystem.Type;

/**
 * @author Federico Tomassetti
 */
public class ConfilictingGenericTypesException extends RuntimeException {

    public ConfilictingGenericTypesException(Type formalType, Type actualType) {
        super(String.format("No matching between %s (formal) and %s (actual)", formalType, actualType));
    }
}
