package com.github.javaparser.symbolsolver.model.typesystem.parametrization;

import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.typesystem.Type;
import com.github.javaparser.symbolsolver.model.typesystem.Wildcard;

import java.util.Optional;

/**
 * @author Federico Tomassetti
 */
public interface TypeParameterValueProvider {

    /**
     * Calculate the value for the given type parameter.
     * It could be inherited.
     */
    Optional<Type> typeParamValue(TypeParameterDeclaration typeParameterDeclaration);

    /**
     * Replace the type typeParametersValues present in the given type with the ones for which this type
     * has a value.
     */
    default Type useThisTypeParametersOnTheGivenType(Type type) {
        if (type.isTypeVariable()) {
            TypeParameterDeclaration typeParameter = type.asTypeParameter();
            if (typeParameter.declaredOnType()) {
                Optional<Type> typeParam = typeParamValue(typeParameter);
                if (typeParam.isPresent()) {
                    type = typeParam.get();
                }
            }
        }

        if (type.isWildcard() && type.asWildcard().isBounded()) {
            if (type.asWildcard().isExtends()) {
                return Wildcard.extendsBound(useThisTypeParametersOnTheGivenType(type.asWildcard().getBoundedType()));
            } else {
                return Wildcard.superBound(useThisTypeParametersOnTheGivenType(type.asWildcard().getBoundedType()));
            }
        }

        if (type.isReferenceType()) {
            type = type.asReferenceType().transformTypeParameters(tp -> useThisTypeParametersOnTheGivenType(tp));
        }

        return type;
    }

    Optional<Type> getGenericParameterByName(String name);
}
