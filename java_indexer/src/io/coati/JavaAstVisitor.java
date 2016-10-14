package io.coati;

import java.lang.String;
import java.util.List;
import java.util.ArrayList;

import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.ConstructorDeclaration;
import com.github.javaparser.ast.body.EnumConstantDeclaration;
import com.github.javaparser.ast.body.EnumDeclaration;
import com.github.javaparser.ast.body.FieldDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.expr.ArrayInitializerExpr;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.QualifiedNameExpr;
import com.github.javaparser.ast.expr.VariableDeclarationExpr;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.PrimitiveType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.ast.ImportDeclaration;
import com.github.javaparser.ast.Modifier;
import com.github.javaparser.ast.Node;
import com.github.javaparser.ast.PackageDeclaration;
import com.github.javaparser.ast.TypeParameter;

import me.tomassetti.symbolsolver.javaparsermodel.JavaParserFacade;
import me.tomassetti.symbolsolver.javaparsermodel.UnsolvedSymbolException;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserMethodDeclaration;
import me.tomassetti.symbolsolver.javaparsermodel.declarations.JavaParserSymbolDeclaration;
import me.tomassetti.symbolsolver.model.declarations.MethodAmbiguityException;
import me.tomassetti.symbolsolver.model.declarations.TypeDeclaration;
import me.tomassetti.symbolsolver.model.declarations.ValueDeclaration;
import me.tomassetti.symbolsolver.model.invokations.MethodUsage;
import me.tomassetti.symbolsolver.model.resolution.SymbolReference;
import me.tomassetti.symbolsolver.model.resolution.TypeSolver;
import me.tomassetti.symbolsolver.model.typesystem.*;

public class JavaAstVisitor extends JavaAstVisitorAdapter
{
	private int m_callbackId = -1;
	private String m_filePath;
	private FileContent m_fileContent;
	private TypeSolver m_typeSolver;
	private List<DeclContext> m_context = new ArrayList<DeclContext>();
	private boolean m_verbose = false;
	static int errorCount = 0;
	
	public JavaAstVisitor(int callbackId, String filePath, FileContent fileContent, TypeSolver typeSolver)
	{
		if (m_verbose)
		{
			System.out.println("indexing file: " + filePath);
		}
		
		m_callbackId = callbackId;
		m_filePath = filePath;
		m_fileContent = fileContent;
		m_typeSolver = typeSolver;
		
		String[] filePathParts = filePath.split("/");
		String fileName = filePathParts[filePathParts.length - 1];
		m_context.add(new DeclContext(fileName + "\ts\tp"));
	}
	
	
	// --- record declarations ---
	
	@Override public void visit(final PackageDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getName();
			
		String packageName = JavaparserDeclNameResolver.getQualifiedName(nameExpr).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, packageName, SymbolType.PACKAGE,
			nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column,
			AccessKind.NONE, false
		);
		
		super.visit(n, v);
	}
	
	@Override public void visit(final ClassOrInterfaceDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getNameExpr();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, (n.isInterface() ? SymbolType.INTERFACE : SymbolType.CLASS),
			nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column,
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), false
		);

		FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin().line, n.getBegin().column);
		recordScope(scopeStartLocation.line, scopeStartLocation.column, n.getEnd().line, n.getEnd().column);
		
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
		
		JavaIndexer.recordSymbol(
			m_callbackId, qualifiedName, SymbolType.TYPE_PARAMETER, 
			n.getBegin().line, n.getBegin().column, n.getBegin().line, n.getBegin().column + n.getName().length() - 1,
			AccessKind.TYPE_PARAMETER, false
		);

		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final EnumDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getNameExpr();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, SymbolType.ENUM, 
			nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column,
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), false
		);
		
		FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin().line, n.getBegin().column);
		recordScope(scopeStartLocation.line, scopeStartLocation.column, n.getEnd().line, n.getEnd().column);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final EnumConstantDeclaration n, final Void v)
	{
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbol(
			m_callbackId, qualifiedName, SymbolType.ENUM_CONSTANT,
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column, 
			AccessKind.NONE, false
		);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final ConstructorDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getNameExpr();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, SymbolType.METHOD,
			nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column, 
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), false
		);
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final MethodDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getNameExpr();
		
		String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toSerializedNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, SymbolType.METHOD, 
			nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column, 
			AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), false
		);
		
		
