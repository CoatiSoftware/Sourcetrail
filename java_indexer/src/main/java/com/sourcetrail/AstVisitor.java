package com.sourcetrail;

import com.sourcetrail.name.DeclName;
import com.sourcetrail.name.FileName;
import com.sourcetrail.name.NameHierarchy;
import com.sourcetrail.name.SymbolName;
import com.sourcetrail.name.TypeName;
import com.sourcetrail.name.resolver.BindingNameResolver;
import com.sourcetrail.name.resolver.DeclNameResolver;
import java.io.File;
import java.lang.String;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;
import java.util.Stack;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.ASTVisitor;
import org.eclipse.jdt.core.dom.Annotation;
import org.eclipse.jdt.core.dom.AnnotationTypeDeclaration;
import org.eclipse.jdt.core.dom.AnnotationTypeMemberDeclaration;
import org.eclipse.jdt.core.dom.AnonymousClassDeclaration;
import org.eclipse.jdt.core.dom.ArrayInitializer;
import org.eclipse.jdt.core.dom.Block;
import org.eclipse.jdt.core.dom.BlockComment;
import org.eclipse.jdt.core.dom.ClassInstanceCreation;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.ConstructorInvocation;
import org.eclipse.jdt.core.dom.CreationReference;
import org.eclipse.jdt.core.dom.EnumConstantDeclaration;
import org.eclipse.jdt.core.dom.EnumDeclaration;
import org.eclipse.jdt.core.dom.ExpressionMethodReference;
import org.eclipse.jdt.core.dom.FieldAccess;
import org.eclipse.jdt.core.dom.FieldDeclaration;
import org.eclipse.jdt.core.dom.IAnnotationBinding;
import org.eclipse.jdt.core.dom.IBinding;
import org.eclipse.jdt.core.dom.IMethodBinding;
import org.eclipse.jdt.core.dom.IPackageBinding;
import org.eclipse.jdt.core.dom.ITypeBinding;
import org.eclipse.jdt.core.dom.IVariableBinding;
import org.eclipse.jdt.core.dom.ImportDeclaration;
import org.eclipse.jdt.core.dom.Javadoc;
import org.eclipse.jdt.core.dom.LineComment;
import org.eclipse.jdt.core.dom.MarkerAnnotation;
import org.eclipse.jdt.core.dom.MemberValuePair;
import org.eclipse.jdt.core.dom.MethodDeclaration;
import org.eclipse.jdt.core.dom.MethodInvocation;
import org.eclipse.jdt.core.dom.Name;
import org.eclipse.jdt.core.dom.NameQualifiedType;
import org.eclipse.jdt.core.dom.NormalAnnotation;
import org.eclipse.jdt.core.dom.PackageDeclaration;
import org.eclipse.jdt.core.dom.ParameterizedType;
import org.eclipse.jdt.core.dom.PrimitiveType;
import org.eclipse.jdt.core.dom.QualifiedName;
import org.eclipse.jdt.core.dom.QualifiedType;
import org.eclipse.jdt.core.dom.SimpleName;
import org.eclipse.jdt.core.dom.SimpleType;
import org.eclipse.jdt.core.dom.SingleMemberAnnotation;
import org.eclipse.jdt.core.dom.SuperConstructorInvocation;
import org.eclipse.jdt.core.dom.SuperFieldAccess;
import org.eclipse.jdt.core.dom.SuperMethodInvocation;
import org.eclipse.jdt.core.dom.SuperMethodReference;
import org.eclipse.jdt.core.dom.SwitchStatement;
import org.eclipse.jdt.core.dom.ThisExpression;
import org.eclipse.jdt.core.dom.Type;
import org.eclipse.jdt.core.dom.TypeDeclaration;
import org.eclipse.jdt.core.dom.TypeMethodReference;
import org.eclipse.jdt.core.dom.TypeParameter;
import org.eclipse.jdt.core.dom.VariableDeclarationFragment;

