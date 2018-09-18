#include "QtHighlighter.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include "ColorScheme.h"
#include "tracing.h"
#include "utility.h"

QVector<QtHighlighter::HighlightingRule> QtHighlighter::s_highlightingRules;
QVector<QtHighlighter::HighlightingRule> QtHighlighter::s_highlightingRulesCpp;
QVector<QtHighlighter::HighlightingRule> QtHighlighter::s_highlightingRulesJava;
QtHighlighter::HighlightingRule QtHighlighter::s_stringQuotationRule;
QtHighlighter::HighlightingRule QtHighlighter::s_charQuotationRule;
QtHighlighter::HighlightingRule QtHighlighter::s_commentRule;
QTextCharFormat QtHighlighter::s_textFormat;

void QtHighlighter::createHighlightingRules()
{
	QStringList keywordPatternsCpp;
	keywordPatternsCpp
		<< "alignas" << "alignof" << "and" << "and_eq" << "asm" << "assert" << "auto" << "bitand" << "bitor"
		<< "break" << "case" << "catch" << "compl" << "complex" << "const" << "constexpr" << "const_cast"
		<< "continue" << "decltype" << "default" << "delete" << "do" << "dynamic_cast" << "else" << "explicit"
		<< "export" << "extern" << "false" << "final" << "for" << "friend" << "goto" << "if" << "imaginary"
		<< "inline" << "mutable" << "new" << "noexcept" << "not" << "not_eq" << "noreturn" << "NULL" << "nullptr"
		<< "operator" << "or" << "or_eq" << "override" << "private" << "protected" << "public" << "register"
		<< "reinterpret_cast" << "requires" << "return" << "signals" << "sizeof" << "slots" << "static"
		<< "static_assert" << "static_cast" << "switch" << "template" << "this" << "thread_local" << "throw"
		<< "throws" << "true" << "try" << "typedef" << "typeid" << "typename" << "using" << "virtual" << "volatile"
		<< "while" << "xor" << "xor_eq";

	QStringList keywordPatternsJava;
	keywordPatternsJava
		<< "abstract" << "assert" << "break" << "case" << "catch" << "const" << "continue" << "default"
		<< "do" << "else" << "extends" << "false" << "final" << "finally" << "for"
		<< "friend" << "goto" << "if" << "implements" << "import" << "instanceof" << "interface" << "native"
		<< "new" << "package" << "private" << "protected" << "public" << "return" << "static" << "strictfp"
		<< "super" << "switch" << "synchronized" << "this" << "true" << "throw" << "throws" << "transient"
		<< "try" << "volatile" << "while";

	QStringList typePatternsCpp;
	typePatternsCpp
		<< "bool" << "char" << "char16_t" << "char32_t" << "class" << "double" << "enum" << "float" << "int"
		<< "long" << "namespace" << "short" << "signed" << "size_t" << "struct" << "union" << "unsigned" << "void"
		<< "wchar_t";

	QStringList typePatternsJava;
	typePatternsJava
		<< "boolean" << "byte" << "char" << "class" << "double" << "enum" << "float" << "int" << "long" << "package"
		<< "short" << "void";

	QRegExp directiveRegExp = QRegExp("#[a-z]+\\b");
	QRegExp annotationRegExp = QRegExp("@[A-Za-z0-9_]+");
	QRegExp numberRegExp = QRegExp("\\b[0-9]+\\b");
	QRegExp functionRegExp = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	QRegExp stringQuotationRegExp = QRegExp("\"([^\"]|\\\\.)*\"");
	QRegExp charQuotationRegExp = QRegExp("\'[^\']\'");
	QRegExp tagQuotationRegExp = QRegExp(" <[^<>\\s]*>$");
	QRegExp commentRegExp = QRegExp("//[^\n]*");

	ColorScheme* scheme = ColorScheme::getInstance().get();

	s_textFormat.setForeground(QColor(scheme->getSyntaxColor("normal").c_str()));

	QColor directiveColor(scheme->getSyntaxColor("directive").c_str());
	QColor annotationColor = directiveColor;
	QColor keywordColor(scheme->getSyntaxColor("keyword").c_str());
	QColor typeColor(scheme->getSyntaxColor("type").c_str());
	QColor numberColor(scheme->getSyntaxColor("number").c_str());
	QColor functionColor(scheme->getSyntaxColor("function").c_str());
	QColor quotationColor(scheme->getSyntaxColor("quotation").c_str());
	QColor commentColor(scheme->getSyntaxColor("comment").c_str());

	s_highlightingRules.clear();
	s_highlightingRules.append(HighlightingRule(directiveColor, directiveRegExp));
	s_highlightingRules.append(HighlightingRule(numberColor, numberRegExp));
	s_highlightingRules.append(HighlightingRule(functionColor, functionRegExp));
	s_highlightingRules.append(HighlightingRule(annotationColor, annotationRegExp));
	s_highlightingRules.append(HighlightingRule(quotationColor, tagQuotationRegExp));

	s_highlightingRulesCpp.clear();

	foreach (const QString &pattern, keywordPatternsCpp)
	{
		s_highlightingRulesCpp.append(HighlightingRule(keywordColor, QRegExp("\\b" + pattern + "\\b")));
	}

	foreach (const QString &pattern, typePatternsCpp)
	{
		s_highlightingRulesCpp.append(HighlightingRule(typeColor, QRegExp("\\b" + pattern + "\\b")));
	}

	s_highlightingRulesJava.clear();

	foreach (const QString &pattern, keywordPatternsJava)
	{
		s_highlightingRulesJava.append(HighlightingRule(keywordColor, QRegExp("\\b" + pattern + "\\b")));
	}

	foreach (const QString &pattern, typePatternsJava)
	{
		s_highlightingRulesJava.append(HighlightingRule(typeColor, QRegExp("\\b" + pattern + "\\b")));
	}

	s_stringQuotationRule = HighlightingRule(quotationColor, stringQuotationRegExp);
	s_charQuotationRule = HighlightingRule(quotationColor, charQuotationRegExp);
	s_commentRule = HighlightingRule(commentColor, commentRegExp);
}

