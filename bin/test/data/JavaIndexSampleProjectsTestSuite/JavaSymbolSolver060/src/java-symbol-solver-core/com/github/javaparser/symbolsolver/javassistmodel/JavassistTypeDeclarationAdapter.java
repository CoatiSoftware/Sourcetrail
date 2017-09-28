/*
 * Copyright 2016 Federico Tomassetti
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

package com.github.javaparser.symbolsolver.javassistmodel;

import com.github.javaparser.symbolsolver.model.declarations.*;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import javassist.CtClass;
import javassist.NotFoundException;
import javassist.bytecode.BadBytecode;
import javassist.bytecode.SignatureAttribute;

import java.util.*;
import java.util.stream.Collectors;

/**
 * @author Federico Tomassetti
 */
public class JavassistTypeDeclarationAdapter {

  private CtClass ctClass;
  private TypeSolver typeSolver;

  public JavassistTypeDeclarationAdapter(CtClass ctClass, TypeSolver typeSolver) {
    this.ctClass = ctClass;
    this.typeSolver = typeSolver;
  }

  public Set<MethodDeclaration> getDeclaredMethods() {
    return Arrays.stream(ctClass.getDeclaredMethods())
        .map(m -> new JavassistMethodDeclaration(m, typeSolver)).collect(Collectors.toSet());
  }

  public List<ConstructorDeclaration> getConstructors() {
    return Arrays.stream(ctClass.getConstructors())
        .map(m -> new JavassistConstructorDeclaration(m, typeSolver)).collect(Collectors.toList());
  }

  public List<FieldDeclaration> getDeclaredFields() {
    List<FieldDeclaration> fieldDecls = new ArrayList<>();
    collectDeclaredFields(ctClass, fieldDecls);
    return fieldDecls;
  }

  private void collectDeclaredFields(CtClass ctClass, List<FieldDeclaration> fieldDecls) {
    if (ctClass != null) {
      Arrays.stream(ctClass.getDeclaredFields())
          .forEach(f -> fieldDecls.add(new JavassistFieldDeclaration(f, typeSolver)));
      try {
        collectDeclaredFields(ctClass.getSuperclass(), fieldDecls);
      } catch (NotFoundException e) {
        // We'll stop here
      }
    }
  }

  public List<TypeParameterDeclaration> getTypeParameters() {
    if (null == ctClass.getGenericSignature()) {
      return Collections.emptyList();
    } else {
      try {
        SignatureAttribute.ClassSignature classSignature =
            SignatureAttribute.toClassSignature(ctClass.getGenericSignature());
        return Arrays.<SignatureAttribute.TypeParameter>stream(classSignature.getParameters())
            .map((tp) -> new JavassistTypeParameter(tp, JavassistFactory.toTypeDeclaration(ctClass, typeSolver), typeSolver))
            .collect(Collectors.toList());
      } catch (BadBytecode badBytecode) {
        throw new RuntimeException(badBytecode);
      }
    }
  }

  public Optional<ReferenceTypeDeclaration> containerType() {
    try {
      return ctClass.getDeclaringClass() == null ?
          Optional.empty() :
          Optional.of(JavassistFactory.toTypeDeclaration(ctClass.getDeclaringClass(), typeSolver));
    } catch (NotFoundException e) {
      throw new RuntimeException(e);
    }
  }
}
