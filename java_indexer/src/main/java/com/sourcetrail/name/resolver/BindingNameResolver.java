package com.sourcetrail.name.resolver;

import com.sourcetrail.ContextList;
import com.sourcetrail.name.DeclName;
import com.sourcetrail.name.FunctionDeclName;
import com.sourcetrail.name.TypeName;
import com.sourcetrail.name.VariableDeclName;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import org.eclipse.jdt.core.dom.ASTNode;
import org.eclipse.jdt.core.dom.AnonymousClassDeclaration;
import org.eclipse.jdt.core.dom.CompilationUnit;
import org.eclipse.jdt.core.dom.IBinding;
import org.eclipse.jdt.core.dom.IMethodBinding;
import org.eclipse.jdt.core.dom.IPackageBinding;
import org.eclipse.jdt.core.dom.ITypeBinding;
import org.eclipse.jdt.core.dom.IVariableBinding;
import org.eclipse.jdt.core.dom.Modifier;

public class BindingNameResolver extends NameResolver
{
	public static IBinding getParentBinding(IBinding binding)
	{
		if (binding instanceof ITypeBinding)
		{
			return getParentBinding((ITypeBinding)binding);
		}
		if (binding instanceof IMethodBinding)
		{
			return ((IMethodBinding)binding).getDeclaringClass();
		}
		if (binding instanceof IVariableBinding)
		{
			return ((IVariableBinding)binding).getDeclaringClass();
		}
		return null;
	}

	public static IBinding getParentBinding(ITypeBinding binding)
	{
		if (binding.isLocal() || binding.isAnonymous())
		{
			if (binding.getDeclaringMember() instanceof IVariableBinding)
			{
				IVariableBinding variableBinding = (IVariableBinding)binding.getDeclaringMember();
				if (variableBinding.getDeclaringClass() != null)
				{
					return variableBinding.getDeclaringClass();
				}
				else
				{
					return variableBinding.getDeclaringMethod();
				}
			}
			else
			{
				return binding.getDeclaringMethod();
			}
		}
		else if (binding.isMember())
		{
			return binding.getDeclaringClass();
		}
		else if (binding.isTypeVariable())
		{
			if (binding.getDeclaringClass() != null)
			{
				return binding.getDeclaringClass();
			}
			else
			{
				return binding.getDeclaringMethod();
			}
		}
		else if (binding.isTopLevel())
		{
			return binding.getPackage();
		}

		return null;	// we don't have a parent (like void doesn't have a parent)
	}

	public BindingNameResolver(
		File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		super(currentFile, compilationUnit, ignoredContexts);
	}

	public static Optional<TypeName> getQualifiedName(
		ITypeBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}

	public static Optional<TypeName> getQualifiedName(
		ITypeBinding binding,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}

	public Optional<TypeName> getQualifiedName(ITypeBinding binding)
	{
		if (binding == null)
		{
			return Optional.empty();
		}

		if (binding.isArray())
		{
			return getQualifiedName(binding.getElementType());
		}
		else if (binding.isAnonymous())
		{
			ASTNode node = m_compilationUnit.findDeclaringNode(binding);
			if (node instanceof AnonymousClassDeclaration)
			{
				DeclName decl = DeclNameResolver.getQualifiedDeclName(
					(AnonymousClassDeclaration)node,
					m_currentFile,
					m_compilationUnit,
					m_ignoredContexts);
				return Optional.of(new TypeName(
					decl.getName(), decl.getTypeParameterNames(), null, decl.getParent()));
			}
			else
			{
				return Optional.empty();
			}
		}

		if (binding.isParameterizedType())
		{
			List<TypeName> typeArguments = new ArrayList<>();
			for (ITypeBinding typeArgumentBinding: binding.getTypeArguments())
			{
				Optional<TypeName> typeArgument = getQualifiedName(typeArgumentBinding);
				if (typeArgument.isPresent())
				{
					typeArguments.add(typeArgument.get());
				}
			}

			Optional<TypeName> typeName = getQualifiedName(binding.getTypeDeclaration());
			if (typeName.isPresent())
			{
				DeclName declName = typeName.get().toDeclName();
				return Optional.of(new TypeName(
					declName.getName(),
					declName.getTypeParameterNames(),
					typeArguments,
					declName.getParent()));
			}
			else
			{
				return Optional.empty();
			}
		}

		String name = binding.getName();
		List<String> typeParameterNames = null;
		if (binding.isGenericType())
		{
			typeParameterNames = new ArrayList<>();
			for (ITypeBinding typeParameter: binding.getTypeParameters())
			{
				typeParameterNames.add(typeParameter.getName());
			}
		}

		DeclName parentDeclName = getQualifiedContextName(binding);
		if (parentDeclName != null && parentDeclName.getIsUnsolved())
		{
			return Optional.empty();
		}

		// type arguments will be set in caller (which is this same method)
		return Optional.of(new TypeName(name, typeParameterNames, null, parentDeclName));
	}