public abstract class AstVisitor extends ASTVisitor
{
	protected AstVisitorClient m_client = null;
	private File m_filePath;
	private FileContent m_fileContent = null;
	private CompilationUnit m_compilationUnit;
	private Stack<List<SymbolName>> m_contextStack = new Stack<>();

	public AstVisitor(
		AstVisitorClient client, File filePath, String fileContent, CompilationUnit compilationUnit)
	{
		m_client = client;
		m_filePath = filePath;
		m_fileContent = new FileContent(fileContent);
		m_compilationUnit = compilationUnit;

		m_contextStack.push(Arrays.asList(new FileName(filePath)));
	}

	protected abstract ReferenceKind getTypeReferenceKind();


	// --- record declarations ---

	@Override public boolean visit(PackageDeclaration node)
	{
		Name name = node.getName();

		Range range = getRange(name);
		if (name instanceof QualifiedName)
		{
			range = getRange(((QualifiedName)name).getName());
		}

		m_client.recordSymbolWithLocation(
			DeclNameResolver.getQualifiedName(name).toNameHierarchy(),
			SymbolKind.PACKAGE,
			range,
			AccessKind.NONE,
			DefinitionKind.EXPLICIT);

		// Here we just record the symbol types of qualifiers. The nodes and their location are
		// visited and recorded because the "name" is a QualifiedName
		while (name instanceof QualifiedName)
		{
			name = ((QualifiedName)name).getQualifier();
			m_client.recordSymbol(
				DeclNameResolver.getQualifiedName(name).toNameHierarchy(),
				SymbolKind.PACKAGE,
				AccessKind.NONE,
				DefinitionKind.EXPLICIT);
		}

		return true;
	}


	@Override public boolean visit(AnnotationTypeDeclaration node)
	{
		DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
			node, m_filePath, m_compilationUnit);
		Range scopeRange = getRange(node);

		m_client.recordSymbolWithLocationAndScope(
			symbolName.toNameHierarchy(),
			SymbolKind.ANNOTATION,
			getRange(node.getName()),
			scopeRange,
			AccessKind.fromModifiers(node.getModifiers()),
			DefinitionKind.EXPLICIT);

		scopeRange.begin = m_fileContent.findStartPosition("{", scopeRange.begin);
		recordScope(scopeRange);

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(AnnotationTypeDeclaration node)
	{
		m_contextStack.pop();
	}


