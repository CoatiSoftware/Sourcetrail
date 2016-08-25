package com.github.javaparser.ast.nodeTypes;

import java.util.ArrayList;
import java.util.List;

import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.Type;

public interface NodeWithParameters<T> {
    List<Parameter> getParameters();

    T setParameters(List<Parameter> parameters);

    default T addParameter(Type type, String name) {
        return addParameter(new Parameter(type, new VariableDeclaratorId(name)));
    }

    default T addParameter(Class<?> paramClass, String name) {
        ((Node) this).tryAddImportToParentCompilationUnit(paramClass);
        return addParameter(new ClassOrInterfaceType(paramClass.getSimpleName()), name);
    }

    @SuppressWarnings("unchecked")
    default T addParameter(Parameter parameter) {
        getParameters().add(parameter);
        parameter.setParentNode((Node) this);
        return (T) this;
    }

    default Parameter addAndGetParameter(Type type, String name) {
        return addAndGetParameter(new Parameter(type, new VariableDeclaratorId(name)));
    }

    default Parameter addAndGetParameter(Class<?> paramClass, String name) {
        ((Node) this).tryAddImportToParentCompilationUnit(paramClass);
        return addAndGetParameter(new ClassOrInterfaceType(paramClass.getSimpleName()), name);
    }

    default Parameter addAndGetParameter(Parameter parameter) {
        getParameters().add(parameter);
        parameter.setParentNode((Node) this);
        return parameter;
    }

    /**
     * Try to find a {@link Parameter} by its name
     * 
     * @param name the name of the param
     * @return null if not found, the param found otherwise
     */
    default Parameter getParamByName(String name){
        return getParameters().stream()
                .filter(p -> p.getName().equals(name)).findFirst().orElse(null);
    }

    /**
     * Try to find a {@link Parameter} by its type
     * 
     * @param type the type of the param
     * @return null if not found, the param found otherwise
     */
    default Parameter getParamByType(String type) {
        return getParameters().stream()
                .filter(p -> p.getType().toString().equals(type)).findFirst().orElse(null);
    }

    /**
     * Try to find a {@link Parameter} by its type
     * 
     * @param type the type of the param <b>take care about generics, it wont work</b>
     * @return null if not found, the param found otherwise
     */
    default Parameter getParamByType(Class<?> type) {
        return getParameters().stream()
                .filter(p -> p.getType().toString().equals(type.getSimpleName())).findFirst().orElse(null);
    }
}