	public static Optional<DeclName> getQualifiedName(
		IMethodBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}

	public static Optional<DeclName> getQualifiedName(
		IMethodBinding binding,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}

	public Optional<DeclName> getQualifiedName(IMethodBinding binding)
	{
		if (binding == null)
		{
			return Optional.empty();
		}

		String name = binding.getName();

		List<String> typeParameterNames = null;
		if (binding.isGenericMethod())
		{
			typeParameterNames = new ArrayList<>();
			for (ITypeBinding typeParameter: binding.getTypeParameters())
			{
				typeParameterNames.add(typeParameter.getName());
			}
		}

		DeclName declName = null;
		{
			boolean isStatic = Modifier.isStatic(binding.getModifiers());

			ContextList ignoredContexts = m_ignoredContexts.copy();
			ignoredContexts.add(binding);

			TypeName returnTypeName = binding.isConstructor()
				? null
				: getQualifiedName(
					  binding.getReturnType(), m_currentFile, m_compilationUnit, ignoredContexts)
					  .orElse(TypeName.unsolved());

			if (binding.isAnnotationMember())
			{
				declName = new VariableDeclName(name, returnTypeName, isStatic);
			}
			else
			{
				List<TypeName> parameterTypeNames = new ArrayList<>();
				for (ITypeBinding parameterType: binding.getParameterTypes())
				{
					parameterTypeNames.add(
						getQualifiedName(
							parameterType, m_currentFile, m_compilationUnit, ignoredContexts)
							.orElse(TypeName.unsolved()));
				}

				declName = new FunctionDeclName(
					name, typeParameterNames, returnTypeName, parameterTypeNames, isStatic);
			}
		}

		DeclName parentDeclName = getQualifiedContextName(binding);
		if (parentDeclName != null)
		{
			if (!parentDeclName.getIsUnsolved())
			{
				declName.setParent(parentDeclName);
			}
			else
			{
				return Optional.empty();
			}
		}

		return Optional.of(declName);
	}

	public static Optional<DeclName> getQualifiedName(
		IPackageBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}

	public static Optional<DeclName> getQualifiedName(
		IPackageBinding binding,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}

	public Optional<DeclName> getQualifiedName(IPackageBinding binding)
	{
		if (!binding.isUnnamed())
		{
			return Optional.of(DeclName.fromDotSeparatedString(binding.getName()));
		}
		return Optional.empty();
	}

	public static DeclName getQualifiedName(
		IVariableBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}

	public static DeclName getQualifiedName(
		IVariableBinding binding,
		File currentFile,
		CompilationUnit compilationUnit,
		ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(
			currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}

	public DeclName getQualifiedName(IVariableBinding binding)
	{
		if (binding == null)
		{
			return DeclName.unsolved();
		}

		if (binding.isField() || binding.isEnumConstant())
		{
			DeclName declName;
			if (binding.isEnumConstant())
			{
				declName = new DeclName(binding.getName());
			}
			else
			{
				TypeName typeName = getQualifiedName(binding.getType()).orElse(TypeName.unsolved());
				declName = new VariableDeclName(
					binding.getName(), typeName, Modifier.isStatic(binding.getModifiers()));
			}

			DeclName parentDeclName = getQualifiedContextName(binding);
			if (parentDeclName != null)
			{
				if (!parentDeclName.getIsUnsolved())
				{
					declName.setParent(parentDeclName);
				}
				else
				{
					declName = DeclName.unsolved();
				}
			}

			return declName;
		}
		else
		{
			if (binding.getDeclaringMethod() != null)
			{
				return DeclName.localSymbol(
					getQualifiedName(binding.getDeclaringMethod()).orElse(DeclName.unsolved()),
					binding.getVariableId());
			}
			else
			{
				return DeclName.globalSymbol(m_currentFile, binding.getVariableId());
			}
		}
	}

	private DeclName getQualifiedContextName(IBinding binding)
	{
		IBinding parentBinding = getParentBinding(binding);

		if (parentBinding == null || m_ignoredContexts.contains(parentBinding))
		{
			return null;
		}

		if (parentBinding instanceof ITypeBinding)
		{
			return getQualifiedName((ITypeBinding)parentBinding)
				.map(tn -> tn.toDeclName())
				.orElse(DeclName.unsolved());
		}
		else if (parentBinding instanceof IMethodBinding)
		{
			return getQualifiedName((IMethodBinding)parentBinding).orElse(DeclName.unsolved());
		}
		else if (parentBinding instanceof IVariableBinding)
		{
			return getQualifiedName((IVariableBinding)parentBinding);
		}
		else if (parentBinding instanceof IPackageBinding)
		{
			return getQualifiedName((IPackageBinding)parentBinding).orElse(null);
		}

		return null;
	}
}