void QtHighlighter::clearHighlightingRules()
{
	s_highlightingRules.clear();
}

QtHighlighter::QtHighlighter(QTextDocument *document, LanguageType language)
	: m_document(document)
	, m_language(language)
{
	if (!s_highlightingRules.size())
	{
		createHighlightingRules();
	}

	if (m_language == LANGUAGE_UNKNOWN)
	{
		return;
	}

	if (m_language == LANGUAGE_JAVA)
	{
		m_highlightingRules = s_highlightingRulesJava;
	}
	else
	{
		m_highlightingRules = s_highlightingRulesCpp;
	}

	m_highlightingRules.append(s_highlightingRules);
}

void QtHighlighter::highlightDocument()
{
	TRACE();

	QTextDocument* doc = document();

	size_t docStart = 0;
	size_t docEnd = 0;
	for (int i = 0; i < doc->blockCount(); i++)
	{
		docEnd += doc->findBlockByLineNumber(i).length();
	}

	if (docEnd > 0)
	{
		docEnd -= 1;
	}

	applyFormat(docStart, docEnd, s_textFormat);

	m_highlightedLines.clear();
	m_highlightedLines.resize(document()->blockCount(), false);

	if (m_language == LANGUAGE_UNKNOWN)
	{
		return;
	}

	createRanges(doc, s_stringQuotationRule, s_charQuotationRule);
}

void QtHighlighter::highlightRange(int startLine, int endLine)
{
	if (startLine < 0 || endLine < 0 || startLine > endLine || endLine > int(m_highlightedLines.size()))
	{
		return;
	}

	bool hasUnhighlightedLines = false;
	for (int i = startLine; i <= endLine; i++)
	{
		if (!m_highlightedLines[i])
		{
			hasUnhighlightedLines = true;
			break;
		}
	}

	if (!hasUnhighlightedLines)
	{
		return;
	}

	QTextDocument* doc = document();
	QTextBlock start = doc->findBlockByLineNumber(startLine);
	QTextBlock end = doc->findBlockByLineNumber(endLine + 1);

	int index = startLine;
	for (QTextBlock it = start; it != end; it = it.next())
	{
		if (!m_highlightedLines[index])
		{
			applyFormat(it.position(), it.position() + it.length() - 1, s_textFormat);

			if (m_language != LANGUAGE_UNKNOWN)
			{
				foreach (const HighlightingRule &rule, m_highlightingRules)
				{
					formatBlockForRule(it, rule);
				}

				formatBlockIfInRange(it, s_stringQuotationRule.format, &m_quotationRanges);
				formatBlockIfInRange(it, s_charQuotationRule.format, &m_quotationRanges);
				formatBlockForRule(it, s_commentRule, &m_quotationRanges);
				formatBlockIfInRange(it, s_commentRule.format, &m_multiLineCommentRanges);
			}
		}
		index++;
	}

	for (int i = startLine; i <= endLine; i++)
	{
		m_highlightedLines[i] = true;
	}
}

