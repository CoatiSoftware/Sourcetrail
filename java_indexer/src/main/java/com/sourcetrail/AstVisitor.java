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
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserFieldDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserParameterDeclaration;
import com.github.javaparser.symbolsolver.javaparsermodel.declarations.JavaParserSymbolDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.MethodAmbiguityException;
import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.declarations.ValueDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.model.resolution.TypeSolver;
import com.github.javaparser.symbolsolver.model.resolution.UnsolvedSymbolException;
import com.github.javaparser.symbolsolver.model.typesystem.*;
import com.github.javaparser.symbolsolver.resolution.MethodResolutionLogic;
import com.sourcetrail.name.JavaDeclName;

public class AstVisitor extends AstVisitorAdapter
{
	protected AstVisitorClient m_client = null;
	private String m_filePath;
	private FileContent m_fileContent;
	private TypeSolver m_typeSolver;
	private List<DeclContext> m_context = new ArrayList<DeclContext>();
	
	public AstVisitor(AstVisitorClient client, String filePath, FileContent fileContent, TypeSolver typeSolver)
	{		
		m_client = client;
		m_filePath = filePath;
		m_fileContent = fileContent;
		m_typeSolver = typeSolver;
		
		m_context.add(new DeclContext("/\tm" + filePath + "\ts\tp"));
	}
	
	// --- record declarations ---
	
