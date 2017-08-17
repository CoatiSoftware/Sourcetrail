package com.sourcetrail.typesolver;

import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.github.javaparser.symbolsolver.model.declarations.ReferenceTypeDeclaration;
import com.github.javaparser.symbolsolver.model.resolution.SymbolReference;
import com.github.javaparser.symbolsolver.resolution.typesolvers.JavaParserTypeSolver;

public class SynchronizedJavaParserTypeSolver extends JavaParserTypeSolver
{	
	private ReentrantReadWriteLock lock = new ReentrantReadWriteLock();
    private Map<String, SymbolReference<ReferenceTypeDeclaration>> solvedTypes = new HashMap<>();
	
	public SynchronizedJavaParserTypeSolver(File srcDir) 
	{
		super(srcDir);
	}

	@Override
	public SymbolReference<ReferenceTypeDeclaration> tryToSolveType(String name) 
	{
		SymbolReference<ReferenceTypeDeclaration> solvedType = SymbolReference.unsolved(ReferenceTypeDeclaration.class);
		
		{
			lock.readLock().lock();
			if (solvedTypes.containsKey(name))
			{
				solvedType = solvedTypes.get(name);
			}
			lock.readLock().unlock();
		}
		
		if (!solvedType.isSolved())
		{
			solvedType = super.tryToSolveType(name);
			
			if (solvedType.isSolved()) 
			{
				lock.writeLock().lock();
				if (!solvedTypes.containsKey(name))
				{
					solvedTypes.put(name, solvedType);
				}
				lock.writeLock().unlock();
			}
		}
		
		return solvedType;
	}
}
