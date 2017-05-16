package com.sourcetrail;

import java.lang.String;
import java.util.List;
import java.util.Optional;
import java.util.ArrayList;

import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.EnumDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.expr.ArrayInitializerExpr;
import com.github.javaparser.ast.expr.FieldAccessExpr;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.Name;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.ObjectCreationExpr;
import com.github.javaparser.ast.expr.SimpleName;
import com.github.javaparser.ast.expr.VariableDeclarationExpr;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.PrimitiveType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.TypeParameter;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.Position;
import com.github.javaparser.Range;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.Modifier;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.JavaParserFacade;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserConstructorDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserFieldDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserParameterDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserSymbolDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodAmbiguityException;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.ValueDeclaration;
import com.github.javaparser.symbolsolver.model.methods.MethodUsage;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.resolution.UnsolvedSymbolException;
import com.github.javaparser.symbolsolver.model.typesystem.*;
import com.github.javaparser.symbolsolver.resolution.MethodResolutionLogic;

public class JavaAstVisitor extends JavaAstVisitorAdapter
{
	protected int m_callbackId = -1;
	private String m_filePath;
	private FileContent m_fileContent;
	private TypeSolver m_typeSolver;
	private List<DeclContext> m_context = new ArrayList<DeclContext>();
	
	public JavaAstVisitor(int callbackId, String filePath, FileContent fileContent, TypeSolver typeSolver)
	{		
		m_callbackId = callbackId;
		m_filePath = filePath;
		m_fileContent = fileContent;
		m_typeSolver = typeSolver;
		
		m_context.add(new DeclContext("/\tm" + filePath + "\ts\tp"));
	}
	
	// --- record declarations ---
	
	@Override public void visit(final PackageDeclaration n, final Void v)
	{
		Name name = n.getName();
		
		
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_callbackId, 
			JavaparserDeclNameResolver.getQualifiedName(name).toSerializedNameHierarchy(), 
			SymbolKind.PACKAGE,
			name.getRange(), 
			n.getRange(),
			AccessKind.NONE, 
			DefinitionKind.EXPLICIT
		);
		
		while (name.getQualifier().isPresent())
		{
			name = name.getQualifier().get();
			JavaIndexer.recordSymbol(
				m_callbackId, 
				JavaparserDeclNameResolver.getQualifiedName(name).toSerializedNameHierarchy(), 
				SymbolKind.PACKAGE, 
				AccessKind.NONE, 
				DefinitionKind.EXPLICIT
			);
		}
		
		super.visit(n, v);
	}
	
	@Override public void visit(final ClassOrInterfaceDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_callbackId, qualifiedName, (n.isInterface() ? SymbolKind.INTERFACE : SymbolKind.CLASS),
			name.getRange(), 
			n.getRange(),
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
			DefinitionKind.EXPLICIT
		);

		if (n.getRange().isPresent())
		{
			FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin());
			recordScope(Range.range(scopeStartLocation.line, scopeStartLocation.column, n.getRange().get().end.line, n.getRange().get().end.column));
		}
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final TypeParameter n, final Void v)
	{
		// todo: test recording of typeargument of type parameter bound type??
		
		
		
		if (m_context.size() != 1)
		{
			 // throw something!
		}
		
		String qualifiedName = m_context.get(0).getName();
		qualifiedName += "\tn";
		qualifiedName += n.getName() + "\ts\tp";
		
		
		Optional<Range> range = Optional.empty();
		if (n.getBegin().isPresent())
		{
			range = Optional.of(Range.range(
				n.getBegin().get().line, 
				n.getBegin().get().column, 
				n.getBegin().get().line, 
				n.getBegin().get().column + n.getNameAsString().length() - 1
			));
		}
		
		JavaIndexer.recordSymbolWithLocation(
			m_callbackId, qualifiedName, SymbolKind.TYPE_PARAMETER, 
			range,
			AccessKind.TYPE_PARAMETER, 
			DefinitionKind.EXPLICIT
		);

		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final EnumDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_callbackId, qualifiedName, SymbolKind.ENUM, 
			name.getRange(), 
			n.getRange(),
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
			DefinitionKind.EXPLICIT
		);

		if (n.getRange().isPresent())
		{
			FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin());
			recordScope(Range.range(scopeStartLocation.line, scopeStartLocation.column, n.getRange().get().end.line, n.getRange().get().end.column));
		}
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final EnumConstantDeclaration n, final Void v)
	{
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithLocation(
			m_callbackId, qualifiedName, SymbolKind.ENUM_CONSTANT,
			n.getRange(), 
			AccessKind.NONE, 
			DefinitionKind.EXPLICIT
		);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final ConstructorDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_callbackId, qualifiedName, SymbolKind.METHOD,
			name.getRange(), 
			n.getRange(), 
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
			DefinitionKind.EXPLICIT
		);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final MethodDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_callbackId, qualifiedName, SymbolKind.METHOD, 
			name.getRange(), 
			n.getRange(), 
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
			DefinitionKind.EXPLICIT
		);
		
		
