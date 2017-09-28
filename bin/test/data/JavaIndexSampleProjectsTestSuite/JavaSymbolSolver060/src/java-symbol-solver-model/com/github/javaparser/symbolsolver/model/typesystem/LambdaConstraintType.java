package com.github.javaparser.symbolsolver.model.typesystem;

public class LambdaConstraintType  implements Type {
    Type bound;

    public LambdaConstraintType(Type bound) {
        this.bound = bound;
    }

    @Override
    public String describe() {
        return "? super " + bound.describe();
    }

    public Type getBound() {
        return bound;
    }

    @Override
    public boolean isConstraint() {
        return true;
    }

    @Override
    public LambdaConstraintType asConstraintType() {
        return this;
    }

    public static LambdaConstraintType bound(Type bound){
        return new LambdaConstraintType(bound);
    }

    @Override
    public boolean isAssignableBy(Type other) {
        return bound.isAssignableBy(other);
    }
}