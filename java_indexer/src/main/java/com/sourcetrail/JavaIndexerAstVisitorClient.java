package com.sourcetrail;

import com.github.javaparser.Range;

public class JavaIndexerAstVisitorClient extends AstVisitorClient 
{
	private int m_address;

	public JavaIndexerAstVisitorClient(int address)
	{
		m_address = address;
	}
	
	@Override
	public boolean getInterrupted()
	{
		return JavaIndexer.getInterrupted(m_address);
	}
	
	@Override
	public void logInfo(String info)
	{
		JavaIndexer.logInfo(m_address, info);
	}

	@Override
	public void logWarning(String warning)
	{
		JavaIndexer.logWarning(m_address, warning);
	}
	
	@Override
	public void logError(String error)
	{
		JavaIndexer.logError(m_address, error);
	}
	
	@Override
	public void recordSymbol(
		String symbolName, SymbolKind symbolType,
		AccessKind access, DefinitionKind definitionKind) 
	{
		JavaIndexer.recordSymbol(
			m_address, symbolName, symbolType.getValue(),  
			access.getValue(), definitionKind.getValue()
		);
	}

	@Override
	public void recordSymbolWithLocation(
		String symbolName, SymbolKind symbolType, Range range,
		AccessKind access, DefinitionKind definitionKind) 
	{
		JavaIndexer.recordSymbolWithLocation(
			m_address, symbolName, symbolType.getValue(), 
			range.begin.line, range.begin.column, range.end.line, range.end.column, 
			access.getValue(), definitionKind.getValue()
		);
	}

	@Override
	public void recordSymbolWithLocationAndScope(
		String symbolName, SymbolKind symbolType, Range range,
		Range scopeRange, AccessKind access, DefinitionKind definitionKind) 
	{
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_address, symbolName, symbolType.getValue(), 
			range.begin.line, range.begin.column, range.end.line, range.end.column, 
			scopeRange.begin.line, scopeRange.begin.column, scopeRange.end.line, scopeRange.end.column, 
			access.getValue(), definitionKind.getValue()
		);
	}

	@Override
	public void recordReference(
		ReferenceKind referenceKind, String referencedName, 
		String contextName, Range range) 
	{
		JavaIndexer.recordReference(
			m_address, referenceKind.getValue(), referencedName, contextName, 
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}

	@Override
	public void recordLocalSymbol(String symbolName, Range range) 
	{
		JavaIndexer.recordLocalSymbol(
			m_address, symbolName,
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}

	@Override
	public void recordComment(Range range) 
	{
		JavaIndexer.recordComment(
			m_address,
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}

	@Override
	public void recordError(String message, boolean fatal, boolean indexed, Range range) 
	{
		JavaIndexer.recordError(
			m_address, message, (fatal ? 1 : 0), (indexed ? 1 : 0), 
			range.begin.line, range.begin.column, range.end.line, range.end.column
		);
	}

}
