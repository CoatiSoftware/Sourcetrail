package com.sourcetrail.name.resolver;

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

import com.sourcetrail.ContextList;
import com.sourcetrail.name.JavaDeclName;
import com.sourcetrail.name.JavaFunctionDeclName;
import com.sourcetrail.name.JavaTypeName;
import com.sourcetrail.name.JavaVariableDeclName;

public class BindingNameResolver extends NameResolver
{
	public static IBinding getParentBinding(IBinding binding)
	{
		if (binding instanceof ITypeBinding)
		{
			return getParentBinding((ITypeBinding) binding);
		}
		else if (binding instanceof IMethodBinding)
		{
			return ((IMethodBinding) binding).getDeclaringClass();
		}
		else if (binding instanceof IVariableBinding)
		{
			return ((IVariableBinding) binding).getDeclaringClass();
		}
		return null;
	}
	
	public static IBinding getParentBinding(ITypeBinding binding)
	{
		if (binding.isLocal() || binding.isAnonymous())
		{
			if (binding.getDeclaringMember() instanceof IVariableBinding)
			{
				IVariableBinding variableBinding = (IVariableBinding) binding.getDeclaringMember();
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
		
		return null; // we don't have a parent (like void doesn't have a parent)
	}
	
	public BindingNameResolver(File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		super(currentFile, compilationUnit, ignoredContexts);
	}
	
	public static Optional<JavaTypeName> getQualifiedName(ITypeBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}
	
	public static Optional<JavaTypeName> getQualifiedName(ITypeBinding binding, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}
	
	public Optional<JavaTypeName> getQualifiedName(ITypeBinding binding)
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
				JavaDeclName decl = DeclNameResolver.getQualifiedDeclName((AnonymousClassDeclaration) node, m_currentFile, m_compilationUnit, m_ignoredContexts);
				return Optional.of(new JavaTypeName(decl.getName(), decl.getTypeParameterNames(), null, decl.getParent()));
			}
			else
			{
				return Optional.empty();
			}
		}
		
		if (binding.isParameterizedType())
		{
			List<JavaTypeName> typeArguments = new ArrayList<>();
			for (ITypeBinding typeArgumentBinding: binding.getTypeArguments())
			{
				Optional<JavaTypeName> typeArgument = getQualifiedName(typeArgumentBinding);
				if (typeArgument.isPresent())
				{
					typeArguments.add(typeArgument.get());
				}
			}
			
			Optional<JavaTypeName> typeName = getQualifiedName(binding.getTypeDeclaration());
			if (typeName.isPresent())
			{
				JavaDeclName declName = typeName.get().toDeclName();
				return Optional.of(new JavaTypeName(declName.getName(), declName.getTypeParameterNames(), typeArguments, declName.getParent()));
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
		
		JavaDeclName parentDeclName = getQualifiedContextName(binding);
		if (parentDeclName != null && parentDeclName.getIsUnsolved())
		{
			return Optional.empty();
		}
		
		// type arguments will be set in caller (which is this same method)
		return Optional.of(new JavaTypeName(name, typeParameterNames, null, parentDeclName));
	}
	
	public static Optional<JavaDeclName> getQualifiedName(IMethodBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}
	
	public static Optional<JavaDeclName> getQualifiedName(IMethodBinding binding, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}
	
	public Optional<JavaDeclName> getQualifiedName(IMethodBinding binding)
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
		
		ContextList ignoredContexts = m_ignoredContexts.copy();
		ignoredContexts.add(binding);
		
		JavaTypeName returnTypeName = binding.isConstructor() ? null : getQualifiedName(binding.getReturnType(), m_currentFile, m_compilationUnit, ignoredContexts).orElse(JavaTypeName.unsolved());
		
		List<JavaTypeName> parameterTypeNames =  new ArrayList<>();
		for (ITypeBinding parameterType: binding.getParameterTypes())
		{
			parameterTypeNames.add(getQualifiedName(parameterType, m_currentFile, m_compilationUnit, ignoredContexts).orElse(JavaTypeName.unsolved()));
		}
		
		boolean isStatic = Modifier.isStatic(binding.getModifiers());
		
		JavaFunctionDeclName declName = new JavaFunctionDeclName(name, typeParameterNames, returnTypeName, parameterTypeNames, isStatic);
		
		JavaDeclName parentDeclName = getQualifiedContextName(binding);
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
	
	public static Optional<JavaDeclName> getQualifiedName(IPackageBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}
	
	public static Optional<JavaDeclName> getQualifiedName(IPackageBinding binding, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}
	
	public Optional<JavaDeclName> getQualifiedName(IPackageBinding binding)
	{
		if (!binding.isUnnamed())
		{
			return Optional.of(JavaDeclName.fromDotSeparatedString(binding.getName()));
		}
		return Optional.empty(); 
	}
	
	public static JavaDeclName getQualifiedName(IVariableBinding binding, File currentFile, CompilationUnit compilationUnit)
	{
		return getQualifiedName(binding, currentFile, compilationUnit, null);
	}
	
	public static JavaDeclName getQualifiedName(IVariableBinding binding, File currentFile, CompilationUnit compilationUnit, ContextList ignoredContexts)
	{
		BindingNameResolver resolver = new BindingNameResolver(currentFile, compilationUnit, ignoredContexts);
		return resolver.getQualifiedName(binding);
	}
	
	public JavaDeclName getQualifiedName(IVariableBinding binding)
	{
		if (binding == null)
		{
			return JavaDeclName.unsolved();
		}
		
		if (binding.isField() || binding.isEnumConstant())
		{
			JavaDeclName declName;
			if (binding.isEnumConstant())
			{
				declName = new JavaDeclName(binding.getName());
			}
			else
			{
				JavaTypeName typeName = getQualifiedName(binding.getType()).orElse(JavaTypeName.unsolved());
				declName = new JavaVariableDeclName(binding.getName(), typeName, Modifier.isStatic(binding.getModifiers()));
			}
			
			JavaDeclName parentDeclName = getQualifiedContextName(binding);
			if (parentDeclName != null)
			{
				if (!parentDeclName.getIsUnsolved())
				{
					declName.setParent(parentDeclName);
				}
				else
				{
					declName = JavaDeclName.unsolved();
				}
			}
			
			return declName;
		}
		else
		{
			if (binding.getDeclaringMethod() != null)
			{
				return JavaDeclName.localSymbol(getQualifiedName(binding.getDeclaringMethod()).orElse(JavaDeclName.unsolved()), binding.getVariableId());
			}
			else
			{
				return JavaDeclName.globalSymbol(m_currentFile, binding.getVariableId());
			}
		}
	}
	
	private JavaDeclName getQualifiedContextName(IBinding binding)
	{
		IBinding parentBinding = getParentBinding(binding);
		
		if (parentBinding == null || m_ignoredContexts.contains(parentBinding))
		{
			return null;
		}
		
		if (parentBinding instanceof ITypeBinding)
		{
			return getQualifiedName((ITypeBinding) parentBinding).map(tn ->tn.toDeclName()).orElse(JavaDeclName.unsolved());
		}
		else if (parentBinding instanceof IMethodBinding)
		{
			return getQualifiedName((IMethodBinding) parentBinding).orElse(JavaDeclName.unsolved());
		}
		else if (parentBinding instanceof IVariableBinding)
		{
			return getQualifiedName((IVariableBinding) parentBinding);
		}
		else if (parentBinding instanceof IPackageBinding)
		{
			return getQualifiedName((IPackageBinding) parentBinding).orElse(null);
		}
		
		return null;
	}
}
