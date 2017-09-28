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

import com.github.javaparser.symbolsolver.model.declarations.*;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import javassist.CtConstructor;
import javassist.NotFoundException;
import javassist.bytecode.BadBytecode;
import javassist.bytecode.SignatureAttribute;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

/**
 * @author Fred Lefévère-Laoide
 */
public class JavassistConstructorDeclaration implements ConstructorDeclaration {
    private CtConstructor ctConstructor;
    private TypeSolver typeSolver;

    public JavassistConstructorDeclaration(CtConstructor ctConstructor, TypeSolver typeSolver) {
        this.ctConstructor = ctConstructor;
        this.typeSolver = typeSolver;
    }

    @Override
    public String toString() {
        return "JavassistMethodDeclaration{" +
                "CtConstructor=" + ctConstructor +
                '}';
    }

    @Override
    public String getName() {
        return ctConstructor.getName();
    }

    @Override
    public boolean isField() {
        return false;
    }

    @Override
    public boolean isParameter() {
        return false;
    }

    @Override
    public boolean isType() {
        return false;
    }

    @Override
    public ClassDeclaration declaringType() {
        return new JavassistClassDeclaration(ctConstructor.getDeclaringClass(), typeSolver);
    }

    @Override
    public int getNumberOfParams() {
        try {
            return ctConstructor.getParameterTypes().length;
        } catch (NotFoundException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public ParameterDeclaration getParam(int i) {
        try {
            boolean variadic = false;
            if ((ctConstructor.getModifiers() & javassist.Modifier.VARARGS) > 0) {
                variadic = i == (ctConstructor.getParameterTypes().length - 1);
            }
            if (ctConstructor.getGenericSignature() != null) {
                SignatureAttribute.MethodSignature methodSignature = SignatureAttribute.toMethodSignature(ctConstructor.getGenericSignature());
                SignatureAttribute.Type signatureType = methodSignature.getParameterTypes()[i];
                return new JavassistParameterDeclaration(JavassistUtils.signatureTypeToType(signatureType, typeSolver, this), typeSolver, variadic);
            } else {
                return new JavassistParameterDeclaration(ctConstructor.getParameterTypes()[i], typeSolver, variadic);
            }
        } catch (NotFoundException e) {
            throw new RuntimeException(e);
        } catch (BadBytecode badBytecode) {
            throw new RuntimeException(badBytecode);
        }
    }

    @Override
    public List<TypeParameterDeclaration> getTypeParameters() {
        try {
            if (ctConstructor.getGenericSignature() == null) {
                return Collections.emptyList();
            }
            SignatureAttribute.MethodSignature methodSignature = SignatureAttribute.toMethodSignature(ctConstructor.getGenericSignature());
            return Arrays.stream(methodSignature.getTypeParameters()).map((jasTp) -> new JavassistTypeParameter(jasTp, this, typeSolver)).collect(Collectors.toList());
        } catch (BadBytecode badBytecode) {
            throw new RuntimeException(badBytecode);
        }
    }

    @Override
    public AccessLevel accessLevel() {
        return JavassistFactory.modifiersToAccessLevel(ctConstructor.getModifiers());
    }
}
