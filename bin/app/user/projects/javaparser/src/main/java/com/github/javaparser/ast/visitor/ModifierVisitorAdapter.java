/*
 * Copyright (C) 2007-2010 Júlio Vilmar Gesser.
 * Copyright (C) 2011, 2013-2016 The JavaParser Team.
 *
 * This file is part of JavaParser.
 * 
 * JavaParser can be used either under the terms of
 * a) the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * b) the terms of the Apache License 
 *
 * You should have received a copy of both licenses in LICENCE.LGPL and
 * LICENCE.APACHE. Please refer to those files for details.
 *
 * JavaParser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */
 
package com.github.javaparser.ast.visitor;

import java.util.List;

import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;
import com.github.javaparser.ast.body.AnnotationDeclaration;
import com.github.javaparser.ast.body.AnnotationMemberDeclaration;
import com.github.javaparser.ast.body.BaseParameter;
import com.github.javaparser.ast.body.BodyDeclaration;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EmptyMemberDeclaration;
import com.github.javaparser.ast.body.EmptyTypeDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.EnumDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.InitializerDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.MultiTypeParameter;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.TypeDeclaration;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.JavadocComment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.expr.AnnotationExpr;
import com.github.javaparser.ast.expr.ArrayAccessExpr;
import com.github.javaparser.ast.expr.ArrayCreationExpr;
import com.github.javaparser.ast.expr.ArrayInitializerExpr;
import com.github.javaparser.ast.expr.AssignExpr;
import com.github.javaparser.ast.expr.BinaryExpr;
import com.github.javaparser.ast.expr.BooleanLiteralExpr;
import com.github.javaparser.ast.expr.CastExpr;
import com.github.javaparser.ast.expr.CharLiteralExpr;
import com.github.javaparser.ast.expr.ClassExpr;
import com.github.javaparser.ast.expr.ConditionalExpr;
import com.github.javaparser.ast.expr.DoubleLiteralExpr;
import com.github.javaparser.ast.expr.EnclosedExpr;
import com.github.javaparser.ast.expr.Expression;
import com.github.javaparser.ast.expr.FieldAccessExpr;
import com.github.javaparser.ast.expr.InstanceOfExpr;
import com.github.javaparser.ast.expr.IntegerLiteralExpr;
import com.github.javaparser.ast.expr.IntegerLiteralMinValueExpr;
import com.github.javaparser.ast.expr.LambdaExpr;
import com.github.javaparser.ast.expr.LongLiteralExpr;
import com.github.javaparser.ast.expr.LongLiteralMinValueExpr;
import com.github.javaparser.ast.expr.MarkerAnnotationExpr;
import com.github.javaparser.ast.expr.MemberValuePair;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.MethodReferenceExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.NormalAnnotationExpr;
import com.github.javaparser.ast.expr.NullLiteralExpr;
import com.github.javaparser.ast.expr.ObjectCreationExpr;
import com.github.javaparser.ast.expr.QualifiedNameExpr;
import com.github.javaparser.ast.expr.SingleMemberAnnotationExpr;
import com.github.javaparser.ast.expr.StringLiteralExpr;
import com.github.javaparser.ast.expr.SuperExpr;
import com.github.javaparser.ast.expr.ThisExpr;
import com.github.javaparser.ast.expr.TypeExpr;
import com.github.javaparser.ast.expr.UnaryExpr;
import com.github.javaparser.ast.expr.VariableDeclarationExpr;
import com.github.javaparser.ast.stmt.AssertStmt;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.BreakStmt;
import com.github.javaparser.ast.stmt.CatchClause;
import com.github.javaparser.ast.stmt.ContinueStmt;
import com.github.javaparser.ast.stmt.DoStmt;
import com.github.javaparser.ast.stmt.EmptyStmt;
import com.github.javaparser.ast.stmt.ExplicitConstructorInvocationStmt;
import com.github.javaparser.ast.stmt.ExpressionStmt;
import com.github.javaparser.ast.stmt.ForStmt;
import com.github.javaparser.ast.stmt.ForeachStmt;
import com.github.javaparser.ast.stmt.IfStmt;
import com.github.javaparser.ast.stmt.LabeledStmt;
import com.github.javaparser.ast.stmt.ReturnStmt;
import com.github.javaparser.ast.stmt.Statement;
import com.github.javaparser.ast.stmt.SwitchEntryStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;
import com.github.javaparser.ast.stmt.SynchronizedStmt;
import com.github.javaparser.ast.stmt.ThrowStmt;
import com.github.javaparser.ast.stmt.TryStmt;
import com.github.javaparser.ast.stmt.TypeDeclarationStmt;
import com.github.javaparser.ast.stmt.WhileStmt;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.IntersectionType;
import com.github.javaparser.ast.type.PrimitiveType;
import com.github.javaparser.ast.type.ReferenceType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.UnionType;
import com.github.javaparser.ast.type.UnknownType;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.ast.type.WildcardType;

