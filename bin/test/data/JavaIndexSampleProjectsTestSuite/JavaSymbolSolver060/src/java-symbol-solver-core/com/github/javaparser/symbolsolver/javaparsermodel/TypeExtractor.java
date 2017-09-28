package com.github.javaparser.symbolsolver.javaparsermodel;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.NodeList;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.expr.*;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.ExpressionStmt;
import com.github.javaparser.ast.stmt.ReturnStmt;
import com.github.javaparser.ast.stmt.Statement;
import com.github.javaparser.ast.type.UnknownType;
import com.github.javaparser.symbolsolver.core.resolution.Context;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserSymbolDeclaration;
import com.github.javaparser.symbolsolver.logic.FunctionalInterfaceLogic;
import com.github.javaparser.symbolsolver.logic.InferenceContext;
import com.github.javaparser.symbolsolver.model.declarations.*;
import com.github.javaparser.symbolsolver.model.methods.MethodUsage;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.resolution.Value;
import com.github.javaparser.symbolsolver.model.typesystem.*;
import com.github.javaparser.symbolsolver.reflectionmodel.MyObjectProvider;
import com.github.javaparser.symbolsolver.reflectionmodel.ReflectionClassDeclaration;
import com.github.javaparser.symbolsolver.resolution.SymbolSolver;
import com.github.javaparser.symbolsolver.resolution.typesolvers.ReflectionTypeSolver;
import com.google.common.collect.ImmutableList;

import java.util.List;
import java.util.Optional;
import java.util.logging.ConsoleHandler;
import java.util.logging.Level;
import java.util.logging.Logger;

import static com.github.javaparser.symbolsolver.javaparser.Navigator.getParentNode;

public class TypeExtractor extends DefaultVisitorAdapter {

    private static Logger logger = Logger.getLogger(TypeExtractor.class.getCanonicalName());

    static {
        logger.setLevel(Level.INFO);
        ConsoleHandler consoleHandler = new ConsoleHandler();
        consoleHandler.setLevel(Level.INFO);
        logger.addHandler(consoleHandler);
    }

    private TypeSolver typeSolver;
    private JavaParserFacade facade;

    public TypeExtractor(TypeSolver typeSolver, JavaParserFacade facade) {
        this.typeSolver = typeSolver;
        this.facade = facade;
    }

    @Override
    public Type visit(VariableDeclarator node, Boolean solveLambdas) {
        if (getParentNode(node) instanceof FieldDeclaration) {
//                FieldDeclaration parent = (FieldDeclaration) getParentNode(node);
            return facade.convertToUsageVariableType(node);
        } else if (getParentNode(node) instanceof VariableDeclarationExpr) {
//                VariableDeclarationExpr parent = (VariableDeclarationExpr) getParentNode(node);
            return facade.convertToUsageVariableType(node);
        } else {
            throw new UnsupportedOperationException(getParentNode(node).getClass().getCanonicalName());
        }
    }

    @Override
    public Type visit(Parameter node, Boolean solveLambdas) {
        if (node.getType() instanceof UnknownType) {
            throw new IllegalStateException("Parameter has unknown type: " + node);
        }
        return facade.convertToUsage(node.getType(), node);
    }


    @Override
    public Type visit(ArrayAccessExpr node, Boolean solveLambdas) {
        Type arrayUsageType = node.getName().accept(this, solveLambdas);
        if (arrayUsageType.isArray()) {
            return ((ArrayType) arrayUsageType).getComponentType();
        }
        return arrayUsageType;
    }

    @Override
    public Type visit(ArrayCreationExpr node, Boolean solveLambdas) {
        Type res = facade.convertToUsage(node.getElementType(), JavaParserFactory.getContext(node, typeSolver));
        for (int i = 0; i < node.getLevels().size(); i++) {
            res = new ArrayType(res);
        }
        return res;
    }

    @Override
    public Type visit(ArrayInitializerExpr node, Boolean solveLambdas) {
        throw new UnsupportedOperationException(node.getClass().getCanonicalName());
    }

    @Override
    public Type visit(AssignExpr node, Boolean solveLambdas) {
        return node.getTarget().accept(this, solveLambdas);
    }

