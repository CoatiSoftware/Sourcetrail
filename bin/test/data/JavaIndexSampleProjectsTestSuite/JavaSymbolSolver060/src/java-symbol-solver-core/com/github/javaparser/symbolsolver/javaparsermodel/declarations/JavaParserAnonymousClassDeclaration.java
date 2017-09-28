package com.github.javaparser.symbolsolver.javaparsermodel.declarations;

import static com.github.javaparser.symbolsolver.javaparser.Navigator.getParentNode;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.expr.ObjectCreationExpr;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFacade;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFactory;
import com.github.javaparser.symbolsolver.logic.AbstractClassDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.AccessLevel;
import com.github.javaparser.symbolsolver.model.declarations.ConstructorDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.FieldDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.TypeParameterDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.typesystem.ReferenceType;
import com.github.javaparser.symbolsolver.model.typesystem.ReferenceTypeImpl;
import com.github.javaparser.symbolsolver.model.typesystem.Type;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.Lists;

import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;
import java.util.stream.Collectors;

/**
 * An anonymous class declaration representation.
 */
public class JavaParserAnonymousClassDeclaration extends AbstractClassDeclaration {

  private final TypeSolver typeSolver;
  private final ObjectCreationExpr wrappedNode;
  private final TypeDeclaration superTypeDeclaration;
  private final String name = "Anonymous-" + UUID.randomUUID();

  public JavaParserAnonymousClassDeclaration(ObjectCreationExpr wrappedNode,
                                             TypeSolver typeSolver) {
    this.typeSolver = typeSolver;
    this.wrappedNode = wrappedNode;
    superTypeDeclaration =
        JavaParserFactory.getContext(wrappedNode.getParentNode().get(), typeSolver)
                         .solveType(wrappedNode.getType().getName().getId(), typeSolver)
                         .getCorrespondingDeclaration();
  }

  public TypeDeclaration getSuperTypeDeclaration() {
    return superTypeDeclaration;
  }

  public <T extends Node> List<T> findMembersOfKind(final Class<T> memberClass) {
    return wrappedNode
        .getAnonymousClassBody()
        .get()
        .stream()
        .filter(node -> memberClass.isAssignableFrom(node.getClass()))
        .map(node -> (T) node)
        .collect(Collectors.toList());
  }

  @Override
  protected ReferenceType object() {
    return new ReferenceTypeImpl(typeSolver.solveType(Object.class.getCanonicalName()), typeSolver);
  }

  @Override
  public ReferenceType getSuperClass() {
    return new ReferenceTypeImpl(superTypeDeclaration.asReferenceType(), typeSolver);
  }

  @Override
  public List<ReferenceType> getInterfaces() {
    return
        superTypeDeclaration
            .asReferenceType().getAncestors()
            .stream()
            .filter(type -> type.getTypeDeclaration().isInterface())
            .collect(Collectors.toList());
  }

  @Override
  public List<ConstructorDeclaration> getConstructors() {
    return
        findMembersOfKind(com.github.javaparser.ast.body.ConstructorDeclaration.class)
            .stream()
            .map(ctor -> new JavaParserConstructorDeclaration(this, ctor, typeSolver))
            .collect(Collectors.toList());
  }

  @Override
  public AccessLevel accessLevel() {
    return AccessLevel.PRIVATE;
  }

  @Override
  public List<ReferenceType> getAncestors() {
    return
        ImmutableList.
            <ReferenceType>builder()
            .add(getSuperClass())
            .addAll(superTypeDeclaration.asReferenceType().getAncestors())
            .build();
  }

  @Override
  public List<FieldDeclaration> getAllFields() {

    List<JavaParserFieldDeclaration> myFields =
        findMembersOfKind(com.github.javaparser.ast.body.FieldDeclaration.class)
            .stream()
            .flatMap(field ->
                         field.getVariables().stream()
                              .map(variable -> new JavaParserFieldDeclaration(variable,
                                                                              typeSolver)))
            .collect(Collectors.toList());

    List<FieldDeclaration> superClassFields =
        getSuperClass().getTypeDeclaration().getAllFields();

    List<FieldDeclaration> interfaceFields =
        getInterfaces().stream()
                       .flatMap(inteface -> inteface.getTypeDeclaration().getAllFields().stream())
                       .collect(Collectors.toList());

    return
        ImmutableList
        .<FieldDeclaration>builder()
        .addAll(myFields)
        .addAll(superClassFields)
        .addAll(interfaceFields)
        .build();
  }

  @Override
  public Set<MethodDeclaration> getDeclaredMethods() {
    return
        findMembersOfKind(com.github.javaparser.ast.body.MethodDeclaration.class)
            .stream()
            .map(method -> new JavaParserMethodDeclaration(method, typeSolver))
            .collect(Collectors.toSet());
  }

  @Override
  public boolean isAssignableBy(Type type) {
    return false;
  }

  @Override
  public boolean isAssignableBy(ReferenceTypeDeclaration other) {
    return false;
  }

  @Override
  public boolean hasDirectlyAnnotation(String qualifiedName) {
    return false;
  }

  @Override
  public String getPackageName() {
    return Helper.getPackageName(wrappedNode);
  }

  @Override
  public String getClassName() {
    return Helper.getClassName("", wrappedNode);
  }

  @Override
  public String getQualifiedName() {
    String containerName = Helper.containerName(getParentNode(wrappedNode));
    if (containerName.isEmpty()) {
      return getName();
    } else {
      return containerName + "." + getName();
    }
  }

  @Override
  public Set<ReferenceTypeDeclaration> internalTypes() {
    return
        findMembersOfKind(com.github.javaparser.ast.body.TypeDeclaration.class)
            .stream()
            .map(typeMember -> JavaParserFacade.get(typeSolver).getTypeDeclaration(typeMember))
            .collect(Collectors.toSet());
  }

  @Override
  public String getName() {
    return name;
  }

  @Override
  public List<TypeParameterDeclaration> getTypeParameters() {
    return Lists.newArrayList();
  }

  @Override
  public Optional<ReferenceTypeDeclaration> containerType() {
    throw new UnsupportedOperationException("containerType is not supported for " + this.getClass().getCanonicalName());
  }
}