	@Override public boolean visit(AnnotationTypeMemberDeclaration node)
	{
		DeclName symbolName = BindingNameResolver
								  .getQualifiedName(
									  node.resolveBinding(), m_filePath, m_compilationUnit)
								  .orElse(DeclName.unsolved());

		m_client.recordSymbolWithLocation(
			symbolName.toNameHierarchy(),
			SymbolKind.FIELD,
			getRange(node.getName()),
			AccessKind.fromModifiers(node.getModifiers()),
			DefinitionKind.EXPLICIT);

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(AnnotationTypeMemberDeclaration node)
	{
		m_contextStack.pop();
	}


	@Override public boolean visit(TypeDeclaration node)
	{
		DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
			node, m_filePath, m_compilationUnit);
		Range scopeRange = getRange(node);

		m_client.recordSymbolWithLocationAndScope(
			symbolName.toNameHierarchy(),
			node.isInterface() ? SymbolKind.INTERFACE : SymbolKind.CLASS,
			getRange(node.getName()),
			scopeRange,
			AccessKind.fromModifiers(node.getModifiers()),
			DefinitionKind.EXPLICIT);

		scopeRange.begin = m_fileContent.findStartPosition("{", scopeRange.begin);
		recordScope(scopeRange);

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(TypeDeclaration node)
	{
		m_contextStack.pop();
	}


	@Override public boolean visit(TypeParameter node)
	{
		DeclName symbolName = BindingNameResolver
								  .getQualifiedName(
									  node.resolveBinding(), m_filePath, m_compilationUnit)
								  .map(tn -> tn.toDeclName())
								  .orElse(DeclName.unsolved());

		m_client.recordSymbolWithLocation(
			symbolName.toNameHierarchy(),
			SymbolKind.TYPE_PARAMETER,
			getRange(node.getName()),
			AccessKind.TYPE_PARAMETER,
			DefinitionKind.EXPLICIT);

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(TypeParameter node)
	{
		m_contextStack.pop();
	}


	@Override public boolean visit(EnumDeclaration node)
	{
		DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
			node, m_filePath, m_compilationUnit);
		Range scopeRange = getRange(node);

		m_client.recordSymbolWithLocationAndScope(
			symbolName.toNameHierarchy(),
			SymbolKind.ENUM,
			getRange(node.getName()),
			scopeRange,
			AccessKind.fromModifiers(node.getModifiers()),
			DefinitionKind.EXPLICIT);

		scopeRange.begin = m_fileContent.findStartPosition("{", scopeRange.begin);
		recordScope(scopeRange);

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(EnumDeclaration node)
	{
		m_contextStack.pop();
	}


	public boolean visit(EnumConstantDeclaration node)
	{
		DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
			node, m_filePath, m_compilationUnit);

		m_client.recordSymbolWithLocation(
			symbolName.toNameHierarchy(),
			SymbolKind.ENUM_CONSTANT,
			getRange(node.getName()),
			AccessKind.NONE,
			DefinitionKind.EXPLICIT);

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(EnumConstantDeclaration node)
	{
		m_contextStack.pop();
	}


	@Override public boolean visit(MethodDeclaration node)
	{
		if (m_client.getInterrupted())
		{
			m_contextStack.push(new ArrayList<>());
			return false;
		}

		DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
			node, m_filePath, m_compilationUnit);

		Range signatureRange = getRange(node);
		if (!node.thrownExceptionTypes().isEmpty())
		{
			Object lastExceptionType = node.thrownExceptionTypes().get(
				node.thrownExceptionTypes().size() - 1);
			if (lastExceptionType instanceof ASTNode)
			{
				signatureRange.end = getRange((ASTNode)lastExceptionType).end;
			}
		}
		else
		{
			signatureRange.end = getRange(node.getName()).end;

			if (!node.parameters().isEmpty())
			{
				Object lastParametersType = node.parameters().get(node.parameters().size() - 1);
				if (lastParametersType instanceof ASTNode)
				{
					signatureRange.end = getRange((ASTNode)lastParametersType).end;
				}
			}

			signatureRange.end = m_fileContent.findStartPosition(")", signatureRange.end);
		}


		m_client.recordSymbolWithLocationAndScopeAndSignature(
			symbolName.toNameHierarchy(),
			SymbolKind.METHOD,
			getRange(node.getName()),
			getRange(node),
			signatureRange,
			AccessKind.fromModifiers(node.getModifiers()),
			DefinitionKind.EXPLICIT);


		Optional<IMethodBinding> overriddenMethod = getOverriddenMethod(node.resolveBinding());
		if (overriddenMethod.isPresent())
		{
			// We use the declaration to replace type arguments with the respective type parameters
			// inside the signature.
			IMethodBinding overriddenMethodDeclaration = overriddenMethod.get().getMethodDeclaration();

			DeclName overriddenMethodName =
				BindingNameResolver
					.getQualifiedName(overriddenMethodDeclaration, m_filePath, m_compilationUnit)
					.orElse(DeclName.unsolved());
			if (!overriddenMethodName.getIsUnsolved())
			{
				m_client.recordSymbol(
					overriddenMethodName.toNameHierarchy(),
					SymbolKind.METHOD,
					AccessKind.NONE,
					DefinitionKind.NONE);
			}

			m_client.recordReference(
				ReferenceKind.OVERRIDE,
				overriddenMethodName.toNameHierarchy(),
				symbolName.toNameHierarchy(),
				getRange(node.getName()));
		}

		m_contextStack.push(Arrays.asList(symbolName));

		return true;
	}

	@Override public void endVisit(MethodDeclaration node)
	{
		m_contextStack.pop();
	}


	@Override public boolean visit(FieldDeclaration node)
	{
		ArrayList<SymbolName> childContext = new ArrayList<>();

		for (Object declarator: node.fragments())
		{
			if (declarator instanceof VariableDeclarationFragment)
			{
				VariableDeclarationFragment fragment = (VariableDeclarationFragment)declarator;

				DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
					fragment, m_filePath, m_compilationUnit);

				m_client.recordSymbolWithLocation(
					symbolName.toNameHierarchy(),
					SymbolKind.FIELD,
					getRange(fragment.getName()),
					AccessKind.fromModifiers(node.getModifiers()),
					DefinitionKind.EXPLICIT);

				childContext.add(symbolName);
			}
		}

		m_contextStack.push(childContext);

		return true;
	}