    @Override
    public Type visit(BinaryExpr node, Boolean solveLambdas) {
        switch (node.getOperator()) {
            case PLUS:
            case MINUS:
            case DIVIDE:
            case MULTIPLY:
                return facade.getBinaryTypeConcrete(node.getLeft(), node.getRight(), solveLambdas);
            case LESS_EQUALS:
            case LESS:
            case GREATER:
            case GREATER_EQUALS:
            case EQUALS:
            case NOT_EQUALS:
            case OR:
            case AND:
                return PrimitiveType.BOOLEAN;
            case BINARY_AND:
            case BINARY_OR:
            case SIGNED_RIGHT_SHIFT:
            case UNSIGNED_RIGHT_SHIFT:
            case LEFT_SHIFT:
            case REMAINDER:
            case XOR:
                return node.getLeft().accept(this, solveLambdas);
            default:
                throw new UnsupportedOperationException("FOO " + node.getOperator().name());
        }
    }

    @Override
    public Type visit(CastExpr node, Boolean solveLambdas) {
        return facade.convertToUsage(node.getType(), JavaParserFactory.getContext(node, typeSolver));
    }

    @Override
    public Type visit(ClassExpr node, Boolean solveLambdas) {
        // This implementation does not regard the actual type argument of the ClassExpr.
        com.github.javaparser.ast.type.Type astType = node.getType();
        Type jssType = facade.convertToUsage(astType, node.getType());
        return new ReferenceTypeImpl(new ReflectionClassDeclaration(Class.class, typeSolver), ImmutableList.of(jssType), typeSolver);
    }

    @Override
    public Type visit(ConditionalExpr node, Boolean solveLambdas) {
        return node.getThenExpr().accept(this, solveLambdas);
    }

    @Override
    public Type visit(EnclosedExpr node, Boolean solveLambdas) {
        return node.getInner().accept(this, solveLambdas);
    }

    /**
     * Java Parser can't differentiate between packages, internal types, and fields.
     * All three are lumped together into FieldAccessExpr. We need to differentiate them.
     */
    private Type solveDotExpressionType(ReferenceTypeDeclaration parentType, FieldAccessExpr node) {
        // Fields and internal type declarations cannot have the same name.
        // Thus, these checks will always be mutually exclusive.
        if (parentType.hasField(node.getName().getId())) {
            return parentType.getField(node.getName().getId()).getType();
        } else if (parentType.hasInternalType(node.getName().getId())) {
            return new ReferenceTypeImpl(parentType.getInternalType(node.getName().getId()), typeSolver);
        } else {
            throw new UnsolvedSymbolException(node.getName().getId());
        }
    }

    @Override
    public Type visit(FieldAccessExpr node, Boolean solveLambdas) {
        // We should understand if this is a static access
        if (node.getScope() instanceof NameExpr ||
                node.getScope() instanceof FieldAccessExpr) {
            Expression staticValue = node.getScope();
            SymbolReference<TypeDeclaration> typeAccessedStatically = JavaParserFactory.getContext(node, typeSolver).solveType(staticValue.toString(), typeSolver);
            if (typeAccessedStatically.isSolved()) {
                // TODO here maybe we have to substitute type typeParametersValues
                return solveDotExpressionType(
                        typeAccessedStatically.getCorrespondingDeclaration().asReferenceType(), node);
            }
        } else if (node.getScope()instanceof ThisExpr){
            // If we are accessing through a 'this' expression, first resolve the type
            // corresponding to 'this'
            SymbolReference<TypeDeclaration> solve = facade.solve((ThisExpr) node.getScope());
            // If found get it's declaration and get the field in there
            if (solve.isSolved()){
                TypeDeclaration correspondingDeclaration = solve.getCorrespondingDeclaration();
                if (correspondingDeclaration instanceof ReferenceTypeDeclaration){
                    return solveDotExpressionType(correspondingDeclaration.asReferenceType(), node);
                }
            }

        } else if (node.getScope().toString().indexOf('.') > 0) {
            // try to find fully qualified name
            SymbolReference<ReferenceTypeDeclaration> sr = typeSolver.tryToSolveType(node.getScope().toString());
            if (sr.isSolved()) {
                return solveDotExpressionType(sr.getCorrespondingDeclaration(), node);
            }
        }
        Optional<Value> value = null;
        try {
            value = new SymbolSolver(typeSolver).solveSymbolAsValue(node.getField().getId(), node);
        } catch (UnsolvedSymbolException use) {
            // This node may have a package name as part of its fully qualified name.
            // We should solve for the type declaration inside this package.
            SymbolReference<ReferenceTypeDeclaration> sref = typeSolver.tryToSolveType(node.toString());
            if (sref.isSolved()) {
                return new ReferenceTypeImpl(sref.getCorrespondingDeclaration(), typeSolver);
            }
        }
        if (value != null && value.isPresent()) {
            return value.get().getType();
        } else {
            throw new UnsolvedSymbolException(node.getField().getId());
        }
    }