/**
 * This visitor adapter can be used to save time when some specific nodes needs
 * to be changed. To do that just extend this class and override the methods
 * from the nodes who needs to be changed, returning the changed node.
 * 
 * @author Julio Vilmar Gesser
 */
public abstract class ModifierVisitorAdapter<A> implements GenericVisitor<Node, A> {

	private void removeNulls(final List<?> list) {
		for (int i = list.size() - 1; i >= 0; i--) {
			if (list.get(i) == null) {
				list.remove(i);
			}
		}
	}

	@Override public Node visit(final AnnotationDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
        final List<BodyDeclaration<?>> members = n.getMembers();
		if (members != null) {
			for (int i = 0; i < members.size(); i++) {
                members.set(i, (BodyDeclaration<?>) members.get(i).accept(this, arg));
			}
			removeNulls(members);
		}
		return n;
	}

	@Override public Node visit(final AnnotationMemberDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		n.setType((Type) n.getType().accept(this, arg));
		if (n.getDefaultValue() != null) {
			n.setDefaultValue((Expression) n.getDefaultValue().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final ArrayAccessExpr n, final A arg) {
		n.setName((Expression) n.getName().accept(this, arg));
		n.setIndex((Expression) n.getIndex().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ArrayCreationExpr n, final A arg) {
		n.setType((Type) n.getType().accept(this, arg));
		if (n.getDimensions() != null) {
			final List<Expression> dimensions = n.getDimensions();
			if (dimensions != null) {
				for (int i = 0; i < dimensions.size(); i++) {
					dimensions.set(i, (Expression) dimensions.get(i).accept(this, arg));
				}
				removeNulls(dimensions);
			}
		}
		if (n.getInitializer() != null) {
			n.setInitializer((ArrayInitializerExpr) n.getInitializer().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final ArrayInitializerExpr n, final A arg) {
		if (n.getValues() != null) {
			final List<Expression> values = n.getValues();
			if (values != null) {
				for (int i = 0; i < values.size(); i++) {
					values.set(i, (Expression) values.get(i).accept(this, arg));
				}
				removeNulls(values);
			}
		}
		return n;
	}

	@Override public Node visit(final AssertStmt n, final A arg) {
		n.setCheck((Expression) n.getCheck().accept(this, arg));
		if (n.getMessage() != null) {
			n.setMessage((Expression) n.getMessage().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final AssignExpr n, final A arg) {
		final Expression target = (Expression) n.getTarget().accept(this, arg);
		if (target == null) {
			return null;
		}
        n.setTarget(target);

		final Expression value = (Expression) n.getValue().accept(this, arg);
		if (value == null) {
			return null;
		}
		n.setValue(value);

		return n;
	}

	@Override public Node visit(final BinaryExpr n, final A arg) {
		final Expression left = (Expression) n.getLeft().accept(this, arg);
		final Expression right = (Expression) n.getRight().accept(this, arg);
		if (left == null) {
			return right;
		}
		if (right == null) {
			return left;
		}
		n.setLeft(left);
		n.setRight(right);
		return n;
	}

	@Override public Node visit(final BlockStmt n, final A arg) {
		final List<Statement> stmts = n.getStmts();
		if (stmts != null) {
			for (int i = 0; i < stmts.size(); i++) {
				stmts.set(i, (Statement) stmts.get(i).accept(this, arg));
			}
			removeNulls(stmts);
		}
		return n;
	}

	@Override public Node visit(final BooleanLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final BreakStmt n, final A arg) {
		return n;
	}

	@Override public Node visit(final CastExpr n, final A arg) {
		final Type type = (Type) n.getType().accept(this, arg);
		final Expression expr = (Expression) n.getExpr().accept(this, arg);
		if (type == null) {
			return expr;
		}
		if (expr == null) {
			return null;
		}
		n.setType(type);
		n.setExpr(expr);
		return n;
	}

	@Override public Node visit(final CatchClause n, final A arg) {
		n.setParam((Parameter)n.getParam().accept(this, arg));
		n.setCatchBlock((BlockStmt) n.getCatchBlock().accept(this, arg));
		return n;

	}

	@Override public Node visit(final CharLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final ClassExpr n, final A arg) {
		n.setType((Type) n.getType().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ClassOrInterfaceDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		final List<TypeParameter> typeParameters = n.getTypeParameters();
		if (typeParameters != null) {
			for (int i = 0; i < typeParameters.size(); i++) {
				typeParameters.set(i, (TypeParameter) typeParameters.get(i).accept(this, arg));
			}
			removeNulls(typeParameters);
		}
		final List<ClassOrInterfaceType> extendz = n.getExtends();
		if (extendz != null) {
			for (int i = 0; i < extendz.size(); i++) {
				extendz.set(i, (ClassOrInterfaceType) extendz.get(i).accept(this, arg));
			}
			removeNulls(extendz);
		}
		final List<ClassOrInterfaceType> implementz = n.getImplements();
		if (implementz != null) {
			for (int i = 0; i < implementz.size(); i++) {
				implementz.set(i, (ClassOrInterfaceType) implementz.get(i).accept(this, arg));
			}
			removeNulls(implementz);
		}
        final List<BodyDeclaration<?>> members = n.getMembers();
		if (members != null) {
			for (int i = 0; i < members.size(); i++) {
                members.set(i, (BodyDeclaration<?>) members.get(i).accept(this, arg));
			}
			removeNulls(members);
		}
		return n;
	}

	@Override public Node visit(final ClassOrInterfaceType n, final A arg) {
		if (n.getScope() != null) {
			n.setScope((ClassOrInterfaceType) n.getScope().accept(this, arg));
		}
		final List<Type> typeArgs = n.getTypeArgs();
		if (typeArgs != null) {
			for (int i = 0; i < typeArgs.size(); i++) {
				typeArgs.set(i, (Type) typeArgs.get(i).accept(this, arg));
			}
			removeNulls(typeArgs);
		}
		return n;
	}

	@Override public Node visit(final CompilationUnit n, final A arg) {
		if (n.getPackage() != null) {
			n.setPackage((PackageDeclaration) n.getPackage().accept(this, arg));
		}
		final List<ImportDeclaration> imports = n.getImports();
		if (imports != null) {
			for (int i = 0; i < imports.size(); i++) {
				imports.set(i, (ImportDeclaration) imports.get(i).accept(this, arg));
			}
			removeNulls(imports);
		}
        final List<TypeDeclaration<?>> types = n.getTypes();
		if (types != null) {
			for (int i = 0; i < types.size(); i++) {
                types.set(i, (TypeDeclaration<?>) types.get(i).accept(this, arg));
			}
			removeNulls(types);
		}
		return n;
	}

	@Override public Node visit(final ConditionalExpr n, final A arg) {
		n.setCondition((Expression) n.getCondition().accept(this, arg));
		n.setThenExpr((Expression) n.getThenExpr().accept(this, arg));
		n.setElseExpr((Expression) n.getElseExpr().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ConstructorDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		final List<TypeParameter> typeParameters = n.getTypeParameters();
		if (typeParameters != null) {
			for (int i = 0; i < typeParameters.size(); i++) {
				typeParameters.set(i, (TypeParameter) typeParameters.get(i).accept(this, arg));
			}
			removeNulls(typeParameters);
		}
		final List<Parameter> parameters = n.getParameters();
		if (parameters != null) {
			for (int i = 0; i < parameters.size(); i++) {
				parameters.set(i, (Parameter) parameters.get(i).accept(this, arg));
			}
			removeNulls(parameters);
		}
		final List<ReferenceType> throwz = n.getThrows();
		if (throwz != null) {
			for (int i = 0; i < throwz.size(); i++) {
				throwz.set(i, (ReferenceType) throwz.get(i).accept(this, arg));
			}
			removeNulls(throwz);
		}
		n.setBody((BlockStmt) n.getBody().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ContinueStmt n, final A arg) {
		return n;
	}

	@Override public Node visit(final DoStmt n, final A arg) {
		final Statement body = (Statement) n.getBody().accept(this, arg);
		if (body == null) {
			return null;
		}
		n.setBody(body);

		final Expression condition = (Expression) n.getCondition().accept(this, arg);
		if (condition == null) {
			return null;
		}
		n.setCondition(condition);

		return n;
	}

	@Override public Node visit(final DoubleLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final EmptyMemberDeclaration n, final A arg) {
		return n;
	}

	@Override public Node visit(final EmptyStmt n, final A arg) {
		return n;
	}

	@Override public Node visit(final EmptyTypeDeclaration n, final A arg) {
		return n;
	}

	@Override public Node visit(final EnclosedExpr n, final A arg) {
		n.setInner((Expression) n.getInner().accept(this, arg));
		return n;
	}

	@Override public Node visit(final EnumConstantDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		final List<Expression> args = n.getArgs();
		if (args != null) {
			for (int i = 0; i < args.size(); i++) {
				args.set(i, (Expression) args.get(i).accept(this, arg));
			}
			removeNulls(args);
		}
        final List<BodyDeclaration<?>> classBody = n.getClassBody();
		if (classBody != null) {
			for (int i = 0; i < classBody.size(); i++) {
                classBody.set(i, (BodyDeclaration<?>) classBody.get(i).accept(this, arg));
			}
			removeNulls(classBody);
		}
		return n;
	}

	@Override public Node visit(final EnumDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		final List<ClassOrInterfaceType> implementz = n.getImplements();
		if (implementz != null) {
			for (int i = 0; i < implementz.size(); i++) {
				implementz.set(i, (ClassOrInterfaceType) implementz.get(i).accept(this, arg));
			}
			removeNulls(implementz);
		}
		final List<EnumConstantDeclaration> entries = n.getEntries();
		if (entries != null) {
			for (int i = 0; i < entries.size(); i++) {
				entries.set(i, (EnumConstantDeclaration) entries.get(i).accept(this, arg));
			}
			removeNulls(entries);
		}
        final List<BodyDeclaration<?>> members = n.getMembers();
		if (members != null) {
			for (int i = 0; i < members.size(); i++) {
                members.set(i, (BodyDeclaration<?>) members.get(i).accept(this, arg));
			}
			removeNulls(members);
		}
		return n;
	}

	@Override public Node visit(final ExplicitConstructorInvocationStmt n, final A arg) {
		if (!n.isThis() && n.getExpr() != null) {
			n.setExpr((Expression) n.getExpr().accept(this, arg));
		}
		final List<Type> typeArgs = n.getTypeArgs();
		if (typeArgs != null) {
			for (int i = 0; i < typeArgs.size(); i++) {
				typeArgs.set(i, (Type) typeArgs.get(i).accept(this, arg));
			}
			removeNulls(typeArgs);
		}
		final List<Expression> args = n.getArgs();
		if (args != null) {
			for (int i = 0; i < args.size(); i++) {
				args.set(i, (Expression) args.get(i).accept(this, arg));
			}
			removeNulls(args);
		}
		return n;
	}

	@Override public Node visit(final ExpressionStmt n, final A arg) {
		final Expression expr = (Expression) n.getExpression().accept(this, arg);
		if (expr == null) {
			return null;
		}
		n.setExpression(expr);
		return n;
	}

	@Override public Node visit(final FieldAccessExpr n, final A arg) {
		final Expression scope = (Expression) n.getScope().accept(this, arg);
		if (scope == null) {
			return null;
		}
		n.setScope(scope);
		return n;
	}

	@Override public Node visit(final FieldDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		n.setType((Type) n.getType().accept(this, arg));
		final List<VariableDeclarator> variables = n.getVariables();
		for (int i = 0; i < variables.size(); i++) {
			variables.set(i, (VariableDeclarator) variables.get(i).accept(this, arg));
		}
		removeNulls(variables);
		return n;
	}

	@Override public Node visit(final ForeachStmt n, final A arg) {
		n.setVariable((VariableDeclarationExpr) n.getVariable().accept(this, arg));
		n.setIterable((Expression) n.getIterable().accept(this, arg));
		n.setBody((Statement) n.getBody().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ForStmt n, final A arg) {
		final List<Expression> init = n.getInit();
		if (init != null) {
			for (int i = 0; i < init.size(); i++) {
				init.set(i, (Expression) init.get(i).accept(this, arg));
			}
			removeNulls(init);
		}
		if (n.getCompare() != null) {
			n.setCompare((Expression) n.getCompare().accept(this, arg));
		}
		final List<Expression> update = n.getUpdate();
		if (update != null) {
			for (int i = 0; i < update.size(); i++) {
				update.set(i, (Expression) update.get(i).accept(this, arg));
			}
			removeNulls(update);
		}
		n.setBody((Statement) n.getBody().accept(this, arg));
		return n;
	}

	@Override public Node visit(final IfStmt n, final A arg) {
		final Expression condition = (Expression)
			n.getCondition().accept(this, arg);
		if (condition == null) {
			return null;
		}
		n.setCondition(condition);
		final Statement thenStmt = (Statement) n.getThenStmt().accept(this, arg);
		if (thenStmt == null) {
			// Remove the entire statement if the then-clause was removed.
			// DumpVisitor, used for toString, has no null check for the
			// then-clause.
			return null;
		}
		n.setThenStmt(thenStmt);
		if (n.getElseStmt() != null) {
			n.setElseStmt((Statement) n.getElseStmt().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final ImportDeclaration n, final A arg) {
		n.setName((NameExpr) n.getName().accept(this, arg));
		return n;
	}

	@Override public Node visit(final InitializerDeclaration n, final A arg) {
		n.setBlock((BlockStmt) n.getBlock().accept(this, arg));
		return n;
	}

	@Override public Node visit(final InstanceOfExpr n, final A arg) {
		n.setExpr((Expression) n.getExpr().accept(this, arg));
		n.setType((Type) n.getType().accept(this, arg));
		return n;
	}

	@Override public Node visit(final IntegerLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final IntegerLiteralMinValueExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final JavadocComment n, final A arg) {
		return n;
	}

	@Override public Node visit(final LabeledStmt n, final A arg) {
		n.setStmt((Statement) n.getStmt().accept(this, arg));
		return n;
	}

	@Override public Node visit(final LongLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final LongLiteralMinValueExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final MarkerAnnotationExpr n, final A arg) {
		n.setName((NameExpr) n.getName().accept(this, arg));
		return n;
	}

	@Override public Node visit(final MemberValuePair n, final A arg) {
		n.setValue((Expression) n.getValue().accept(this, arg));
		return n;
	}

	@Override public Node visit(final MethodCallExpr n, final A arg) {
		if (n.getScope() != null) {
			n.setScope((Expression) n.getScope().accept(this, arg));
		}
		final List<Type> typeArgs = n.getTypeArgs();
		if (typeArgs != null) {
			for (int i = 0; i < typeArgs.size(); i++) {
				typeArgs.set(i, (Type) typeArgs.get(i).accept(this, arg));
			}
			removeNulls(typeArgs);
		}
		final List<Expression> args = n.getArgs();
		if (args != null) {
			for (int i = 0; i < args.size(); i++) {
				args.set(i, (Expression) args.get(i).accept(this, arg));
			}
			removeNulls(args);
		}
		return n;
	}

	@Override public Node visit(final MethodDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		final List<TypeParameter> typeParameters = n.getTypeParameters();
		if (typeParameters != null) {
			for (int i = 0; i < typeParameters.size(); i++) {
				typeParameters.set(i, (TypeParameter) typeParameters.get(i).accept(this, arg));
			}
			removeNulls(typeParameters);
		}
		n.setType((Type) n.getType().accept(this, arg));
		final List<Parameter> parameters = n.getParameters();
		if (parameters != null) {
			for (int i = 0; i < parameters.size(); i++) {
				parameters.set(i, (Parameter) parameters.get(i).accept(this, arg));
			}
			removeNulls(parameters);
		}
		final List<ReferenceType> throwz = n.getThrows();
		if (throwz != null) {
			for (int i = 0; i < throwz.size(); i++) {
				throwz.set(i, (ReferenceType) throwz.get(i).accept(this, arg));
			}
			removeNulls(throwz);
		}
		if (n.getBody() != null) {
			n.setBody((BlockStmt) n.getBody().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final NameExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final NormalAnnotationExpr n, final A arg) {
		n.setName((NameExpr) n.getName().accept(this, arg));
		final List<MemberValuePair> pairs = n.getPairs();
		if (pairs != null) {
			for (int i = 0; i < pairs.size(); i++) {
				pairs.set(i, (MemberValuePair) pairs.get(i).accept(this, arg));
			}
			removeNulls(pairs);
		}
		return n;
	}

	@Override public Node visit(final NullLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final ObjectCreationExpr n, final A arg) {
		if (n.getScope() != null) {
			n.setScope((Expression) n.getScope().accept(this, arg));
		}
		final List<Type> typeArgs = n.getTypeArgs();
		if (typeArgs != null) {
			for (int i = 0; i < typeArgs.size(); i++) {
				typeArgs.set(i, (Type) typeArgs.get(i).accept(this, arg));
			}
			removeNulls(typeArgs);
		}
		n.setType((ClassOrInterfaceType) n.getType().accept(this, arg));
		final List<Expression> args = n.getArgs();
		if (args != null) {
			for (int i = 0; i < args.size(); i++) {
				args.set(i, (Expression) args.get(i).accept(this, arg));
			}
			removeNulls(args);
		}
        final List<BodyDeclaration<?>> anonymousClassBody = n.getAnonymousClassBody();
		if (anonymousClassBody != null) {
			for (int i = 0; i < anonymousClassBody.size(); i++) {
                anonymousClassBody.set(i, (BodyDeclaration<?>) anonymousClassBody.get(i).accept(this, arg));
			}
			removeNulls(anonymousClassBody);
		}
		return n;
	}

	@Override public Node visit(final PackageDeclaration n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		n.setName((NameExpr) n.getName().accept(this, arg));
		return n;
	}
	
	@Override public Node visit(final Parameter n, final A arg) {
        visit((BaseParameter<?>) n, arg);
		n.setType((Type) n.getType().accept(this, arg));
		return n;
	}
	
	@Override public Node visit(MultiTypeParameter n, A arg) {
        visit((BaseParameter<?>) n, arg);
        n.setType((UnionType)n.getType().accept(this, arg));
        return n;
    }

    protected Node visit(final BaseParameter<?> n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}
		
		n.setId((VariableDeclaratorId) n.getId().accept(this, arg));
		return n;
	}

	@Override public Node visit(final PrimitiveType n, final A arg) {
		return n;
	}

	@Override public Node visit(final QualifiedNameExpr n, final A arg) {
		n.setQualifier((NameExpr) n.getQualifier().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ReferenceType n, final A arg) {
		n.setType((Type) n.getType().accept(this, arg));
		return n;
	}

    @Override
    public Node visit(final IntersectionType n, final A arg) {
        final List<ReferenceType> elements = n.getElements();
        if (elements != null) {
            for (int i = 0; i < elements.size(); i++) {
                elements.set(i, (ReferenceType) elements.get(i).accept(this, arg));
            }
            removeNulls(elements);
        }
        return n;
    }

    @Override
    public Node visit(final UnionType n, final A arg) {
        final List<ReferenceType> elements = n.getElements();
        if (elements != null) {
            for (int i = 0; i < elements.size(); i++) {
                elements.set(i, (ReferenceType) elements.get(i).accept(this, arg));
            }
            removeNulls(elements);
        }
        return n;
    }

	@Override public Node visit(final ReturnStmt n, final A arg) {
		if (n.getExpr() != null) {
			n.setExpr((Expression) n.getExpr().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final SingleMemberAnnotationExpr n, final A arg) {
		n.setName((NameExpr) n.getName().accept(this, arg));
		n.setMemberValue((Expression) n.getMemberValue().accept(this, arg));
		return n;
	}

	@Override public Node visit(final StringLiteralExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final SuperExpr n, final A arg) {
		if (n.getClassExpr() != null) {
			n.setClassExpr((Expression) n.getClassExpr().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final SwitchEntryStmt n, final A arg) {
		if (n.getLabel() != null) {
			n.setLabel((Expression) n.getLabel().accept(this, arg));
		}
		final List<Statement> stmts = n.getStmts();
		if (stmts != null) {
			for (int i = 0; i < stmts.size(); i++) {
				stmts.set(i, (Statement) stmts.get(i).accept(this, arg));
			}
			removeNulls(stmts);
		}
		return n;
	}

	@Override public Node visit(final SwitchStmt n, final A arg) {
		n.setSelector((Expression) n.getSelector().accept(this, arg));
		final List<SwitchEntryStmt> entries = n.getEntries();
		if (entries != null) {
			for (int i = 0; i < entries.size(); i++) {
				entries.set(i, (SwitchEntryStmt) entries.get(i).accept(this, arg));
			}
			removeNulls(entries);
		}
		return n;

	}

	@Override public Node visit(final SynchronizedStmt n, final A arg) {
		n.setExpr((Expression) n.getExpr().accept(this, arg));
		n.setBlock((BlockStmt) n.getBlock().accept(this, arg));
		return n;
	}

	@Override public Node visit(final ThisExpr n, final A arg) {
		if (n.getClassExpr() != null) {
			n.setClassExpr((Expression) n.getClassExpr().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final ThrowStmt n, final A arg) {
		n.setExpr((Expression) n.getExpr().accept(this, arg));
		return n;
	}

	@Override public Node visit(final TryStmt n, final A arg) {
		n.setTryBlock((BlockStmt) n.getTryBlock().accept(this, arg));
		final List<CatchClause> catchs = n.getCatchs();
		if (catchs != null) {
			for (int i = 0; i < catchs.size(); i++) {
				catchs.set(i, (CatchClause) catchs.get(i).accept(this, arg));
			}
			removeNulls(catchs);
		}
		if (n.getFinallyBlock() != null) {
			n.setFinallyBlock((BlockStmt) n.getFinallyBlock().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final TypeDeclarationStmt n, final A arg) {
        n.setTypeDeclaration((TypeDeclaration<?>) n.getTypeDeclaration().accept(this, arg));
		return n;
	}

	@Override public Node visit(final TypeParameter n, final A arg) {
		final List<ClassOrInterfaceType> typeBound = n.getTypeBound();
		if (typeBound != null) {
			for (int i = 0; i < typeBound.size(); i++) {
				typeBound.set(i, (ClassOrInterfaceType) typeBound.get(i).accept(this, arg));
			}
			removeNulls(typeBound);
		}
		return n;
	}

	@Override public Node visit(final UnaryExpr n, final A arg) {
		n.setExpr((Expression) n.getExpr().accept(this, arg));
		return n;
	}

	@Override public Node visit(final UnknownType n, final A arg) {
		return n;
	}

	@Override public Node visit(final VariableDeclarationExpr n, final A arg) {
		final List<AnnotationExpr> annotations = n.getAnnotations();
		if (annotations != null) {
			for (int i = 0; i < annotations.size(); i++) {
				annotations.set(i, (AnnotationExpr) annotations.get(i).accept(this, arg));
			}
			removeNulls(annotations);
		}

		final Type type = (Type) n.getType().accept(this, arg);
		if (type == null) {
			return null;
		}
		n.setType(type);

		final List<VariableDeclarator> vars = n.getVars();
		for (int i = 0; i < vars.size();) {
			final VariableDeclarator decl = (VariableDeclarator)
				vars.get(i).accept(this, arg);
			if (decl == null) {
				vars.remove(i);
			} else {
				vars.set(i++, decl);
			}
		}
		if (vars.isEmpty()) {
			return null;
		}

		return n;
	}

	@Override public Node visit(final VariableDeclarator n, final A arg) {
		final VariableDeclaratorId id = (VariableDeclaratorId)
			n.getId().accept(this, arg);
		if (id == null) {
			return null;
		}
		n.setId(id);
		if (n.getInit() != null) {
			n.setInit((Expression) n.getInit().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final VariableDeclaratorId n, final A arg) {
		return n;
	}

	@Override public Node visit(final VoidType n, final A arg) {
		return n;
	}

	@Override public Node visit(final WhileStmt n, final A arg) {
		n.setCondition((Expression) n.getCondition().accept(this, arg));
		n.setBody((Statement) n.getBody().accept(this, arg));
		return n;
	}

	@Override public Node visit(final WildcardType n, final A arg) {
		if (n.getExtends() != null) {
			n.setExtends((ReferenceType) n.getExtends().accept(this, arg));
		}
		if (n.getSuper() != null) {
			n.setSuper((ReferenceType) n.getSuper().accept(this, arg));
		}
		return n;
	}

	@Override public Node visit(final LambdaExpr n, final A arg) {
		return n;
	}

	@Override public Node visit(final MethodReferenceExpr n, final A arg){
		return n;
	}

	@Override public Node visit(final TypeExpr n, final A arg){
		return n;
	}

	@Override public Node visit(final BlockComment n, final A arg) {
		return n;
	}

	@Override public Node visit(final LineComment n, final A arg) {
		return n;
	}

}
