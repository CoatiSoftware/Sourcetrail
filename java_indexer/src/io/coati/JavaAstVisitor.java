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
import com.github.javaparser.ast.body.ModifierSet;
import com.github.javaparser.ast.body.Parameter;
import com.github.javaparser.ast.body.VariableDeclarator;
import com.github.javaparser.ast.body.VariableDeclaratorId;
import com.github.javaparser.ast.comments.BlockComment;
import com.github.javaparser.ast.comments.LineComment;
import com.github.javaparser.ast.expr.ArrayInitializerExpr;
import com.github.javaparser.ast.expr.MethodCallExpr;
import com.github.javaparser.ast.expr.NameExpr;
import com.github.javaparser.ast.expr.VariableDeclarationExpr;
import com.github.javaparser.ast.stmt.BlockStmt;
import com.github.javaparser.ast.stmt.SwitchStmt;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.PrimitiveType;
import com.github.javaparser.ast.type.Type;
import com.github.javaparser.ast.type.VoidType;
import com.github.javaparser.ast.ImportDeclaration;
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
	
	public JavaAstVisitor(int callbackId, String filePath, FileContent fileContent, TypeSolver typeSolver)
	{
		m_callbackId = callbackId;
		m_filePath = filePath;
		m_fileContent = fileContent;
		m_typeSolver = typeSolver;
		
		String[] filePathParts = filePath.split("/");
		String fileName = filePathParts[filePathParts.length - 1];
		m_context.add(new DeclContext(fileName + "\t\r"));
	}
	
	
	// --- record declarations ---
	
	@Override public void visit(final PackageDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getName();
			
		String packageName = JavaDeclNameResolver.getQualifiedName(nameExpr).toNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, packageName, SymbolType.PACKAGE,
			nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn(), 
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn(),
			AccessKind.NONE, false
		);
		
		super.visit(n, v);
	}
	
	@Override public void visit(final ClassOrInterfaceDeclaration n, final Void v)
	{
		NameExpr nameExpr = n.getNameExpr();
		
		String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, (n.isInterface() ? SymbolType.INTERFACE : SymbolType.CLASS),
			nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn(), 
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn(),
			AccessKind.fromAccessSpecifier(ModifierSet.getAccessSpecifier(n.getModifiers())), false
		);

		FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBeginLine(), n.getBeginColumn());
		recordScope(scopeStartLocation.line, scopeStartLocation.column, n.getEndLine(), n.getEndColumn());
		
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
		qualifiedName += "\n";
		qualifiedName += n.getName() + "\t\r";
		
		JavaIndexer.recordSymbol(
			m_callbackId, qualifiedName, SymbolType.TYPE_PARAMETER, 
			n.getBeginLine(), n.getBeginColumn(), n.getBeginLine(), n.getBeginColumn() + n.getName().length() - 1,
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
		
		String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, SymbolType.ENUM, 
			nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn(), 
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn(),
			AccessKind.fromAccessSpecifier(ModifierSet.getAccessSpecifier(n.getModifiers())), false
		);
		
		FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBeginLine(), n.getBeginColumn());
		recordScope(scopeStartLocation.line, scopeStartLocation.column, n.getEndLine(), n.getEndColumn());
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	@Override public void visit(final EnumConstantDeclaration n, final Void v)
	{
		String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy();
		
		JavaIndexer.recordSymbol(
			m_callbackId, qualifiedName, SymbolType.ENUM_CONSTANT,
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn(), 
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
		
		String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, SymbolType.METHOD,
			nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn(), 
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn(), 
			AccessKind.fromAccessSpecifier(ModifierSet.getAccessSpecifier(n.getModifiers())), false
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
		
		String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(n, m_typeSolver).toNameHierarchy();
		
		JavaIndexer.recordSymbolWithScope(
			m_callbackId, qualifiedName, SymbolType.METHOD, 
			nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn(), 
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn(), 
			AccessKind.fromAccessSpecifier(ModifierSet.getAccessSpecifier(n.getModifiers())), false
		);
		
		
/// test this!
		Node parent = n.getParentNode();
		
		me.tomassetti.symbolsolver.model.declarations.MethodDeclaration overridden = getOverridden(n, parent);
		if (overridden != null && (overridden instanceof JavaParserMethodDeclaration))
		{
			String overriddenName = JavaDeclNameResolver.getQualifiedDeclName(((JavaParserMethodDeclaration)overridden).getWrappedNode(), m_typeSolver).toNameHierarchy();
			
			JavaIndexer.recordReference(
				m_callbackId, ReferenceKind.OVERRIDE, overriddenName, qualifiedName, 
				nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn()
			);
		}
		
		
		
		
		
		
		List<DeclContext> parentContext = m_context;
		m_context = new ArrayList<DeclContext>();
		m_context.add(new DeclContext(qualifiedName));
		super.visit(n, v);
		m_context = parentContext;
	}
	
	private me.tomassetti.symbolsolver.model.declarations.MethodDeclaration getOverridden(MethodDeclaration overrider, Node searchScope)
	{
		List<ClassOrInterfaceType> ancestors = new ArrayList<>();

		if (searchScope instanceof ClassOrInterfaceDeclaration)
		{
			ancestors.addAll(((ClassOrInterfaceDeclaration)searchScope).getImplements());
			ancestors.addAll(((ClassOrInterfaceDeclaration)searchScope).getExtends());
		}
		if (searchScope instanceof EnumDeclaration)
		{
			ancestors.addAll(((EnumDeclaration)searchScope).getImplements());
		}
		
		if (!ancestors.isEmpty())
		{
			boolean parametersResolved = true;
			List<TypeUsage> parameterTypes = new ArrayList<>();
			try
			{
				for (Parameter parameter: overrider.getParameters())
				{
					Type parameterType = parameter.getType();
					parameterTypes.add(JavaParserFacade.get(m_typeSolver).convert(parameterType, parameterType));
				}
			}
			catch (UnsolvedSymbolException e)
			{
				parametersResolved = false;
			}
			
			if (parametersResolved)
			{
				for (ClassOrInterfaceType ancestor: ancestors)
				{
					try
					{
						TypeUsage ancestorTypeUsage = JavaParserFacade.get(m_typeSolver).convert(ancestor, ancestor);
						if (ancestorTypeUsage.isReferenceType())
						{
							SymbolReference<me.tomassetti.symbolsolver.model.declarations.MethodDeclaration> solvedMethod = ancestorTypeUsage.asReferenceTypeUsage().solveMethod(overrider.getName(), parameterTypes);
							if (solvedMethod.isSolved())
							{
								return solvedMethod.getCorrespondingDeclaration();
							}
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
			String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(varDecl, m_typeSolver).toNameHierarchy();		
			VariableDeclaratorId varDeclId = varDecl.getId();
			
			JavaIndexer.recordSymbol(
				m_callbackId, qualifiedName, SymbolType.FIELD, 
				varDeclId.getBeginLine(), varDeclId.getBeginColumn(), varDeclId.getEndLine(), varDeclId.getEndColumn(),
				AccessKind.fromAccessSpecifier(ModifierSet.getAccessSpecifier(n.getModifiers())), false
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
			String qualifiedName = m_filePath + "<" + identifier.getBeginLine() + ":" + identifier.getBeginColumn() + ">";
			
			JavaIndexer.recordLocalSymbol(
				m_callbackId, qualifiedName, 
				identifier.getBeginLine(), identifier.getBeginColumn(), identifier.getEndLine(), identifier.getEndColumn()
			);
			
		}
		
		// don't change the context here.
		super.visit(n, v);
	}
	
	@Override public void visit(final Parameter n, final Void v)
	{
		VariableDeclaratorId identifier = n.getId();
		String qualifiedName = m_filePath + "<" + identifier.getBeginLine() + ":" + identifier.getBeginColumn() + ">";
		
		JavaIndexer.recordLocalSymbol(
			m_callbackId, qualifiedName, 
			identifier.getBeginLine(), identifier.getBeginColumn(), identifier.getEndLine(), identifier.getEndColumn()
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
			String importedName = JavaDeclNameResolver.getQualifiedName(nameExpr).toNameHierarchy();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.IMPORT, 
					importedName, context.getName(), 
					nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn()
				);
			}
		}
		else
		{
			try
			{
				NameExpr nameExpr = n.getName();
				
				JavaDeclName importedDeclName = null;
				
				SymbolReference<TypeDeclaration> symbolReference = m_typeSolver.tryToSolveType(
					JavaDeclNameResolver.getQualifiedName(nameExpr).toString()
				);
				if (symbolReference.isSolved())
				{
					importedDeclName = JavaDeclNameResolver.getQualifiedDeclName(symbolReference.getCorrespondingDeclaration(), m_typeSolver);
				}
	
				if (importedDeclName != null)
				{
					String importedName = importedDeclName.toNameHierarchy();
					for (DeclContext context: m_context)
					{
						JavaIndexer.recordReference(
							m_callbackId, ReferenceKind.IMPORT, 
							importedName, context.getName(), 
							nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn()
						);
					}
				}
			}
			catch (Exception e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
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
				String referencedName = JavaTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toNameHierarchy();

				int beginLine = n.getBeginLine();
				int beginColumn = n.getBeginColumn();
				int endLine = n.getBeginLine();
				int endColumn = n.getBeginColumn() + n.getName().length() - 1;
				
				if (n.getScope() != null)
				{
					endLine = n.getScope().getEndLine();
					endColumn = n.getScope().getEndColumn() + n.getName().length() + 1; // +1 for separator
				}
				
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					beginLine, beginColumn, endLine, endColumn
				);
			}
		}
		catch (Exception e)
		{
			if (m_verbose)
			{
				System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
			}
		}
		super.visit(n, v);
	}
	
	@Override public void visit(final PrimitiveType n, final Void v)
	{
		try
		{
			String referencedName = JavaTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toNameHierarchy();
			
			JavaIndexer.recordSymbolWithoutLocation(
				m_callbackId, referencedName, SymbolType.BUILTIN_TYPE,
				AccessKind.NONE, true
			);
			
			for (DeclContext context: m_context)
			{		
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn()
				);
			}
		}
		catch (Exception e)
		{
			if (m_verbose)
			{
				System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
			}
		}
		
		super.visit(n, v);
	}
	
	@Override public void visit(final VoidType n, final Void v)
	{
		try
		{
			String referencedName = JavaTypeNameResolver.getQualifiedTypeName(n, m_typeSolver).toNameHierarchy();
			
			JavaIndexer.recordSymbolWithoutLocation(
				m_callbackId, referencedName, SymbolType.BUILTIN_TYPE,
				AccessKind.NONE, true
			);
			
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, getTypeReferenceKind(), referencedName, context.getName(), 
					n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn()
				);
			}
		}
		catch (Exception e)
		{
			if (m_verbose)
			{
				System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
			}
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
			if (m_verbose)
			{
				System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
			}
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
						String qualifiedName = m_filePath + "<" + identifier.getBeginLine() + ":" + identifier.getBeginColumn() + ">";
						
						JavaIndexer.recordLocalSymbol(
							m_callbackId, qualifiedName,
							e.getBeginLine(), e.getBeginColumn(), e.getEndLine(), e.getEndColumn()
						);
					}
					else if (wrappedNode instanceof VariableDeclarator)
					{
						if (getFieldDeclarationInParentHierarchy(wrappedNode) != null)
						{
							String qualifiedName = JavaDeclNameResolver.getQualifiedDeclName((VariableDeclarator)wrappedNode, m_typeSolver).toNameHierarchy();
							
							for (DeclContext context: m_context)
							{
								JavaIndexer.recordReference(
									m_callbackId, ReferenceKind.USAGE, qualifiedName, context.getName(),
									e.getBeginLine(), e.getBeginColumn(), e.getEndLine(), e.getEndColumn()
								);
							}
						}
						else
						{
							VariableDeclaratorId identifier = ((VariableDeclarator)wrappedNode).getId();
							String qualifiedName = m_filePath + "<" + identifier.getBeginLine() + ":" + identifier.getBeginColumn() + ">";
							
							JavaIndexer.recordLocalSymbol(
								m_callbackId, qualifiedName,
								e.getBeginLine(), e.getBeginColumn(), e.getEndLine(), e.getEndColumn()
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
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
			}
			catch (MethodAmbiguityException e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
			}
			catch(StackOverflowError e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
	        }
			catch (Exception e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
			}
		}
		
		if (!qualifiedName.isEmpty())
		{
			NameExpr nameExpr = n.getNameExpr();
			for (DeclContext context: m_context)
			{
				JavaIndexer.recordReference(
					m_callbackId, ReferenceKind.CALL, qualifiedName, context.getName(),
					nameExpr.getBeginLine(), nameExpr.getBeginColumn(), nameExpr.getEndLine(), nameExpr.getEndColumn()
				);
			}
		}
		
		super.visit(n, v);
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
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
			}
			catch (MethodAmbiguityException e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
			}
			catch(StackOverflowError e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
				}
	        }
			catch (Exception e)
			{
				if (m_verbose)
				{
					System.out.println(e + " at location " + n.getBeginLine() + ", " + n.getBeginColumn());
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
					type.getBeginLine(), type.getBeginColumn(), type.getEndLine(), type.getEndColumn()
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
			qualifiedName = JavaDeclNameResolver.getQualifiedDeclName(wrappedNode, m_typeSolver).toNameHierarchy();
		}
		else // todo: move this implementation somewhere else
		{
			qualifiedName = solvedMethod.declaringType().getQualifiedName();
			qualifiedName = qualifiedName.replace(".", "\t\r\n");
			
			qualifiedName += "\t\r\n" + solvedMethod.getName() + "\t";
			
			String returnType = solvedMethod.returnType().describe();
			qualifiedName += returnType;
		//	qualifiedName += returnType.substring(returnType.lastIndexOf(".") + 1);
			
			qualifiedName += "\r(";
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
		recordScope(n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn());
		super.visit(n, v);
	}
	
	@Override public void visit(final ArrayInitializerExpr n, final Void v)
	{
		recordScope(n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn());
		super.visit(n, v);
	}
	
	@Override public void visit(final SwitchStmt n, final Void v)
	{
		FileContent.Location scopeStartLocation = m_fileContent.find("{", n.getBeginLine(), n.getBeginColumn());
		recordScope(scopeStartLocation.line, scopeStartLocation.column, n.getEndLine(), n.getEndColumn());
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
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn()
		);
		super.visit(n, v);
	}

	@Override public void visit(final BlockComment n, final Void v) 
	{
		JavaIndexer.recordComment(
			m_callbackId,
			n.getBeginLine(), n.getBeginColumn(), n.getEndLine(), n.getEndColumn()
		);
		super.visit(n, v);
	}
}