    @Override
    public Type visit(InstanceOfExpr node, Boolean solveLambdas) {
        return PrimitiveType.BOOLEAN;
    }

    @Override
    public Type visit(StringLiteralExpr node, Boolean solveLambdas) {
        return new ReferenceTypeImpl(new ReflectionTypeSolver().solveType("java.lang.String"), typeSolver);
    }

    @Override
    public Type visit(IntegerLiteralExpr node, Boolean solveLambdas) {
        return PrimitiveType.INT;
    }

    @Override
    public Type visit(LongLiteralExpr node, Boolean solveLambdas) {
        return PrimitiveType.LONG;
    }

    @Override
    public Type visit(CharLiteralExpr node, Boolean solveLambdas) {
        return PrimitiveType.CHAR;
    }

    @Override
    public Type visit(DoubleLiteralExpr node, Boolean solveLambdas) {
        if (node.getValue().toLowerCase().endsWith("f")) {
            return PrimitiveType.FLOAT;
        }
        return PrimitiveType.DOUBLE;
    }

    @Override
    public Type visit(BooleanLiteralExpr node, Boolean solveLambdas) {
        return PrimitiveType.BOOLEAN;
    }

    @Override
    public Type visit(NullLiteralExpr node, Boolean solveLambdas) {
        return NullType.INSTANCE;
    }

    @Override
    public Type visit(MethodCallExpr node, Boolean solveLambdas) {
        logger.finest("getType on method call " + node);
        // first solve the method
        MethodUsage ref = facade.solveMethodAsUsage(node);
        logger.finest("getType on method call " + node + " resolved to " + ref);
        logger.finest("getType on method call " + node + " return type is " + ref.returnType());
        return ref.returnType();
        // the type is the return type of the method
    }

    @Override
    public Type visit(NameExpr node, Boolean solveLambdas) {
        logger.finest("getType on name expr " + node);
        Optional<Value> value = new SymbolSolver(typeSolver).solveSymbolAsValue(node.getName().getId(), node);
        if (!value.isPresent()) {
            throw new UnsolvedSymbolException("Solving " + node, node.getName().getId());
        } else {
            return value.get().getType();
        }
    }

    @Override
    public Type visit(ObjectCreationExpr node, Boolean solveLambdas) {
        Type type = facade.convertToUsage(node.getType(), node);
        return type;
    }

    @Override
    public Type visit(ThisExpr node, Boolean solveLambdas) {
        // If 'this' is prefixed by a class eg. MyClass.this
        if (node.getClassExpr().isPresent()){
            // Get the class name
            String className = node.getClassExpr().get().toString();
            // Attempt to resolve using a typeSolver
            SymbolReference<ReferenceTypeDeclaration> clazz = typeSolver.tryToSolveType(className);
            if (clazz.isSolved()){
                return new ReferenceTypeImpl(clazz.getCorrespondingDeclaration(),typeSolver);
            }
            // Attempt to resolve locally in Compilation unit
            Optional<CompilationUnit> cu = node.getAncestorOfType(CompilationUnit.class);
            if (cu.isPresent()){
                Optional<ClassOrInterfaceDeclaration> classByName = cu.get().getClassByName(className);
                if (classByName.isPresent()){
                    return new ReferenceTypeImpl(facade.getTypeDeclaration(classByName.get()), typeSolver);
                }
            }

        }
        return new ReferenceTypeImpl(facade.getTypeDeclaration(facade.findContainingTypeDecl(node)), typeSolver);
    }

