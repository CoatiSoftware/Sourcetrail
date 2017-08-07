package com.sourcetrail;

import java.util.Optional;

import com.github.javaparser.Range;

public abstract class AstVisitorClient 
{
	public void recordSymbolWithLocation(
		String symbolName, SymbolKind symbolType, 
		Optional<Range> range,
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbolWithLocation(
			symbolName, symbolType, 
			range.orElse(Range.range(0, 0, 0, 0)),
			access, definitionKind
		);
	}
	
	public void recordSymbolWithLocationAndScope(
		String symbolName, SymbolKind symbolType, 
		Optional<Range> range,
		Optional<Range> scopeRange,
		AccessKind access, DefinitionKind definitionKind
	)
	{
		recordSymbolWithLocationAndScope(
			symbolName, symbolType, 
			range.orElse(Range.range(0, 0, 0, 0)),
			scopeRange.orElse(Range.range(0, 0, 0, 0)),
			access, definitionKind
		);
	}
	
	public void recordReference(
		ReferenceKind referenceKind, String referencedName, String contextName, 
		Optional<Range> range
	)
	{
		recordReference(
			referenceKind, referencedName, contextName, 
			range.orElse(Range.range(0, 0, 0, 0))
		);
	}
	
	public void recordLocalSymbol(
		String symbolName,
		Optional<Range> range
	)
	{
		recordLocalSymbol(symbolName, range.orElse(Range.range(0, 0, 0, 0)));
	}
	
	public void recordComment(
		Optional<Range> range
	)
	{
		recordComment(range.orElse(Range.range(0, 0, 0, 0)));
	}
	
	public void recordError(
		String message, boolean fatal, boolean indexed, 
		Optional<Range> range
	)
	{
		recordError(
			message, fatal, indexed, 
			range.orElse(Range.range(0, 0, 0, 0))
		);
	}
	
	public abstract boolean getInterrupted();
	
	public abstract void logInfo(String info);
	
	public abstract void logWarning(String warning);
	
	public abstract void logError(String error);

	public abstract void recordSymbol(
		String symbolName, SymbolKind symbolType, 
		AccessKind access, DefinitionKind definitionKind
	);
	
	public abstract void recordSymbolWithLocation(
		String symbolName, SymbolKind symbolType, 
		Range range,
		AccessKind access, DefinitionKind definitionKind
	);
	
	public abstract void recordSymbolWithLocationAndScope(
		String symbolName, SymbolKind symbolType, 
		Range range,
		Range scopeRange,
		AccessKind access, DefinitionKind definitionKind
	);
	
	public abstract void recordReference(
		ReferenceKind referenceKind, String referencedName, String contextName, 
		Range range
	);
		
	public abstract void recordLocalSymbol(
		String symbolName,
		Range range
	);
	
	public abstract void recordComment(
		Range range
	);
	
	public abstract void recordError(
		String message, boolean fatal, boolean indexed, 
		Range range
	);
}