// test this!
		me.tomassetti.symbolsolver.model.declarations.MethodDeclaration overridden = getOverridden(n);
		if (overridden != null && (overridden instanceof JavaParserMethodDeclaration))
		{
			String overriddenName = JavaparserDeclNameResolver.getQualifiedDeclName(((JavaParserMethodDeclaration)overridden).getWrappedNode(), m_typeSolver).toSerializedNameHierarchy();
			
			JavaIndexer.recordReference(
				m_callbackId, ReferenceKind.OVERRIDE, overriddenName, qualifiedName, 
				nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column
			);
		}
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	private me.tomassetti.symbolsolver.model.declarations.MethodDeclaration getOverridden(MethodDeclaration overrider)
	{
		com.github.javaparser.ast.body.TypeDeclaration<?> scopeNode = overrider.getParentNodeOfType(com.github.javaparser.ast.body.TypeDeclaration.class);
		if (scopeNode instanceof ClassOrInterfaceDeclaration)
		{
			List<TypeUsage> parameterTypes = new ArrayList<>();
			try
			{
				for (Parameter parameter: overrider.getParameters())
				{
					Type parameterType = parameter.getType();
					parameterTypes.add(JavaParserFacade.get(m_typeSolver).convert(parameterType, parameterType));
				}
				
				TypeDeclaration scopeDecl = JavaParserFacade.get(m_typeSolver).getTypeDeclaration((ClassOrInterfaceDeclaration)scopeNode);
				for (ReferenceTypeUsage ancestor: scopeDecl.getAllAncestors())
				{
					try
					{
						SymbolReference<me.tomassetti.symbolsolver.model.declarations.MethodDeclaration> solvedMethod = ancestor.solveMethod(overrider.getName(), parameterTypes);
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
		}
		return null;
	}

	@Override public void visit(final FieldDeclaration n, final Void v)
	{
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		
		List<VariableDeclarator> variableDeclarators = n.getVariables();
		for (int i = 0; i < variableDeclarators.size(); i++)
		{
			VariableDeclarator varDecl = variableDeclarators.get(i);
			String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(varDecl, m_typeSolver).toSerializedNameHierarchy();		
			VariableDeclaratorId varDeclId = varDecl.getId();
			
			JavaIndexer.recordSymbol(
				m_callbackId, qualifiedName, SymbolType.FIELD, 
				varDeclId.getBegin().line, varDeclId.getBegin().column, varDeclId.getEnd().line, varDeclId.getEnd().column,
				AccessKind.fromAccessSpecifier(Modifier.getAccessSpecifier(n.getModifiers())), false
			);

			m_context.add(new DeclContext(qualifiedName));
		}
		
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final VariableDeclarationExpr n, final Void v)
	{
		for (VariableDeclarator declarator: n.getVars())
		{
			VariableDeclaratorId identifier = declarator.getId();
			String qualifiedName = m_filePath + "<" + identifier.getBegin().line + ":" + identifier.getBegin().column + ">";
			
			JavaIndexer.recordLocalSymbol(
				m_callbackId, qualifiedName, 
				identifier.getBegin().line, identifier.getBegin().column, identifier.getEnd().line, identifier.getEnd().column
			);
			
		}
		
		// don't change the context here.
		super.visit(n, v);
	}
	
	@Override public void visit(final Parameter n, final Void v)
	{
		VariableDeclaratorId identifier = n.getId();
		String qualifiedName = m_filePath + "<" + identifier.getBegin().line + ":" + identifier.getBegin().column + ">";
		
		JavaIndexer.recordLocalSymbol(
			m_callbackId, qualifiedName, 
			identifier.getBegin().line, identifier.getBegin().column, identifier.getEnd().line, identifier.getEnd().column
		);
		
		// don't change the context here.
		super.visit(n, v);
	}
	
	
	// --- record references ---

	
	@Override public void visit(final ImportDeclaration n, final Void v)
	{
		if (n.isAsterisk())
		{
			NameExpr nameExpr = n.getName();
			String importedName = JavaparserDeclNameResolver.getQualifiedName(nameExpr).toSerializedNameHierarchy();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.IMPORT, 
					importedName, context.getName(), 
					nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column
				);
			}
		}
		else
		{
			try
			{
				NameExpr nameExpr = n.getName();
				List<JavaDeclName> importedDeclNames = new ArrayList<>();

				SymbolReference<TypeDeclaration> symbolReference = m_typeSolver.tryToSolveType(
					JavaparserDeclNameResolver.getQualifiedName(nameExpr).toString()
				);
				if (symbolReference.isSolved())
				{
					importedDeclNames.add(JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(
						symbolReference.getCorrespondingDeclaration(), m_typeSolver
					));
				}
				else
				{
					if (nameExpr instanceof QualifiedNameExpr) 
					{
						String typeName = ((QualifiedNameExpr)nameExpr).getQualifier().toString();
						String memberName = nameExpr.getName();
						
						me.tomassetti.symbolsolver.model.declarations.TypeDeclaration ref = m_typeSolver.solveType(typeName);
						
						for (me.tomassetti.symbolsolver.model.declarations.MethodDeclaration methodDecl: ref.getDeclaredMethods()) // look for method
						{
							if (methodDecl.getName().equals(memberName))
							{
								importedDeclNames.add(
									JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(methodDecl, m_typeSolver
								));
							}
						}
						if (importedDeclNames.isEmpty() && ref.hasField(memberName)) // look for field
						{
							JavaDeclName importedTypeDeclName = JavaSymbolSolverDeclNameResolver.getQualifiedDeclName(ref, m_typeSolver);
							if (importedTypeDeclName != null)
							{
								JavaDeclName importedDeclName = new JavaDeclName(memberName);
								importedDeclName.setParent(importedTypeDeclName);
								importedDeclNames.add(importedDeclName);
							}
						}
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
								nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column
							);
						}
					}
				}
				else
				{
					JavaIndexer.recordError(
						m_callbackId, "Import not found.", true, true, 
						nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column
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

				int beginLine = n.getBegin().line;
				int beginColumn = n.getBegin().column;
				int endLine = n.getBegin().line;
				int endColumn = n.getBegin().column + n.getName().length() - 1;
				
				if (n.getScope() != null)
				{
					endLine = n.getScope().getEnd().line;
					endColumn = n.getScope().getEnd().column + n.getName().length() + 1; // +1 for separator
				}
				
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					beginLine, beginColumn, endLine, endColumn
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
			
			JavaIndexer.recordSymbolWithoutLocation(
				m_callbackId, referencedName, SymbolType.BUILTIN_TYPE,
				AccessKind.NONE, true
			);
			
			for (DeclContext context: m_context)
			{		
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column
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
			
			JavaIndexer.recordSymbolWithoutLocation(
				m_callbackId, referencedName, SymbolType.BUILTIN_TYPE,
				AccessKind.NONE, true
			);
			
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column
				);
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
			if (valueDecl instanceof JavaParserSymbolDeclaration)
			{
				Node wrappedNode = null;
				if (valueDecl instanceof JavaParserSymbolDeclaration)
				{
					wrappedNode = ((JavaParserSymbolDeclaration)valueDecl).getWrappedNode();
				}
				if (wrappedNode != null)
				{
					if (wrappedNode instanceof Parameter)
					{
						VariableDeclaratorId identifier = ((Parameter)wrappedNode).getId();
						String qualifiedName = m_filePath + "<" + identifier.getBegin().line + ":" + identifier.getBegin().column + ">";
						
						JavaIndexer.recordLocalSymbol(
							m_callbackId, qualifiedName,
							e.getBegin().line, e.getBegin().column, e.getEnd().line, e.getEnd().column
						);
					}
					else if (wrappedNode instanceof VariableDeclarator)
					{
						if (getFieldDeclarationInParentHierarchy(wrappedNode) != null)
						{
							String qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName((VariableDeclarator)wrappedNode, m_typeSolver).toSerializedNameHierarchy();
							
							for (DeclContext context: m_context)
							{
								JavaIndexer.recordReference(
									m_callbackId, ReferenceKind.USAGE, qualifiedName, context.getName(),
									e.getBegin().line, e.getBegin().column, e.getEnd().line, e.getEnd().column
								);
							}
						}
						else
						{
							VariableDeclaratorId identifier = ((VariableDeclarator)wrappedNode).getId();
							String qualifiedName = m_filePath + "<" + identifier.getBegin().line + ":" + identifier.getBegin().column + ">";
							
							JavaIndexer.recordLocalSymbol(
								m_callbackId, qualifiedName,
								e.getBegin().line, e.getBegin().column, e.getEnd().line, e.getEnd().column
							);
						}
					}
				}
			}
		}
	}
	
	private static FieldDeclaration getFieldDeclarationInParentHierarchy(Node decl)
	{
		FieldDeclaration context = null;
		{
			Node parentNode = decl.getParentNode();
			while (parentNode != null && !(parentNode instanceof FieldDeclaration))
			{
				parentNode = parentNode.getParentNode();
			}
			
			if (parentNode != null)
			{
				context = (FieldDeclaration)parentNode;
			}
		}
		return context;
	}

	@Override public void visit(final MethodCallExpr n, final Void v)
	{
		String qualifiedName = "";
		if (m_context.size() > 0)
		{
			try
			{
				MethodUsage solvedMethod = JavaParserFacade.get(m_typeSolver).solveMethodAsUsage(n);
				qualifiedName = getQualifiedName(solvedMethod);
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
			NameExpr nameExpr = n.getNameExpr();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.CALL, qualifiedName, context.getName(),
					nameExpr.getBegin().line, nameExpr.getBegin().column, nameExpr.getEnd().line, nameExpr.getEnd().column
				);
			}
		}
		
		super.visit(n, v);
	}
	
	
	private void recordException(Exception e, Node n)
	{
		JavaIndexer.recordSymbol(
			m_callbackId, "unsolved-symbol\ts\tp", SymbolType.TYPE_MAX, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column, 
			AccessKind.DEFAULT, false
		);
		errorCount++;
		if (m_verbose)
		{
			System.out.println(e + " at location " + n.getBegin().line + ", " + n.getBegin().column + " [errors: " + errorCount + "]");
		}
		
	}
	
	private void recordError(Error e, Node n)
	{
		JavaIndexer.recordSymbol(
			m_callbackId, "unsolved-symbol\ts\tp", SymbolType.TYPE_MAX, 
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column, 
			AccessKind.DEFAULT, false
		);
		errorCount++;
		if (m_verbose)
		{
			System.out.println(e + " at location " + n.getBegin().line + ", " + n.getBegin().column + " [errors: " + errorCount + "]");
		}
	}


	/*
	@Override public void visit(final ObjectCreationExpr n, final Void v)
	{
		String qualifiedName = "";
		if (m_context.size() > 0)
		{
			try
			{
				ClassOrInterfaceDeclaration decl = Utility.getJavaparserDeclForType(n.getType(), m_typeSolver);
				if (decl != null)
				{
					TODO: implement when there is a method to get the constructor decl for a constructor expression.
					
					for (BodyDeclaration member: decl.getMembers())
					{
						if (member instanceof ConstructorDeclaration)
						{
							ConstructorDeclaration constructorDecl = (ConstructorDeclaration)member;
							
							for (Expression arg: n.getArgs())
							{
								JavaParserFacade.get(m_typeSolver).getType(arg).asReferenceTypeUsage().getQualifiedName();
							}
							
							constructorDecl.getParameters().get(0).getType().;
						}
					}
				}
				
			    
				qualifiedName = getQualifiedName(methodUsage.get());
			}
			catch (UnsupportedOperationException e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBegin().line + ", " + n.getBegin().column);
				}
			}
			catch (MethodAmbiguityException e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBegin().line + ", " + n.getBegin().column);
				}
			}
			catch(StackOverflowError e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBegin().line + ", " + n.getBegin().column);
				}
	        }
			catch (Exception e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBegin().line + ", " + n.getBegin().column);
				}
			}
		}
		
		if (!qualifiedName.isEmpty())
		{
			ClassOrInterfaceType type = n.getType();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordRef(
					m_callbackId, ReferenceType.CALL.getValue(), qualifiedName, context.getName(),
					type.getBegin().line, type.getBegin().column, type.getEnd().line, type.getEnd().column
				);
			}
		}
		
		super.visit(n, v);
	}
	*/
	private String getQualifiedName(MethodUsage solvedMethod)
	{
		me.tomassetti.symbolsolver.model.declarations.MethodDeclaration methodDecl = solvedMethod.getDeclaration();
		String qualifiedName = "";
		if (methodDecl instanceof JavaParserMethodDeclaration)
		{
			MethodDeclaration wrappedNode = ((JavaParserMethodDeclaration)methodDecl).getWrappedNode();
			qualifiedName = JavaparserDeclNameResolver.getQualifiedDeclName(wrappedNode, m_typeSolver).toSerializedNameHierarchy();
		}
		else // todo: move this implementation somewhere else
		{
			qualifiedName = solvedMethod.declaringType().getQualifiedName();
			qualifiedName = qualifiedName.replace(".", "\ts\tp\tn");
			
			qualifiedName += "\ts\tp\tn" + solvedMethod.getName() + "\ts";
			
			String returnType = solvedMethod.returnType().describe();
			qualifiedName += returnType;
		//	qualifiedName += returnType.substring(returnType.lastIndexOf(".") + 1);
			
			qualifiedName += "\tp(";
			for (int i = 0; i < solvedMethod.getParamTypes().size(); i++)
			{
				if(i != 0)
				{
					qualifiedName += (", ");
				}
				String paramType = solvedMethod.getParamTypes().get(i).describe();
				qualifiedName += paramType;
			//	qualifiedName += paramType.substring(paramType.lastIndexOf(".") + 1);
			}
			qualifiedName = qualifiedName.concat(")");
		}
		return qualifiedName;
	}
	
	@Override public void visit(final BlockStmt n, final Void v)
	{
		recordScope(n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column);
		super.visit(n, v);
	}
	
	@Override public void visit(final ArrayInitializerExpr n, final Void v)
	{
		recordScope(n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column);
		super.visit(n, v);
	}
	
	@Override public void visit(final SwitchStmt n, final Void v)
	{
		FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBegin().line, n.getBegin().column);
		recordScope(scopeStartLocation.line, scopeStartLocation.column, n.getEnd().line, n.getEnd().column);
		super.visit(n, v);
	}
	
	private void recordScope(int beginLine, int beginColumn, int endLine, int endColumn)
	{
		String qualifiedName = m_filePath + "<" + beginLine + ":" + beginColumn + ">";
		JavaIndexer.recordLocalSymbol(m_callbackId, qualifiedName, beginLine, beginColumn, beginLine, beginColumn);
		JavaIndexer.recordLocalSymbol(m_callbackId, qualifiedName, endLine, endColumn, endLine, endColumn);
	}
	
	@Override public void visit(final LineComment n, final Void v)
	{
		JavaIndexer.recordComment(
			m_callbackId,
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column
		);
		super.visit(n, v);
	}

	@Override public void visit(final BlockComment n, final Void v) 
	{
		JavaIndexer.recordComment(
			m_callbackId,
			n.getBegin().line, n.getBegin().column, n.getEnd().line, n.getEnd().column
		);
		super.visit(n, v);
	}
}