	@Override 
	public Boolean visit(final PackageDeclaration n, final Void v)
	{
		Name name = n.getName();
		
		m_client.recordSymbolWithLocationAndScope( 
			JavaparserDeclNameResolver.getQualifiedName(name).toNameHierarchy().serialize(), 
			SymbolKind.PACKAGE,
			name.getRange(), 
			n.getRange(),
			AccessKind.NONE, 
			DefinitionKind.EXPLICIT
		);
		
		while (name.getQualifier().isPresent())
		{
			name = name.getQualifier().get();
			m_client.recordSymbol(
				JavaparserDeclNameResolver.getQualifiedName(name).toNameHierarchy().serialize(), 
				SymbolKind.PACKAGE, 
				AccessKind.NONE, 
				DefinitionKind.EXPLICIT
			);
		}
		
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final ClassOrInterfaceDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy().serialize();
		
		m_client.recordSymbolWithLocationAndScope(
			qualifiedName, (n.isInterface() ? SymbolKind.INTERFACE : SymbolKind.CLASS),
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
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
	}
	
	@Override
	public Boolean visit(final TypeParameter n, final Void v)
	{
		// todo: test recording of typeargument of type parameter bound type??
		
		
		if (m_context.size() != 1)
		{
			 // throw something!
		}

		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy().serialize();
		
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
		
		m_client.recordSymbolWithLocation(
			qualifiedName, SymbolKind.TYPE_PARAMETER, 
			range,
			AccessKind.TYPE_PARAMETER, 
			DefinitionKind.EXPLICIT
		);

		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
	}
	
	@Override 
	public Boolean visit(final EnumDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy().serialize();
		
		m_client.recordSymbolWithLocationAndScope(
			qualifiedName, SymbolKind.ENUM, 
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
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
	}
	
	@Override
	public Boolean visit(final EnumConstantDeclaration n, final Void v)
	{
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy().serialize();
		
		m_client.recordSymbolWithLocation(
			qualifiedName, SymbolKind.ENUM_CONSTANT,
			n.getRange(), 
			AccessKind.NONE, 
			DefinitionKind.EXPLICIT
		);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
	}
	
	@Override
	public Boolean visit(final ConstructorDeclaration n, final Void v)
	{
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy().serialize();
		
		m_client.recordSymbolWithLocationAndScope(
			qualifiedName, SymbolKind.METHOD,
			name.getRange(), 
			n.getRange(), 
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
			DefinitionKind.EXPLICIT
		);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
	}
	
	@Override 
	public Boolean visit(final MethodDeclaration n, final Void v)
	{
		if (m_client.getInterrupted())
		{
			// return something that is not null in order to cancel AST traversal;
			return false;
		}
		
		SimpleName name = n.getName();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy().serialize();
		
		m_client.recordSymbolWithLocationAndScope(
			qualifiedName, SymbolKind.METHOD, 
			name.getRange(), 
			n.getRange(), 
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
			DefinitionKind.EXPLICIT
		);
		
		
// test this!
		com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration overridden = getOverridden(n);
		if (overridden != null && (overridden instanceof JavaParserMethodDeclaration))
		{
			String overriddenName = JavaparserDeclNameResolver.getQualifiedDeclName(((JavaParserMethodDeclaration)overridden).getWrappedNode(), m_typeSolver).toNameHierarchy().serialize();
			
			m_client.recordReference(
				ReferenceKind.OVERRIDE, overriddenName, qualifiedName, 
				name.getRange()
			);
		}
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
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

	@Override 
	public Boolean visit(final FieldDeclaration n, final Void v)
	{
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		
		for (VariableDeclarator declarator: n.getVariables())
		{
			String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(declarator, m_typeSolver).toNameHierarchy().serialize();
			SimpleName name = declarator.getName();
			m_client.recordSymbolWithLocation(
				qualifiedName, SymbolKind.FIELD, 
				name.getRange(),
				AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), 
				DefinitionKind.EXPLICIT
			);

			m_context.add(new DeclContext(qualifiedName));
		}
		
		Boolean result = super.visit(n, v);
		m_context = parentContext;
		return result;
	}
	
	@Override 
	public Boolean visit(final VariableDeclarationExpr n, final Void v)
	{
		for (VariableDeclarator declarator: n.getVariables())
		{
			SimpleName name = declarator.getName();

			if (name.getBegin().isPresent())
			{
				String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
				
				m_client.recordLocalSymbol(qualifiedName, name.getRange());
			}
		}
		
		// don't change the context here.
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final Parameter n, final Void v)
	{
		SimpleName name = n.getName();
		
		if (name.getBegin().isPresent())
		{
			String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
			
			m_client.recordLocalSymbol(qualifiedName, name.getRange());
		}
		
		// don't change the context here.
		return super.visit(n, v);
	}
	
	
	// --- record references ---

	@Override 
	public Boolean visit(final ImportDeclaration n, final Void v)
	{
		Name name = n.getName();
		
		if (n.isAsterisk())
		{
			String importedName = JavaparserDeclNameResolver.getQualifiedName(name).toNameHierarchy().serialize();
			for (DeclContext context: m_context)
			{
				m_client.recordReference(
					ReferenceKind.IMPORT, 
					importedName, context.getName(), 
					name.getRange()
				);
			}
		}
		else
		{
			try
			{
				List<JavaDeclName> importedDeclNames = new ArrayList<>();
				
				SymbolReference<ReferenceTypeDeclaration> solvedTypeDeclatarion = m_typeSolver.tryToSolveType(name.asString());
				if (solvedTypeDeclatarion.isSolved())
				{
					importedDeclNames.add(JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(solvedTypeDeclatarion.getCorrespondingDeclaration(), m_typeSolver));
				}
				else if (n.isStatic())
				{
					String typeName = name.getQualifier().get().asString();
					String memberName = name.getIdentifier();
	
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
				}
				
				if (!importedDeclNames.isEmpty())
				{
					for (JavaDeclName importedDeclName: importedDeclNames)
					{
						String nameHierarchy = importedDeclName.toNameHierarchy().serialize();
						for (DeclContext context: m_context)
						{
							m_client.recordReference(
								ReferenceKind.IMPORT, 
								nameHierarchy, context.getName(), 
								n.getRange()
							);
						}
					}
				}
				else
				{
					m_client.recordError("Import not found: " + name, true, true, n.getRange());
				}
			}
			catch (Exception e)
			{
				recordException(e, n);
			}
		}
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final ClassOrInterfaceType n, final Void v)
	{
		try
		{
			for (DeclContext context: m_context)
			{
				String referencedName = JavaparserTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toNameHierarchy().serialize();

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
				
				m_client.recordReference(
					getTypeReferenceKind(), referencedName, context.getName(), range
				);
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		return super.visit(n, v);
	}
	
	@Override
	public Boolean visit(final PrimitiveType n, final Void v)
	{
		try
		{
			String referencedName = JavaparserTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toNameHierarchy().serialize();
			
			m_client.recordSymbol(
				referencedName, SymbolKind.BUILTIN_TYPE,
				AccessKind.NONE, 
				DefinitionKind.EXPLICIT
			);
			
			for (DeclContext context: m_context)
			{		
				m_client.recordReference(
					getTypeReferenceKind(), referencedName, context.getName(), 
					n.getRange()
				);
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final VoidType n, final Void v)
	{
		try
		{
			String referencedName = JavaparserTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toNameHierarchy().serialize();
			
			m_client.recordSymbol(
				referencedName, SymbolKind.BUILTIN_TYPE, AccessKind.NONE, DefinitionKind.EXPLICIT
			);
			
			for (DeclContext context: m_context)
			{
				m_client.recordReference(
					getTypeReferenceKind(), referencedName, context.getName(), n.getRange()
				);
			}
		}
		catch (Exception e)
		{
			recordException(e, n);
		}
		
		return super.visit(n, v);
	}

	@Override 
	public Boolean visit(final FieldAccessExpr n, final Void v)
	{
		SimpleName fieldName = n.getName();
		
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
							String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(var, m_typeSolver).toNameHierarchy().serialize();
							for (DeclContext context: m_context)
							{
								m_client.recordReference(
									ReferenceKind.USAGE, qualifiedName, context.getName(),
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
		
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final NameExpr n, final Void v)
	{
		try
		{
			SymbolReference<? extends ValueDeclaration> ref = JavaParserFacade.get(m_typeSolver).solve(n);
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
							if (var.getName().getIdentifier().equals(n.getName().getIdentifier()))
							{
								String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(var, m_typeSolver).toNameHierarchy().serialize();
								for (DeclContext context: m_context)
								{
									m_client.recordReference(
										ReferenceKind.USAGE, qualifiedName, context.getName(),
										n.getName().getRange()
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
							
							m_client.recordLocalSymbol(
								qualifiedName,
								n.getRange()
							);
						}
					}
					else if (wrappedNode instanceof VariableDeclarator)
					{
						if (wrappedNode.getAncestorOfType(FieldDeclaration.class).isPresent())
						{
							String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName((VariableDeclarator)wrappedNode, m_typeSolver).toNameHierarchy().serialize();
							
							for (DeclContext context: m_context)
							{
								m_client.recordReference(
									ReferenceKind.USAGE, qualifiedName, context.getName(),
									n.getRange()
								);
							}
						}
						else
						{
							SimpleName name = ((VariableDeclarator)wrappedNode).getName();
							if (name.getBegin().isPresent())
							{
								String qualifiedName = m_filePath + "<" + name.getBegin().get().line + ":" + name.getBegin().get().column + ">";
								
								m_client.recordLocalSymbol(
									qualifiedName,
									n.getRange()
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
		
		return super.visit(n, v);
	}

	@Override 
	public Boolean visit(final MethodCallExpr n, final Void v)
	{
		String qualifiedName = "";
		if (m_context.size() > 0)
		{
			try
			{
				SymbolReference<com.github.javaparser.symbolsolver.model.declarations.MethodDeclaration> solvedSymbol = JavaParserFacade.get(m_typeSolver).solve(n);
				if (solvedSymbol.isSolved())
				{
					qualifiedName = JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(solvedSymbol.getCorrespondingDeclaration(), m_typeSolver).toNameHierarchy().serialize();
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
				m_client.recordReference(
					ReferenceKind.CALL, qualifiedName, context.getName(),
					name.getRange()
				);
			}
		}
		
		return super.visit(n, v);
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
		
		m_client.logError(e + " at " + m_filePath + "<"+ beginLine + ", " + beginColumn + ">");
		m_client.recordSymbolWithLocation(
			JavaDeclName.unsolved().toNameHierarchy().serialize(), SymbolKind.TYPE_MAX, 
			n.getRange(), 
			AccessKind.DEFAULT, 
			DefinitionKind.EXPLICIT
		);
	}

	@Override 
	public Boolean visit(final ObjectCreationExpr n, final Void v)
	{
		String qualifiedName = "";
		if (m_context.size() > 0)
		{
			try
			{
				SymbolReference<com.github.javaparser.symbolsolver.model.declarations.ConstructorDeclaration> solvedSymbol = JavaParserFacade.get(m_typeSolver).solve(n);
				if (solvedSymbol.isSolved())
				{
					qualifiedName = JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(solvedSymbol.getCorrespondingDeclaration(), m_typeSolver).toNameHierarchy().serialize();
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
				m_client.recordReference(
					ReferenceKind.CALL, qualifiedName, context.getName(),
					type.getRange()
				);
			}
		}
		
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final BlockStmt n, final Void v)
	{
		recordScope(n.getRange());
		return super.visit(n, v);
	}
	
	@Override 
	public Boolean visit(final ArrayInitializerExpr n, final Void v)
	{
		recordScope(n.getRange());
		return super.visit(n, v);
	}
	
	@Override
	public Boolean visit(final SwitchStmt n, final Void v)
	{
		if (n.getRange().isPresent())
		{
			FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin());
			recordScope(Range.range(scopeStartLocation.line, scopeStartLocation.column, n.getRange().get().end.line, n.getRange().get().end.column));
		}
		return super.visit(n, v);
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
		m_client.recordLocalSymbol(qualifiedName, Range.range(range.begin, range.begin));
		m_client.recordLocalSymbol(qualifiedName, Range.range(range.end, range.end));
	}
	
	@Override 
	public Boolean visit(final LineComment n, final Void v)
	{
		m_client.recordComment(n.getRange());
		return super.visit(n, v);
	}

	@Override 
	public Boolean visit(final BlockComment n, final Void v) 
	{
		m_client.recordComment(n.getRange());
		return super.visit(n, v);
	}
}