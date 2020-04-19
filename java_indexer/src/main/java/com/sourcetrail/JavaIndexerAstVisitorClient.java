package com.sourcetrail;

import com.sourcetrail.name.NameElement;
import com.sourcetrail.name.NameHierarchy;

public class JavaIndexerAstVisitorClient extends AstVisitorClient
{
	private int m_address;
	private String m_javaLangPackageName;
	private boolean m_javaLangPackageRecorded;

	public JavaIndexerAstVisitorClient(int address)
	{
		m_address = address;

		NameHierarchy javaLangPackageNameHierarchy = new NameHierarchy();
		javaLangPackageNameHierarchy.push(new NameElement("java"));
		javaLangPackageNameHierarchy.push(new NameElement("lang"));
		m_javaLangPackageName = javaLangPackageNameHierarchy.serialize();
		m_javaLangPackageRecorded = false;
	}

	@Override public boolean getInterrupted()
	{
		return JavaIndexer.getInterrupted(m_address);
	}

	@Override public void logInfo(String info)
	{
		JavaIndexer.logInfo(m_address, info);
	}

	@Override public void logWarning(String warning)
	{
		JavaIndexer.logWarning(m_address, warning);
	}

	@Override public void logError(String error)
	{
		JavaIndexer.logError(m_address, error);
	}

	@Override
	public void recordSymbol(
		NameHierarchy symbolName, SymbolKind symbolKind, AccessKind access, DefinitionKind definitionKind)
	{
		JavaIndexer.recordSymbol(
			m_address,
			symbolName.serialize(),
			symbolKind.getValue(),
			access.getValue(),
			definitionKind.getValue());
	}

	@Override
	public void recordSymbolWithLocation(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		Range range,
		AccessKind access,
		DefinitionKind definitionKind)
	{
		JavaIndexer.recordSymbolWithLocation(
			m_address,
			symbolName.serialize(),
			symbolKind.getValue(),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column,
			access.getValue(),
			definitionKind.getValue());
	}

	@Override
	public void recordSymbolWithLocationAndScope(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		Range range,
		Range scopeRange,
		AccessKind access,
		DefinitionKind definitionKind)
	{
		JavaIndexer.recordSymbolWithLocationAndScope(
			m_address,
			symbolName.serialize(),
			symbolKind.getValue(),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column,
			scopeRange.begin.line,
			scopeRange.begin.column,
			scopeRange.end.line,
			scopeRange.end.column,
			access.getValue(),
			definitionKind.getValue());
	}

	@Override
	public void recordSymbolWithLocationAndScopeAndSignature(
		NameHierarchy symbolName,
		SymbolKind symbolKind,
		Range range,
		Range scopeRange,
		Range signatureRange,
		AccessKind access,
		DefinitionKind definitionKind)
	{
		JavaIndexer.recordSymbolWithLocationAndScopeAndSignature(
			m_address,
			symbolName.serialize(),
			symbolKind.getValue(),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column,
			scopeRange.begin.line,
			scopeRange.begin.column,
			scopeRange.end.line,
			scopeRange.end.column,
			signatureRange.begin.line,
			signatureRange.begin.column,
			signatureRange.end.line,
			signatureRange.end.column,
			access.getValue(),
			definitionKind.getValue());
	}

	@Override
	public void recordReference(
		ReferenceKind referenceKind, NameHierarchy referencedName, NameHierarchy contextName, Range range)
	{
		String serializedReferencedName = referencedName.serialize();
		if (!m_javaLangPackageRecorded && serializedReferencedName.startsWith(m_javaLangPackageName))
		{
			JavaIndexer.recordSymbol(
				m_address,
				m_javaLangPackageName,
				SymbolKind.PACKAGE.getValue(),
				AccessKind.NONE.getValue(),
				DefinitionKind.NONE.getValue());

			m_javaLangPackageRecorded = true;
		}

		JavaIndexer.recordReference(
			m_address,
			referenceKind.getValue(),
			serializedReferencedName,
			contextName.serialize(),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column);
	}

	@Override public void recordQualifierLocation(NameHierarchy qualifierName, Range range)
	{
		JavaIndexer.recordQualifierLocation(
			m_address,
			qualifierName.serialize(),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column);
	}

	@Override public void recordLocalSymbol(NameHierarchy symbolName, Range range)
	{
		JavaIndexer.recordLocalSymbol(
			m_address,
			symbolName.serialize(),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column);
	}

	@Override public void recordComment(Range range)
	{
		JavaIndexer.recordComment(
			m_address, range.begin.line, range.begin.column, range.end.line, range.end.column);
	}

	@Override public void recordError(String message, boolean fatal, boolean indexed, Range range)
	{
		JavaIndexer.recordError(
			m_address,
			message,
			(fatal ? 1 : 0),
			(indexed ? 1 : 0),
			range.begin.line,
			range.begin.column,
			range.end.line,
			range.end.column);
	}
}