	@Override public void endVisit(FieldDeclaration node)
	{
		m_contextStack.pop();
	}


	// --- record references ---

	@Override public boolean visit(final ImportDeclaration node)
	{
		DeclName symbolName = DeclName.unsolved();
		IBinding binding = node.resolveBinding();
		if (!binding.isRecovered())
		{
			if (binding instanceof IPackageBinding)
			{
				symbolName = BindingNameResolver
								 .getQualifiedName(
									 (IPackageBinding)binding, m_filePath, m_compilationUnit)
								 .orElse(DeclName.unsolved());
			}
			else if (binding instanceof ITypeBinding)
			{
				symbolName = BindingNameResolver
								 .getQualifiedName(
									 (ITypeBinding)binding, m_filePath, m_compilationUnit)
								 .map(tn -> tn.toDeclName())
								 .orElse(DeclName.unsolved());
			}
			else if (binding instanceof IMethodBinding)
			{
				symbolName = BindingNameResolver
								 .getQualifiedName(
									 (IMethodBinding)binding, m_filePath, m_compilationUnit)
								 .orElse(DeclName.unsolved());
			}
			else if (binding instanceof IVariableBinding)
			{
				symbolName = BindingNameResolver.getQualifiedName(
					(IVariableBinding)binding, m_filePath, m_compilationUnit);
			}
		}

		for (SymbolName context: m_contextStack.peek())
		{
			Range range = getRange(node.getName());
			if (node.getName() instanceof QualifiedName)
			{
				range = getRange(((QualifiedName)node.getName()).getName());
			}

			m_client.recordReference(
				ReferenceKind.IMPORT, symbolName.toNameHierarchy(), context.toNameHierarchy(), range);
		}

		// record package symbol kind of current node if appliccable
		if (binding instanceof IPackageBinding)
		{
			Optional<DeclName> packageName = BindingNameResolver.getQualifiedName(
				(IPackageBinding)binding, m_filePath, m_compilationUnit);
			if (packageName.isPresent())
			{
				m_client.recordSymbol(
					packageName.get().toNameHierarchy(),
					SymbolKind.PACKAGE,
					AccessKind.NONE,
					DefinitionKind.NONE);
			}
		}

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, true).recordQualifierOfNode(node);