void QtHighlighter::rehighlightLines(const std::vector<int>& lines)
{
	for (int line : lines)
	{
		if (line >= 0 && line < int(m_highlightedLines.size()))
		{
			m_highlightedLines[line] = false;
		}
	}
}

void QtHighlighter::applyFormat(int startPosition, int endPosition, const QTextCharFormat& format)
{
	QTextCursor cursor(document());
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
	cursor.setCharFormat(format);
}

QTextCharFormat QtHighlighter::getFormat(int startPosition, int endPosition) const
{
	QTextCursor cursor(document());
	cursor.setPosition(endPosition);
	return cursor.charFormat();
}

void QtHighlighter::createRanges(
	QTextDocument* doc, const HighlightingRule& stringRule, const HighlightingRule& charRule)
{
	m_quotationRanges.clear();
	m_multiLineCommentRanges.clear();

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		utility::append(m_quotationRanges, getRangesForRule(it, stringRule));
		utility::append(m_quotationRanges, getRangesForRule(it, charRule));
	}

	m_multiLineCommentRanges = createMultiLineCommentRanges(doc, &m_quotationRanges);
}

std::vector<std::pair<int, int>> QtHighlighter::createMultiLineCommentRanges(
	QTextDocument* doc, std::vector<std::pair<int, int>>* ranges)
{
	QRegExp commentStartExpression = QRegExp("(^([^/]|/[^/])*)/\\*");
	QRegExp commentEndExpression = QRegExp("\\*/");

	QTextCursor cursorStart(doc);
	QTextCursor cursorEnd(doc);

	std::vector<std::pair<int, int>> multiLineCommentRanges;

	while (true)
	{
		do
		{
			cursorStart = document()->find(commentStartExpression, cursorStart);
			if (!cursorStart.isNull())
			{
				cursorStart.setPosition(cursorStart.selectionEnd() - 2);
			}
		}
		while (isInRange(cursorStart.selectionEnd(), *ranges));

		if (cursorStart.isNull())
		{
			break;
		}

		cursorEnd = document()->find(commentEndExpression, cursorStart);
		if (cursorEnd.isNull())
		{
			break;
		}

		multiLineCommentRanges.push_back(std::pair<int, int>(cursorStart.position(), cursorEnd.position()));
		cursorStart = cursorEnd;
	}

	return multiLineCommentRanges;
}

QtHighlighter::HighlightingRule::HighlightingRule()
{
}

QtHighlighter::HighlightingRule::HighlightingRule(const QColor& color, const QRegExp& regExp)
{
	format.setForeground(color);
	pattern = regExp;
}

bool QtHighlighter::isInRange(int pos, const std::vector<std::pair<int, int>>& ranges) const
{
	for (const std::pair<int, int> p : ranges)
	{
		if (pos >= p.first && pos <= p.second)
		{
			return true;
		}
	}

	return false;
}

std::vector<std::pair<int, int>> QtHighlighter::getRangesForRule(
	const QTextBlock& block, const HighlightingRule& rule) const
{
	QRegExp expression(rule.pattern);
	int pos = block.position();
	int index = expression.indexIn(block.text());

	std::vector<std::pair<int, int>> ranges;

	while (index >= 0)
	{
		int length = expression.matchedLength();

		ranges.push_back(std::pair<int, int>(pos + index, pos + index + length));

		index = expression.indexIn(block.text(), index + length);
	}

	return ranges;
}

void QtHighlighter::formatBlockForRule(
	const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges
){
	QRegExp expression(rule.pattern);
	int pos = block.position();
	int index = expression.indexIn(block.text());

	while (index >= 0)
	{
		int length = expression.matchedLength();

		if (!ranges || !isInRange(pos + index, *ranges))
		{
			applyFormat(pos + index, pos + index + length, rule.format);
		}

		index = expression.indexIn(block.text(), index + length);
	}
}

void QtHighlighter::formatBlockIfInRange(
	const QTextBlock& block, const QTextCharFormat& format, std::vector<std::pair<int, int>>* ranges
){
	int startPos = block.position();
	int endPos = startPos + block.length() - 1;

	for (auto range : *ranges)
	{
		int start = std::max(range.first, startPos);
		int end = std::min(range.second, endPos);

		if (start <= end)
		{
			applyFormat(start, end, format);
		}
	}
}

QTextDocument* QtHighlighter::document() const
{
	return m_document;
}
