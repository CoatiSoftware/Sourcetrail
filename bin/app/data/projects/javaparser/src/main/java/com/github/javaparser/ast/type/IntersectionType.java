package com.github.javaparser.ast.type;

import com.github.javaparser.Range;
import com.github.javaparser.ast.visitor.GenericVisitor;
import com.github.javaparser.ast.visitor.VoidVisitor;

import java.util.List;

/**
 * Represents a set of types. A given value of this type has to be assignable to at all of the element types.
 * As of Java 8 it is used in casts or while expressing bounds for generic types.
 *
 * For example:
 * public class A&gt;T extends Serializable &amp; Cloneable&lt; { }
 *
 * Or:
 * void foo((Serializable &amp; Cloneable)myObject);
 *
 * @since 3.0.0
 */
public class IntersectionType extends Type {

    private List<ReferenceType> elements;

    public IntersectionType(Range range, List<ReferenceType> elements) {
        super(range);
        setElements(elements);
    }

    public IntersectionType(List<ReferenceType> elements) {
        super();
        setElements(elements);
    }

    @Override
    public <R, A> R accept(GenericVisitor<R, A> v, A arg) {
        return v.visit(this, arg);
    }

    @Override
    public <A> void accept(VoidVisitor<A> v, A arg) {
        v.visit(this, arg);
    }

    public List<ReferenceType> getElements() {
        return elements;
    }

    public void setElements(List<ReferenceType> elements) {
        if (this.elements != null) {
            for (ReferenceType element : elements){
                element.setParentNode(null);
            }
        }
        this.elements = elements;
        setAsParentNodeOf(this.elements);
    }
}