// test this!
		com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration overridden = getOverridden(n);
		if (overridden != null && (overridden instanceof JavaParserMethodDeclaration))
		{
			String overriddenName = JavaparserDeclNameResolver.getQualifiedDeclName(((JavaParserMethodDeclaration)overridden).getWrappedNode(), m_typeSolver).toSerializedNameHierarchy();
			
			JavaIndexer.recordReference(
				m_callbackId, ReferenceKind.OVERRIDE, overriddenName, qualifiedName, 
				name.getRange()
			);
		}
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	private com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration getOverridden(MethodDeclaration overrider)
	{
		com.github.javaparser.ast.body.TypeDeclaration<?> scopeNode = overrider.getAncestorOfType(com.github.javaparser.ast.body.TypeDeclaration.class).get();
		if (scopeNode instanceof ClassOrInterfaceDeclaration)
		{
			List<com.github.javaparser.symbolsolver.model.typesystem.Type> parameterTypes = new ArrayList<>();
			try
			{
				for (Parameter parameter: overrider.getParameters())
				{
					Type parameterType = parameter.getType();
					parameterTypes.add(JavaParserFacade.get(m_typeSolver).convert(parameterType, parameterType));
				}
				
				ReferenceTypeDeclaration scopeDecl = JavaParserFacade.get(m_typeSolver).getTypeDeclaration((ClassOrInterfaceDeclaration)scopeNode);
				for (ReferenceType ancestor: scopeDecl.getAllAncestors())
				{
					try
					{
						SymbolReference<com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration> solvedMethod = MethodResolutionLogic.solveMethodInType(
							ancestor.getTypeDeclaration(), 
							overrider.getNameAsString(), 
							parameterTypes, 
							m_typeSolver
						);
						
						if (solvedMethod.isSolved())
						{
							return solvedMethod.getCorrespondingDeclaration();
						}
					}
					catch (UnsolvedSymbolException e)
					{
						// nothing to do here, just try to solve in the next ancestor
					}
					catch (Exception e)
					{
						// hmm, maybe we should handle these cases. soon..
						// don't do anything for parse exceptions. they are displayed as errors anyways.
					}
				}
			}
			catch (UnsolvedSymbolException e)
			{
				return null;
			}
			catch (ClassCastException e)
			{
				return null;
			}
			catch (Exception e)
			{
				return null;
			}
		}
		return null;
	}

	@Override public void visit(final FieldDeclaration n, final Void v)
	{
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		
		for (VariableDeclarator declarator: n.getVariables())
		{
			String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(declarator, m_typeSolver).toSerializedNameHierarchy();
			SimpleName name = declarator.getName();
			
			JavaIndexer.recordSymbolWithLocation(
				m_callbackId, qualifiedName, SymbolKind.FIELD, 
				name.getRange(),
				AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
				DefinitionKind.EXPLICIT
			);

			m_context.add(new DeclContext(qualifiedName));
		}
		
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final VariableDeclarationExpr n, final Void v)
	{
		for (VariableDeclarator declarator: n.getVariables())
		{
			SimpleName name = declarator.getName();

			if (name.getBegin().isPresent())
			{
				String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
				
				JavaIndexer.recordLocalSymbol(
					m_callbackId, qualifiedName, 
					name.getRange()
				);
			}
		}
		
		// don't change the context here.
		super.visit(n, v);
	}
	
	@Override public void visit(final Parameter n, final Void v)
	{
		SimpleName name = n.getName();
		
		if (name.getBegin().isPresent())
		{
			String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
			
			JavaIndexer.recordLocalSymbol(
				m_callbackId, qualifiedName, 
				name.getRange()
			);
		}
		
		// don't change the context here.
		super.visit(n, v);
	}
	
	
	// --- record references ---

	@Override 
	public void visit(final ImportDeclaration n, final Void v)
	{
		Name name = n.getName();
		if (n.isAsterisk() || !n.isStatic())
		{
			String importedName = JavaparserDeclNameResolver.getQualifiedName(name).toSerializedNameHierarchy();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.IMPORT, 
					importedName, context.getName(), 
					name.getRange()
				);
			}
		}
		else
		{
			try
			{
				String typeName = name.getQualifier().get().asString();
				String memberName = name.getIdentifier();
				
				List<JavaDeclName> importedDeclNames = new ArrayList<>();

				ReferenceTypeDeclaration solvedDecl = m_typeSolver.solveType(typeName);
				
				for (com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration methodDecl: solvedDecl.getDeclaredMethods()) // look for method
				{
					if (methodDecl.getName().equals(memberName))
					{
						importedDeclNames.add(
							JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(methodDecl, m_typeSolver
						));
					}
				}
				if (importedDeclNames.isEmpty() && solvedDecl.hasField(memberName)) // look for field
				{
					JavaDeclName importedTypeDeclName = JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(solvedDecl, m_typeSolver);
					if (importedTypeDeclName != null)
					{
						JavaDeclName importedDeclName = new JavaDeclName(memberName);
						importedDeclName.setParent(importedTypeDeclName);
						importedDeclNames.add(importedDeclName);
					}
				}
				
				
				if (!importedDeclNames.isEmpty())
				{
					for (JavaDeclName importedDeclName: importedDeclNames)
					{
						String nameHierarchy = importedDeclName.toSerializedNameHierarchy();
						for (DeclContext context: m_context)
						{
							JavaIndexer.recordReference(
								m_callbackId, ReferenceKind.IMPORT, 
								nameHierarchy, context.getName(), 
								n.getRange()
							);
						}
					}
				}
				else
				{
					JavaIndexer.recordError(
						m_callbackId, "Import not found.", true, true, 
						n.getRange()
					);
				}
			}
			catch (Exception e)
			{
				recordException(e, n);
			}
		}
		super.visit(n, v);
	}
	
	@Override public void visit(final ClassOrInterfaceType n, final Void v)
	{
		try
		{
			for (DeclContext context: m_context)
			{
				String referencedName = JavaparserTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toSerializedNameHierarchy();

				Range range = Range.range(0, 0, 0, 0);
				
				if (n.getRange().isPresent())
				{
					range = Range.range(
						n.getRange().get().begin.line,
						n.getRange().get().begin.column,
						n.getRange().get().begin.line,
						n.getRange().get().begin.column + n.getNameAsString().length() - 1
					);
				}
				
				Optional<ClassOrInterfaceType> scope = n.getScope();
				if (scope.isPresent())
				{
					Optional<Position> position = scope.get().getEnd();

					if (position.isPresent())
					{
						range = range.withEnd(
							Position.pos(position.get().line, 
							position.get().column + n.getNameAsString().length() + 1 // +1 for separator
						));
					}
				}
				
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					range
				);
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		super.visit(n, v);
	}
	
	@Override public void visit(final PrimitiveType n, final Void v)
	{
		try
		{
			String referencedName = JavaparserTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toSerializedNameHierarchy();
			
			JavaIndexer.recordSymbol(
				m_callbackId, referencedName, SymbolKind.BUILTIN_TYPE,
				AccessKind.NONE, 
				DefinitionKind.EXPLICIT
			);
			
			for (DeclContext context: m_context)
			{		
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					n.getRange()
				);
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		
		super.visit(n, v);
	}
	
	@Override public void visit(final VoidType n, final Void v)
	{
		try
		{
			String referencedName = JavaparserTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toSerializedNameHierarchy();
			
			JavaIndexer.recordSymbol(
				m_callbackId, referencedName, SymbolKind.BUILTIN_TYPE,
				AccessKind.NONE, 
				DefinitionKind.EXPLICIT
			);
			
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					n.getRange()
				);
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		
		super.visit(n, v);
	}

	@Override public void visit(final FieldAccessExpr n, final Void v)
	{
		SimpleName fieldName = n.getField();
		
		try
		{
			SymbolReference<? extends ValueDeclaration> ref = JavaParserFacade.get(m_typeSolver).solve(fieldName);
			if (ref.isSolved())
			{
				ValueDeclaration valueDecl = ref.getCorrespondingDeclaration();
				
				Node wrappedNode = null;
				if (valueDecl instanceof JavaParserFieldDeclaration)
				{
					wrappedNode = ((JavaParserFieldDeclaration)valueDecl).getWrappedNode();
				}
				if (wrappedNode != null && wrappedNode instanceof FieldDeclaration)
				{
					
					for (VariableDeclarator var: ((FieldDeclaration)wrappedNode).getVariables())
					{
						if (var.getName().getIdentifier().equals(fieldName.getIdentifier()))
						{
							String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(var, m_typeSolver).toSerializedNameHierarchy();
							for (DeclContext context: m_context)
							{
								JavaIndexer.recordReference(
									m_callbackId, ReferenceKind.USAGE, qualifiedName, context.getName(),
									fieldName.getRange()
								);
							}
						}
					}
				}
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		
		super.visit(n, v);
	}
	
	@Override public void visit(final NameExpr n, final Void v)
	{
		try
		{
			recordRef(n);
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		
		super.visit(n, v);
	}

	private void recordRef(NameExpr e)
	{
		SymbolReference<? extends ValueDeclaration> ref = JavaParserFacade.get(m_typeSolver).solve(e);
		if (ref.isSolved())
		{
			ValueDeclaration valueDecl = ref.getCorrespondingDeclaration();
			
			Node wrappedNode = null;
			if (valueDecl instanceof JavaParserSymbolDeclaration)
			{
				wrappedNode = ((JavaParserSymbolDeclaration)valueDecl).getWrappedNode();
			}
			else if (valueDecl instanceof JavaParserParameterDeclaration)
			{
				wrappedNode = ((JavaParserParameterDeclaration)valueDecl).getWrappedNode();
			}
			else if (valueDecl instanceof JavaParserFieldDeclaration)
			{
				wrappedNode = ((JavaParserFieldDeclaration)valueDecl).getWrappedNode();
			}
			
			if (wrappedNode != null)
			{
				if (wrappedNode instanceof FieldDeclaration)
				{
					
					for (VariableDeclarator var: ((FieldDeclaration)wrappedNode).getVariables())
					{
						if (var.getName().getIdentifier().equals(e.getName().getIdentifier()))
						{
							String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(var, m_typeSolver).toSerializedNameHierarchy();
							for (DeclContext context: m_context)
							{
								JavaIndexer.recordReference(
									m_callbackId, ReferenceKind.USAGE, qualifiedName, context.getName(),
									e.getName().getRange()
								);
							}
						}
					}
				}
				else if (wrappedNode instanceof Parameter)
				{
					SimpleName name = ((Parameter)wrappedNode).getName();
					if (name.getBegin().isPresent())
					{
						String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
						
						JavaIndexer.recordLocalSymbol(
							m_callbackId, qualifiedName,
							e.getRange()
						);
					}
				}
				else if (wrappedNode instanceof VariableDeclarator)
				{
					if (wrappedNode.getAncestorOfType(FieldDeclaration.class).isPresent())
					{
						String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName((VariableDeclarator)wrappedNode, m_typeSolver).toSerializedNameHierarchy();
						
						for (DeclContext context: m_context)
						{
							JavaIndexer.recordReference(
								m_callbackId, ReferenceKind.USAGE, qualifiedName, context.getName(),
								e.getRange()
							);
						}
					}
					else
					{
						SimpleName name = ((VariableDeclarator)wrappedNode).getName();
						if (name.getBegin().isPresent())
						{
							String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
							
							JavaIndexer.recordLocalSymbol(
								m_callbackId, qualifiedName,
								e.getRange()
							);
						}
					}
				}
			}
		}
	}

	@Override public void visit(final MethodCallExpr n, final Void v)
	{
		String qualifiedName = "";
		if (m_context.size() > 0)
		{
			try
			{
				SymbolReference<com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration> solvedSymbol = JavaParserFacade.get(m_typeSolver).solve(n);
				if (solvedSymbol.isSolved())
				{
					qualifiedName = getQualifiedName(solvedSymbol.getCorrespondingDeclaration());
				}
				else
				{
					throw new UnsolvedSymbolException(n.getNameAsString());
				}
			}
			catch (UnsupportedOperationException e)
			{
				recordException(e, n);
			}
			catch (MethodAmbiguityException e)
			{
				recordException(e, n);
			}
			catch(StackOverflowError e)
			{
				recordError(e, n);
	        }
			catch (Exception e)
			{
				recordException(e, n);
			}
		}
		
		if (!qualifiedName.isEmpty())
		{
			SimpleName name = n.getName();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.CALL, qualifiedName, context.getName(),
					name.getRange()
				);
			}
		}
		
		super.visit(n, v);
	}
	
	
	private void recordException(Exception e, Node n)
	{
		recordExceptionOrError(e.getClass().getSimpleName(), n);
	}
	
	private void recordError(Error e, Node n)
	{
		recordExceptionOrError(e.getClass().getSimpleName(), n);
	}
	
	private void recordExceptionOrError(String e, Node n)
	{
		String beginLine = "?";
		String beginColumn = "?";
		
		if (n.getBegin().isPresent())
		{
			beginLine = n.getBegin().get().line + "";
			beginColumn = n.getBegin().get().column + "";
		}
		
		JavaIndexer.logError(m_callbackId, e + " at " + m_filePath + "<"+ beginLine + ", " + beginColumn + ">");
		JavaIndexer.recordSymbolWithLocation(
			m_callbackId, ".\tmunsolved-symbol\ts\tp", SymbolKind.TYPE_MAX, 
			n.getRange(), 
			AccessKind.DEFAULT, 
			DefinitionKind.EXPLICIT
		);
	}

	@Override public void visit(final ObjectCreationExpr n, final Void v)
	{
		String qualifiedName = "";
		if (m_context.size() > 0)
		{
			try
			{
				SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ConstructorDeclaration> solvedSymbol = JavaParserFacade.get(m_typeSolver).solve(n);
				if (solvedSymbol.isSolved())
				{
					qualifiedName = getQualifiedName(solvedSymbol.getCorrespondingDeclaration());
				}
				else
				{
					throw new UnsolvedSymbolException("constructor for " + n.getType().getNameAsString());
				}
			}
			catch (UnsupportedOperationException e)
			{
				recordException(e, n);
			}
			catch (MethodAmbiguityException e)
			{
				recordException(e, n);
			}
			catch(StackOverflowError e)
			{
				recordError(e, n);
	        }
			catch (Exception e)
			{
				recordException(e, n);
			}
		}
		
		if (!qualifiedName.isEmpty())
		{
			ClassOrInterfaceType type = n.getType();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.CALL, qualifiedName, context.getName(),
					type.getRange()
				);
			}
		}
		
		super.visit(n, v);
	}
	
	private String getQualifiedName(com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration method)
	{
		String qualifiedName = "";
		if (method instanceof JavaParserMethodDeclaration)
		{
			MethodDeclaration wrappedNode = ((JavaParserMethodDeclaration)method).getWrappedNode();
			qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(wrappedNode, m_typeSolver).toSerializedNameHierarchy();
		}
		else // todo: move this implementation somewhere else
		{
			qualifiedName = ".\tm" + method.declaringType().getQualifiedName();
			qualifiedName = qualifiedName.replace(".", "\ts\tp\tn");
			
			qualifiedName += "\ts\tp\tn" + method.getName() + "\ts";
			
			String returnType = method.getReturnType().describe();
			qualifiedName += returnType;
		//	qualifiedName += returnType.substring(returnType.lastIndexOf(".") + 1);
			
			qualifiedName += "\tp(";
			for (int i = 0; i < method.getNumberOfParams(); i++)
			{
				if(i != 0)
				{
					qualifiedName += (", ");
				}
				String paramType = method.getParam(i).describeType();
				qualifiedName += paramType;
			//	qualifiedName += paramType.substring(paramType.lastIndexOf(".") + 1);
			}
			qualifiedName = qualifiedName.concat(")");
		}
		return qualifiedName;
	}
	
	private String getQualifiedName(com.github.javaparser.symbolsolver.model.declarations.ConstructorDeclaration constructor)
	{
		String qualifiedName = "";
		if (constructor instanceof JavaParserConstructorDeclaration)
		{
			ConstructorDeclaration wrappedNode = ((JavaParserConstructorDeclaration)constructor).getWrappedNode();
			qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(wrappedNode, m_typeSolver).toSerializedNameHierarchy();
		}
		else // todo: move this implementation somewhere else
		{
			qualifiedName = ".\tm" + constructor.declaringType().getQualifiedName();
			qualifiedName = qualifiedName.replace(".", "\ts\tp\tn");
			
			qualifiedName += "\ts\tp\tn" + constructor.getName() + "\ts\tp(";
			for (int i = 0; i < constructor.getNumberOfParams(); i++)
			{
				if(i != 0)
				{
					qualifiedName += (", ");
				}
				String paramType = constructor.getParam(i).describeType();
				qualifiedName += paramType;
			//	qualifiedName += paramType.substring(paramType.lastIndexOf(".") + 1);
			}
			qualifiedName = qualifiedName.concat(")");
		}
		return qualifiedName;
	}
	
	@Override public void visit(final BlockStmt n, final Void v)
	{
		recordScope(n.getRange());
		super.visit(n, v);
	}
	
	@Override public void visit(final ArrayInitializerExpr n, final Void v)
	{
		recordScope(n.getRange());
		super.visit(n, v);
	}
	
	@Override public void visit(final SwitchStmt n, final Void v)
	{
		if (n.getRange().isPresent())
		{
			FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin());
			recordScope(Range.range(scopeStartLocation.line, scopeStartLocation.column, n.getRange().get().end.line, n.getRange().get().end.column));
		}
		super.visit(n, v);
	}
	
	private void recordScope(Optional<Range> range)
	{
		if (range.isPresent())
		{
			recordScope(range.get());
		}
	}
	
	private void recordScope(Range range)
	{
		String qualifiedName = m_filePath + "<" + range.begin.line + ":" + range.begin.column + ">";
		JavaIndexer.recordLocalSymbol(m_callbackId, qualifiedName, range.begin.line, range.begin.column, range.begin.line, range.begin.column);
		JavaIndexer.recordLocalSymbol(m_callbackId, qualifiedName, range.end.line, range.end.column, range.end.line, range.end.column);
	}
	
	@Override public void visit(final LineComment n, final Void v)
	{
		JavaIndexer.recordComment(
			m_callbackId,
			n.getRange()
		);
		super.visit(n, v);
	}

	@Override public void visit(final BlockComment n, final Void v) 
	{
		JavaIndexer.recordComment(
			m_callbackId,
			n.getRange()
		);
		super.visit(n, v);
	}
}