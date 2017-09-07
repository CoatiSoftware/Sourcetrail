package com.sourcetrail;

import java.util.Optional;

import com.sourcetrail.name.NameHierarchy;

public abstract class AstVisitorClient 
{
	public void recordSymbolWithLocation(
			NameHierarchy symbolName, SymbolKind symbolKind, 
			Optional<Range> range,
			AccessKind access, DefinitionKind definitionKind)
	{
		recordSymbolWithLocation(
				symbolName, symbolKind, 
				range.orElse(new Range(0, 0, 0, 0)),
				access, definitionKind);
	}
	
	public void recordSymbolWithLocationAndScope(
			NameHierarchy symbolName, SymbolKind symbolKind, 
			Optional<Range> range,
			Optional<Range> scopeRange,
			AccessKind access, DefinitionKind definitionKind)
	{
		recordSymbolWithLocationAndScope(
			symbolName, symbolKind, 
			range.orElse(new Range(0, 0, 0, 0)),
			scopeRange.orElse(new Range(0, 0, 0, 0)),
			access, definitionKind);
	}
	
	public void recordReference(
			ReferenceKind referenceKind, NameHierarchy referencedName, NameHierarchy contextName, 
			Optional<Range> range)
	{
		recordReference(
				referenceKind, referencedName, contextName, 
				range.orElse(new Range(0, 0, 0, 0)));
	}
	
	public void recordLocalSymbol(
			NameHierarchy symbolName,
			Optional<Range> range)
	{
		recordLocalSymbol(symbolName, range.orElse(new Range(0, 0, 0, 0)));
	}
	
	public void recordComment(
			Optional<Range> range)
	{
		recordComment(range.orElse(new Range(0, 0, 0, 0)));
	}
	
	public void recordError(
		String message, boolean fatal, boolean indexed, 
		Optional<Range> range)
	{
		recordError(
				message, fatal, indexed, 
				range.orElse(new Range(0, 0, 0, 0)));
	}
	
	public abstract boolean getInterrupted();
	
	public abstract void logInfo(String info);
	
	public abstract void logWarning(String warning);
	
	public abstract void logError(String error);

	public abstract void recordSymbol(
			NameHierarchy symbolName, SymbolKind symbolKind, 
			AccessKind access, DefinitionKind definitionKind);
	
	public abstract void recordSymbolWithLocation(
			NameHierarchy symbolName, SymbolKind symbolKind, 
			Range range,
			AccessKind access, DefinitionKind definitionKind);
	
	public abstract void recordSymbolWithLocationAndScope(
			NameHierarchy symbolName, SymbolKind symbolKind, 
			Range range,
			Range scopeRange,
			AccessKind access, DefinitionKind definitionKind);
	
	public abstract void recordReference(
			ReferenceKind referenceKind, NameHierarchy referencedName, NameHierarchy contextName, 
			Range range);
		
	public abstract void recordLocalSymbol(
			NameHierarchy symbolName,
			Range range);
	
	public abstract void recordComment(
			Range range);
	
	public abstract void recordError(
			String message, boolean fatal, boolean indexed, 
			Range range);
}