    @Override
    public Type visit(SuperExpr node, Boolean solveLambdas) {
        TypeDeclaration typeOfNode = facade.getTypeDeclaration(facade.findContainingTypeDecl(node));
        if (typeOfNode instanceof ClassDeclaration) {
            return ((ClassDeclaration) typeOfNode).getSuperClass();
        } else {
            throw new UnsupportedOperationException(node.getClass().getCanonicalName());
        }
    }

    @Override
    public Type visit(UnaryExpr node, Boolean solveLambdas) {
        switch (node.getOperator()) {
            case MINUS:
            case PLUS:
                return node.getExpression().accept(this, solveLambdas);
            case LOGICAL_COMPLEMENT:
                return PrimitiveType.BOOLEAN;
            case POSTFIX_DECREMENT:
            case PREFIX_DECREMENT:
            case POSTFIX_INCREMENT:
            case PREFIX_INCREMENT:
                return node.getExpression().accept(this, solveLambdas);
            default:
                throw new UnsupportedOperationException(node.getOperator().name());
        }
    }

    @Override
    public Type visit(VariableDeclarationExpr node, Boolean solveLambdas) {
        if (node.getVariables().size() != 1) {
            throw new UnsupportedOperationException();
        }
        return facade.convertToUsageVariableType(node.getVariables().get(0));
    }


    @Override
    public Type visit(LambdaExpr node, Boolean solveLambdas) {
        if (getParentNode(node) instanceof MethodCallExpr) {
            MethodCallExpr callExpr = (MethodCallExpr) getParentNode(node);
            int pos = JavaParserSymbolDeclaration.getParamPos(node);
            SymbolReference<MethodDeclaration> refMethod = facade.solve(callExpr);
            if (!refMethod.isSolved()) {
                throw new UnsolvedSymbolException(getParentNode(node).toString(), callExpr.getName().getId());
            }
            logger.finest("getType on lambda expr " + refMethod.getCorrespondingDeclaration().getName());
            if (solveLambdas) {

                // The type parameter referred here should be the java.util.stream.Stream.T
                Type result = refMethod.getCorrespondingDeclaration().getParam(pos).getType();

                if (callExpr.getScope().isPresent()) {
                    Expression scope = callExpr.getScope().get();

                    // If it is a static call we should not try to get the type of the scope
                    boolean staticCall = false;
                    if (scope instanceof NameExpr) {
                        NameExpr nameExpr = (NameExpr) scope;
                        try {
                            SymbolReference<TypeDeclaration> type = JavaParserFactory.getContext(nameExpr, typeSolver).solveType(nameExpr.getName().getId(), typeSolver);
                            if (type.isSolved()){
                                staticCall = true;
                            }
                        } catch (Exception e) {

                        }
                    }

                    if (!staticCall) {
                        Type scopeType = facade.getType(scope);
                        if (scopeType.isReferenceType()) {
                            result = scopeType.asReferenceType().useThisTypeParametersOnTheGivenType(result);
                        }
                    }
                }

                // We need to replace the type variables
                Context ctx = JavaParserFactory.getContext(node, typeSolver);
                result = facade.solveGenericTypes(result, ctx, typeSolver);

                //We should find out which is the functional method (e.g., apply) and replace the params of the
                //solveLambdas with it, to derive so the values. We should also consider the value returned by the
                //lambdas
                Optional<MethodUsage> functionalMethod = FunctionalInterfaceLogic.getFunctionalMethod(result);
                if (functionalMethod.isPresent()) {
                    LambdaExpr lambdaExpr = node;

                    InferenceContext lambdaCtx = new InferenceContext(MyObjectProvider.INSTANCE);
                    InferenceContext funcInterfaceCtx = new InferenceContext(MyObjectProvider.INSTANCE);

                    // At this point parameterType
                    // if Function<T=? super Stream.T, ? extends map.R>
                    // we should replace Stream.T
                    Type functionalInterfaceType = ReferenceTypeImpl.undeterminedParameters(functionalMethod.get().getDeclaration().declaringType(), typeSolver);

                    lambdaCtx.addPair(result, functionalInterfaceType);

                    Type actualType;

                    if (lambdaExpr.getBody() instanceof ExpressionStmt) {
                        actualType = facade.getType(((ExpressionStmt)lambdaExpr.getBody()).getExpression());
                    } else if (lambdaExpr.getBody() instanceof BlockStmt) {
                        BlockStmt blockStmt = (BlockStmt) lambdaExpr.getBody();
                        NodeList<Statement> statements = blockStmt.getStatements();

                        // Get all the return statements in the lambda block
                        List<ReturnStmt> returnStmts = blockStmt.getNodesByType(ReturnStmt.class);

                        if (returnStmts.size() > 0){

                            actualType = returnStmts.stream()
                            .map(returnStmt -> {
                                Optional<Expression> expression = returnStmt.getExpression();
                                if (expression.isPresent()){
                                    return facade.getType(expression.get());
                                } else{
                                    return VoidType.INSTANCE;
                                }
                            })
                            .filter(x -> x != null && !x.isVoid() && !x.isNull())
                            .findFirst()
                            .orElse(VoidType.INSTANCE);

                        } else {
                            return VoidType.INSTANCE;
                        }


                    } else {
                        throw new UnsupportedOperationException();
                    }

                    Type formalType = functionalMethod.get().returnType();

                    // Infer the functional interfaces' return vs actual type
                    funcInterfaceCtx.addPair(formalType, actualType);
                    // Substitute to obtain a new type
                    Type functionalTypeWithReturn = funcInterfaceCtx.resolve(funcInterfaceCtx.addSingle(functionalInterfaceType));

                    // if the functional method returns void anyway
                    // we don't need to bother inferring types
                    if (!(formalType instanceof VoidType)){
                        lambdaCtx.addPair(result, functionalTypeWithReturn);
                        result = lambdaCtx.resolve(lambdaCtx.addSingle(result));
                    }
                }

                return result;
            } else {
                return refMethod.getCorrespondingDeclaration().getParam(pos).getType();
            }
        } else {
            throw new UnsupportedOperationException("The type of a lambda expr depends on the position and its return value");
        }
    }

