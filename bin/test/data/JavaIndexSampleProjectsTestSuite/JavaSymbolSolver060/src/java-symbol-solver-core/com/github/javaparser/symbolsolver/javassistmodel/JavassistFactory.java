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

import com.github.javaparser.symbolsolver.model.declarations.AccessLevel;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.typesystem.*;
import javassist.CtClass;
import javassist.NotFoundException;

import java.lang.reflect.Modifier;

/**
 * @author Federico Tomassetti
 */
public class JavassistFactory {

  public static Type typeUsageFor(CtClass ctClazz, TypeSolver typeSolver) {
    try {
      if (ctClazz.isArray()) {
        return new ArrayType(typeUsageFor(ctClazz.getComponentType(), typeSolver));
      } else if (ctClazz.isPrimitive()) {
        if (ctClazz.getName().equals("void")) {
          return VoidType.INSTANCE;
        } else {
          return PrimitiveType.byName(ctClazz.getName());
        }
      } else {
        if (ctClazz.isInterface()) {
          return new ReferenceTypeImpl(new JavassistInterfaceDeclaration(ctClazz, typeSolver),
              typeSolver);
        } else if (ctClazz.isEnum()) {
          return new ReferenceTypeImpl(new JavassistEnumDeclaration(ctClazz, typeSolver),
              typeSolver);
        } else {
          return new ReferenceTypeImpl(new JavassistClassDeclaration(ctClazz, typeSolver),
              typeSolver);
        }
      }
    } catch (NotFoundException e) {
      throw new RuntimeException(e);
    }
  }

  public static ReferenceTypeDeclaration toTypeDeclaration(CtClass ctClazz, TypeSolver typeSolver) {
    if (ctClazz.isInterface()) {
      return new JavassistInterfaceDeclaration(ctClazz, typeSolver);
    } else if (ctClazz.isEnum()) {
      return new JavassistEnumDeclaration(ctClazz, typeSolver);
    } else if (ctClazz.isAnnotation()) {
      throw new UnsupportedOperationException("CtClass of annotation not yet supported");
    } else if (ctClazz.isArray()) {
      throw new IllegalArgumentException("This method should not be called passing an array");
    } else {
      return new JavassistClassDeclaration(ctClazz, typeSolver);
    }
  }

  static AccessLevel modifiersToAccessLevel(final int modifiers) {
    if (Modifier.isPublic(modifiers)) {
      return AccessLevel.PUBLIC;
    } else if (Modifier.isProtected(modifiers)) {
      return AccessLevel.PROTECTED;
    } else if (Modifier.isPrivate(modifiers)) {
      return AccessLevel.PRIVATE;
    } else {
      return AccessLevel.PACKAGE_PROTECTED;
    }
  }

}