		return true;
	}

	@Override public boolean visit(SingleMemberAnnotation node)
	{
		return recordAnnotation(node);
	}

	@Override public boolean visit(NormalAnnotation node)
	{
		for (Object value: node.values())
		{
			if (value instanceof MemberValuePair)
			{
				SimpleName name = ((MemberValuePair)value).getName();
				IBinding binding = name.resolveBinding();
				if (binding instanceof IMethodBinding)
				{
					DeclName symbolName =
						BindingNameResolver
							.getQualifiedName((IMethodBinding)binding, m_filePath, m_compilationUnit)
							.orElse(DeclName.unsolved());

					for (SymbolName context: m_contextStack.peek())
					{
						m_client.recordReference(
							ReferenceKind.USAGE,
							symbolName.toNameHierarchy(),
							context.toNameHierarchy(),
							getRange(name));
					}
				}
			}
		}

		return recordAnnotation(node);
	}

	@Override public boolean visit(MarkerAnnotation node)
	{
		return recordAnnotation(node);
	}

	private boolean recordAnnotation(Annotation node)
	{
		for (SymbolName context: m_contextStack.peek())
		{
			IAnnotationBinding annotationBinding = node.resolveAnnotationBinding();
			ITypeBinding typeBinding = null;
			if (annotationBinding != null)
			{
				typeBinding = annotationBinding.getAnnotationType();
			}

			Range range = getRange(node.getTypeName());
			if (node.getTypeName() instanceof QualifiedName)
			{
				range = getRange(((QualifiedName)node.getTypeName()).getName());
			}

			m_client.recordReference(
				ReferenceKind.ANNOTATION_USAGE,
				BindingNameResolver.getQualifiedName(typeBinding, m_filePath, m_compilationUnit)
					.orElse(TypeName.unsolved())
					.toDeclName()
					.toNameHierarchy(),
				context.toNameHierarchy(),
				range);
		}

		return true;
	}


	@Override public boolean visit(SimpleType node)
	{
		for (SymbolName context: m_contextStack.peek())
		{
			ITypeBinding binding = node.resolveBinding();
			if (binding != null)
			{
				binding = binding.getTypeDeclaration();
			}

			Range range = getRange(node.getName());
			if (node.getName() instanceof QualifiedName)
			{
				range = getRange(((QualifiedName)node.getName()).getName());
			}

			m_client.recordReference(
				getTypeReferenceKind(),
				BindingNameResolver.getQualifiedName(binding, m_filePath, m_compilationUnit)
					.orElse(TypeName.unsolved())
					.toDeclName()
					.toNameHierarchy(),
				context.toNameHierarchy(),
				range);
		}

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false).recordQualifierOfNode(node);

		return true;
	}

	@Override public boolean visit(QualifiedType node)
	{
		for (SymbolName context: m_contextStack.peek())
		{
			ITypeBinding binding = node.resolveBinding();
			if (binding != null)
			{
				binding = binding.getTypeDeclaration();
			}

			m_client.recordReference(
				getTypeReferenceKind(),
				BindingNameResolver.getQualifiedName(binding, m_filePath, m_compilationUnit)
					.orElse(TypeName.unsolved())
					.toDeclName()
					.toNameHierarchy(),
				context.toNameHierarchy(),
				getRange(node.getName()));
		}

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false).recordQualifierOfNode(node);

		return true;
	}

	@Override public boolean visit(NameQualifiedType node)
	{
		for (SymbolName context: m_contextStack.peek())
		{
			ITypeBinding binding = node.resolveBinding();
			if (binding != null)
			{
				binding = binding.getTypeDeclaration();
			}

			m_client.recordReference(
				getTypeReferenceKind(),
				BindingNameResolver.getQualifiedName(binding, m_filePath, m_compilationUnit)
					.orElse(TypeName.unsolved())
					.toDeclName()
					.toNameHierarchy(),
				context.toNameHierarchy(),
				getRange(node.getName()));
		}

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false).recordQualifierOfNode(node);

		return true;
	}

	@Override public boolean visit(final PrimitiveType node)
	{
		NameHierarchy referencedName =
			TypeName.fromDotSeparatedString(node.getPrimitiveTypeCode().toString())
				.toDeclName()
				.toNameHierarchy();

		m_client.recordSymbol(
			referencedName, SymbolKind.BUILTIN_TYPE, AccessKind.NONE, DefinitionKind.EXPLICIT);

		for (SymbolName context: m_contextStack.peek())
		{
			m_client.recordReference(
				getTypeReferenceKind(), referencedName, context.toNameHierarchy(), getRange(node));
		}
		return true;
	}

	@Override public boolean visit(SimpleName node)
	{
		IBinding binding = node.resolveBinding();
		if (binding instanceof IVariableBinding)
		{
			IVariableBinding variableBinding = ((IVariableBinding)binding).getVariableDeclaration();
			DeclName declName = BindingNameResolver.getQualifiedName(
				variableBinding, m_filePath, m_compilationUnit);

			if (declName.getIsUnsolved() && variableBinding.getDeclaringClass() == null &&
				variableBinding.getName().equals("length"))
			{
				// Do nothing. We ignore the case of unsolved symbols on array type
			}
			else if (declName.getIsLocal() || declName.getIsGlobal())
			{
				m_client.recordLocalSymbol(declName.toNameHierarchy(), getRange(node));
			}
			else
			{
				m_client.recordSymbol(
					declName.toNameHierarchy(), SymbolKind.FIELD, AccessKind.NONE, DefinitionKind.NONE);

				for (SymbolName context: m_contextStack.peek())
				{
					m_client.recordReference(
						ReferenceKind.USAGE,
						declName.toNameHierarchy(),
						context.toNameHierarchy(),
						getRange(node));
				}
			}
		}
		return true;
	}

	@Override public boolean visit(ParameterizedType node)
	{
		ITypeBinding binding = node.resolveBinding();
		if (binding != null)
		{
			binding = binding.getTypeDeclaration();
		}

		for (Object o: node.typeArguments())
		{
			if (o instanceof Type)
			{
				Type type = (Type)o;
				ITypeBinding typeBinding = type.resolveBinding();
				m_client.recordReference(
					ReferenceKind.TYPE_ARGUMENT,
					BindingNameResolver.getQualifiedName(typeBinding, m_filePath, m_compilationUnit)
						.orElse(TypeName.unsolved())
						.toDeclName()
						.toNameHierarchy(),
					BindingNameResolver.getQualifiedName(binding, m_filePath, m_compilationUnit)
						.orElse(TypeName.unsolved())
						.toDeclName()
						.toNameHierarchy(),
					getRange(type));
			}
		}

		return true;
	}

	@Override public boolean visit(QualifiedName node)
	{
		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false).recordQualifierOfNode(node);

		return true;
	}

	@Override public boolean visit(FieldAccess node)
	{
		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(SuperFieldAccess node)
	{
		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(ThisExpression node)
	{
		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(MethodInvocation node)
	{
		IMethodBinding methodBinding = node.resolveMethodBinding();

		recordReferenceToMethodDeclaration(
			methodBinding, getRange(node.getName()), ReferenceKind.CALL, m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(SuperMethodInvocation node)
	{
		IMethodBinding methodBinding = node.resolveMethodBinding();

		recordReferenceToMethodDeclaration(
			methodBinding, getRange(node.getName()), ReferenceKind.CALL, m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(ConstructorInvocation node)
	{
		IMethodBinding methodBinding = node.resolveConstructorBinding();

		recordReferenceToMethodDeclaration(
			methodBinding,
			m_fileContent.findRange("this", getRange(node).begin),
			ReferenceKind.CALL,
			m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		return true;
	}

	@Override public boolean visit(SuperConstructorInvocation node)
	{
		IMethodBinding methodBinding = node.resolveConstructorBinding();

		recordReferenceToMethodDeclaration(
			methodBinding,
			m_fileContent.findRange("super", getRange(node).begin),
			ReferenceKind.CALL,
			m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		return true;
	}

	@Override public boolean visit(CreationReference node)
	{
		IMethodBinding methodBinding = node.resolveMethodBinding();

		recordReferenceToMethodDeclaration(
			methodBinding,
			m_fileContent.findRange("new", getRange(node).begin),
			ReferenceKind.USAGE,
			m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false).recordQualifierOfNode(node);

		return true;
	}

	@Override public boolean visit(ExpressionMethodReference node)
	{
		IMethodBinding methodBinding = node.resolveMethodBinding();

		recordReferenceToMethodDeclaration(
			methodBinding, getRange(node.getName()), ReferenceKind.USAGE, m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(SuperMethodReference node)
	{
		IMethodBinding methodBinding = node.resolveMethodBinding();

		recordReferenceToMethodDeclaration(
			methodBinding, getRange(node.getName()), ReferenceKind.USAGE, m_contextStack.peek());

		recordReferenceToTypeArguments(methodBinding, node.typeArguments());

		new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
			.recordQualifierOfNode(node, m_fileContent);

		return true;
	}

	@Override public boolean visit(TypeMethodReference node)
	{
		IMethodBinding methodBinding = node.resolveMethodBinding();
		if (methodBinding == null && node.getType() != null && node.getType().isArrayType())
		{
			// Do nothing. We ignore the case of unsolved symbols on array type
		}
		else
		{
			recordReferenceToMethodDeclaration(
				methodBinding, getRange(node.getName()), ReferenceKind.USAGE, m_contextStack.peek());

			recordReferenceToTypeArguments(methodBinding, node.typeArguments());

			new QualifierVisitor(m_client, m_filePath, m_compilationUnit, false)
				.recordQualifierOfNode(node);
		}

		return true;
	}

	@Override public boolean visit(ClassInstanceCreation node)
	{
		if (node.getAnonymousClassDeclaration() != null)
		{
			// record anonymous class here instead of overriding visit(AnonymousClassDeclaration
			// node) because the ClassInstanceCreation still contains the Type node.

			AnonymousClassDeclaration anonymousClassDeclaration = node.getAnonymousClassDeclaration();
			DeclName symbolName = DeclNameResolver.getQualifiedDeclName(
				anonymousClassDeclaration, m_filePath, m_compilationUnit);

			Range anonymousClassScope = getRange(anonymousClassDeclaration);

			m_client.recordSymbolWithLocationAndScope(
				symbolName.toNameHierarchy(),
				SymbolKind.CLASS,
				new Range(anonymousClassScope.begin, anonymousClassScope.begin),
				anonymousClassScope,
				AccessKind.NONE,
				DefinitionKind.EXPLICIT);

			recordScope(anonymousClassScope);

			m_contextStack.push(Arrays.asList(symbolName));
		}
		else
		{
			IMethodBinding constructorBinding = node.resolveConstructorBinding();
			if (constructorBinding != null)
			{
				constructorBinding = constructorBinding.getMethodDeclaration();
			}

			DeclName referencedDeclName = BindingNameResolver
											  .getQualifiedName(
												  constructorBinding, m_filePath, m_compilationUnit)
											  .orElse(DeclName.unsolved());
			if (!referencedDeclName.getIsUnsolved())
			{
				m_client.recordSymbol(
					referencedDeclName.toNameHierarchy(),
					SymbolKind.METHOD,
					AccessKind.NONE,
					DefinitionKind.NONE);
			}

			for (SymbolName context: m_contextStack.peek())
			{
				m_client.recordReference(
					ReferenceKind.CALL,
					referencedDeclName.toNameHierarchy(),
					context.toNameHierarchy(),
					getRange(node.getType()));
			}

			recordReferenceToTypeArguments(constructorBinding, node.typeArguments());
		}

		return true;
	}

	@Override public void endVisit(ClassInstanceCreation node)
	{
		if (node.getAnonymousClassDeclaration() != null)
		{
			m_contextStack.pop();
		}
	}

	@Override public boolean visit(Block node)
	{
		recordScope(getRange(node));
		return true;
	}

	@Override public boolean visit(ArrayInitializer node)
	{
		recordScope(getRange(node));
		return true;
	}

	@Override public boolean visit(SwitchStatement node)
	{
		Range scopeRange = getRange(node);
		scopeRange.begin = m_fileContent.findStartPosition("{", scopeRange.begin);
		recordScope(scopeRange);

		return true;
	}

	@Override public boolean visit(LineComment node)
	{
		m_client.recordComment(getRange(node));
		return true;
	}

	@Override public boolean visit(BlockComment node)
	{
		m_client.recordComment(getRange(node));
		return true;
	}

	@Override public boolean visit(Javadoc node)
	{
		m_client.recordComment(getRange(node));
		return true;
	}


	// --- utility methods ---

	private void recordReferenceToMethodDeclaration(
		IMethodBinding methodBinding, Range range, ReferenceKind referenceKind, List<SymbolName> contexts)
	{
		if (methodBinding != null)
		{
			// replacing type arguments of invocation with type variables of declaration
			methodBinding = methodBinding.getMethodDeclaration();
		}

		DeclName referencedDeclName =
			BindingNameResolver.getQualifiedName(methodBinding, m_filePath, m_compilationUnit)
				.orElse(DeclName.unsolved());
		if (!referencedDeclName.getIsUnsolved())
		{
			m_client.recordSymbol(
				referencedDeclName.toNameHierarchy(),
				SymbolKind.METHOD,
				AccessKind.NONE,
				DefinitionKind.NONE);
		}

		for (SymbolName context: m_contextStack.peek())
		{
			m_client.recordReference(
				referenceKind, referencedDeclName.toNameHierarchy(), context.toNameHierarchy(), range);
		}
	}

	private void recordReferenceToTypeArguments(IMethodBinding methodBinding, List typeArguments)
	{
		if (!typeArguments.isEmpty())
		{
			if (methodBinding != null)
			{
				// replacing type arguments of invocation with type variables of declaration
				methodBinding = methodBinding.getMethodDeclaration();
			}

			for (Object o: typeArguments)
			{
				if (o instanceof Type)
				{
					Type type = (Type)o;
					ITypeBinding typeBinding = type.resolveBinding();
					m_client.recordReference(
						ReferenceKind.TYPE_ARGUMENT,
						BindingNameResolver
							.getQualifiedName(typeBinding, m_filePath, m_compilationUnit)
							.orElse(TypeName.unsolved())
							.toDeclName()
							.toNameHierarchy(),
						BindingNameResolver
							.getQualifiedName(methodBinding, m_filePath, m_compilationUnit)
							.orElse(DeclName.unsolved())
							.toNameHierarchy(),
						getRange(type));
				}
			}
		}
	}

	private void recordScope(Range range)
	{
		NameHierarchy nameHierarchy = DeclName.scope(m_filePath, range.begin).toNameHierarchy();
		m_client.recordLocalSymbol(nameHierarchy, new Range(range.begin, range.begin));
		m_client.recordLocalSymbol(nameHierarchy, new Range(range.end, range.end));
	}

	protected Range getRange(ASTNode node)
	{
		return Utility.getRange(node, m_compilationUnit);
	}

	private IPackageBinding getDeclaringPackage(IBinding binding)
	{
		if (binding != null)
		{
			IBinding parentBinding = BindingNameResolver.getParentBinding(binding);
			if (parentBinding instanceof IPackageBinding)
			{
				return (IPackageBinding)parentBinding;
			}
			return getDeclaringPackage(parentBinding);
		}
		return null;
	}

	private Optional<IMethodBinding> getOverriddenMethod(IMethodBinding method)
	{
		if (method != null)
		{
			for (ITypeBinding declaringClassAncestor: getAllAncestorTypes(method.getDeclaringClass()))
			{
				for (IMethodBinding potentiallyOverridden:
					 declaringClassAncestor.getDeclaredMethods())
				{
					if (method.overrides(potentiallyOverridden))
					{
						return Optional.of(potentiallyOverridden);
					}
				}
			}
		}

		return Optional.empty();
	}

	private List<ITypeBinding> getAllAncestorTypes(ITypeBinding type)
	{
		List<ITypeBinding> allAncestorTypes = new ArrayList<>();

		List<ITypeBinding> directAncestorTypes = getDirectAncestorTypes(type);
		allAncestorTypes.addAll(directAncestorTypes);

		for (ITypeBinding directAncestorType: directAncestorTypes)
		{
			allAncestorTypes.addAll(getAllAncestorTypes(directAncestorType));
		}

		return allAncestorTypes;
	}

	private List<ITypeBinding> getDirectAncestorTypes(ITypeBinding type)
	{
		List<ITypeBinding> ancestorTypes = new ArrayList<>();

		if (type != null)
		{
			ITypeBinding superclass = type.getSuperclass();
			if (superclass != null)
			{
				ancestorTypes.add(superclass);
			}

			ancestorTypes.addAll(Arrays.asList(type.getInterfaces()));
		}

		return ancestorTypes;
	}
}