    @Override
    public Type visit(MethodReferenceExpr node, Boolean solveLambdas) {
        if (getParentNode(node) instanceof MethodCallExpr) {
            MethodCallExpr callExpr = (MethodCallExpr) getParentNode(node);
            int pos = JavaParserSymbolDeclaration.getParamPos(node);
            SymbolReference<com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration> refMethod = facade.solve(callExpr, false);
            if (!refMethod.isSolved()) {
                throw new UnsolvedSymbolException(getParentNode(node).toString(), callExpr.getName().getId());
            }
            logger.finest("getType on method reference expr " + refMethod.getCorrespondingDeclaration().getName());
            //logger.finest("Method param " + refMethod.getCorrespondingDeclaration().getParam(pos));
            if (solveLambdas) {
                MethodUsage usage = facade.solveMethodAsUsage(callExpr);
                Type result = usage.getParamType(pos);
                // We need to replace the type variables
                Context ctx = JavaParserFactory.getContext(node, typeSolver);
                result = facade.solveGenericTypes(result, ctx, typeSolver);

                //We should find out which is the functional method (e.g., apply) and replace the params of the
                //solveLambdas with it, to derive so the values. We should also consider the value returned by the
                //lambdas
                Optional<MethodUsage> functionalMethod = FunctionalInterfaceLogic.getFunctionalMethod(result);
                if (functionalMethod.isPresent()) {
                    if (node instanceof MethodReferenceExpr) {
                        MethodReferenceExpr methodReferenceExpr = (MethodReferenceExpr) node;

                        Type actualType = facade.toMethodUsage(methodReferenceExpr).returnType();
                        Type formalType = functionalMethod.get().returnType();

                        InferenceContext inferenceContext = new InferenceContext(MyObjectProvider.INSTANCE);
                        inferenceContext.addPair(formalType, actualType);
                        result = inferenceContext.resolve(inferenceContext.addSingle(result));
                    }
                }

                return result;
            } else {
                return refMethod.getCorrespondingDeclaration().getParam(pos).getType();
            }
        } else {
            throw new UnsupportedOperationException("The type of a method reference expr depends on the position and its return value");
        }
    }


}
