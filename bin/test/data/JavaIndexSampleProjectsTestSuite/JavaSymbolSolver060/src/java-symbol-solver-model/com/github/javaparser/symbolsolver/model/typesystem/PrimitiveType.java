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

import com.google.common.collect.ImmutableList;

import java.util.List;

/**
 * @author Federico Tomassetti
 */
public class PrimitiveType implements Type {

    ///
    /// Constants
    ///

    public static final PrimitiveType BYTE = new PrimitiveType("byte", Byte.class.getCanonicalName(), ImmutableList.of());
    public static final PrimitiveType SHORT = new PrimitiveType("short", Short.class.getCanonicalName(), ImmutableList.of(BYTE));
    public static final PrimitiveType CHAR = new PrimitiveType("char", Character.class.getCanonicalName(), ImmutableList.of());
    public static final PrimitiveType INT = new PrimitiveType("int", Integer.class.getCanonicalName(), ImmutableList.of(BYTE, SHORT, CHAR));
    public static final PrimitiveType LONG = new PrimitiveType("long", Long.class.getCanonicalName(), ImmutableList.of(BYTE, SHORT, INT, CHAR));
    public static final PrimitiveType BOOLEAN = new PrimitiveType("boolean", Boolean.class.getCanonicalName(), ImmutableList.of());
    public static final PrimitiveType FLOAT = new PrimitiveType("float", Float.class.getCanonicalName(), ImmutableList.of(LONG, INT, SHORT, BYTE, CHAR));
    public static final PrimitiveType DOUBLE = new PrimitiveType("double", Double.class.getCanonicalName(), ImmutableList.of(FLOAT, LONG, INT, SHORT, BYTE, CHAR));
    public static final List<PrimitiveType> ALL = ImmutableList.of(INT, BOOLEAN, LONG, CHAR, FLOAT, DOUBLE, SHORT, BYTE);

    ///
    /// Fields
    ///

    private String name;
    private String boxTypeQName;
    private List<PrimitiveType> promotionTypes;

    private PrimitiveType(String name, String boxTypeQName, List<PrimitiveType> promotionTypes) {
        this.name = name;
        this.boxTypeQName = boxTypeQName;
        this.promotionTypes = promotionTypes;
    }

    public static Type byName(String name) {
        name = name.toLowerCase();
        for (PrimitiveType ptu : ALL) {
            if (ptu.describe().equals(name)) {
                return ptu;
            }
        }
        throw new IllegalArgumentException("Name " + name);
    }

    @Override
    public String toString() {
        return "PrimitiveTypeUsage{" +
                "name='" + name + '\'' +
                '}';
    }

    public PrimitiveType asPrimitive() {
        return this;
    }

    @Override
    public boolean isArray() {
        return false;
    }

    @Override
    public boolean isPrimitive() {
        return true;
    }

    @Override
    public boolean isReferenceType() {
        return false;
    }

    @Override
    public String describe() {
        return name;
    }

    @Override
    public boolean isTypeVariable() {
        return false;
    }

    @Override
    public boolean isAssignableBy(Type other) {
        if (other.isPrimitive()) {
            return this == other || promotionTypes.contains(other);
        } else if (other.isReferenceType()) {
            if (other.asReferenceType().getQualifiedName().equals(boxTypeQName)) {
                return true;
            }
            for (PrimitiveType promotion : promotionTypes) {
                if (other.asReferenceType().getQualifiedName().equals(promotion.boxTypeQName)) {
                    return true;
                }
            }
            return false;
        } else if (other.isConstraint()){
            return this.isAssignableBy(other.asConstraintType().getBound());
        } else {
            return false;
        }
    }

    public String getBoxTypeQName() {
        return boxTypeQName;
    }

}
