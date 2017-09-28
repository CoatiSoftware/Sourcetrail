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

package com.github.javaparser.symbolsolver.javaparsermodel.declarations;

import com.github.javaparser.symbolsolver.model.declarations.*;

import java.util.Collections;
import java.util.List;

/**
 * @author Federico Tomassetti
 */
class DefaultConstructorDeclaration implements ConstructorDeclaration {

    private ClassDeclaration classDeclaration;

    DefaultConstructorDeclaration(ClassDeclaration classDeclaration) {
        this.classDeclaration = classDeclaration;
    }

    @Override
    public ClassDeclaration declaringType() {
        return classDeclaration;
    }

    @Override
    public int getNumberOfParams() {
        return 0;
    }

    @Override
    public ParameterDeclaration getParam(int i) {
        throw new UnsupportedOperationException();
    }

    @Override
    public String getName() {
        return classDeclaration.getName();
    }

    @Override
    public AccessLevel accessLevel() {
        return AccessLevel.PUBLIC;
    }

    @Override
    public List<TypeParameterDeclaration> getTypeParameters() {
        return Collections.emptyList();
    }
}
