package com.sourcetrail;

import com.sourcetrail.name.NameHierarchy;

public abstract class AstVisitorClient
{
	public abstract boolean getInterrupted();

	public abstract void logInfo(String info);

	public abstract void logWarning(String warning);

	public abstract void logError(String error);

	public abstract void recordSymbol(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		AccessKind access,
		DefinitionKind definitionKind);

	public abstract void recordSymbolWithLocation(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		Range range,
		AccessKind access,
		DefinitionKind definitionKind);

	public abstract void recordSymbolWithLocationAndScope(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		Range range,
		Range scopeRange,
		AccessKind access,
		DefinitionKind definitionKind);

	public abstract void recordSymbolWithLocationAndScopeAndSignature(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		Range range,
		Range scopeRange,
		Range signatureRange,
		AccessKind access,
		DefinitionKind definitionKind);

	public abstract void recordReference(
		ReferenceKind referenceKind,
		NameHierarchy referencedName,
		NameHierarchy contextName,
		Range range);

	public abstract void recordQualifierLocation(NameHierarchy qualifierName, Range range);

	public abstract void recordLocalSymbol(NameHierarchy symbolName, Range range);

	public abstract void recordComment(Range range);

	public abstract void recordError(String message, boolean fatal, boolean indexed, Range range);
}
