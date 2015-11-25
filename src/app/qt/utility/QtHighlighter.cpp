#include "qt/utility/QtHighlighter.h"

#include <QTextCursor>
#include <QTextDocument>

#include "settings/ColorScheme.h"

QtHighlighter::QtHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	QStringList keywordPatterns;
	keywordPatterns
		<< "break" << "case" << "const" << "continue" << "default"
		<< "delete" << "do" << "else" << "explicit" << "false" << "for"
		<< "friend" << "if" << "inline" << "new" << "NULL" << "nullptr" << "operator"
		<< "private" << "protected" << "public" << "return" << "signals"
		<< "slots" << "static" << "switch" << "template" << "true" << "typedef"
		<< "typename" << "virtual" << "volatile" << "while";

	QStringList typePatterns;
	typePatterns
		<< "bool" << "char" << "class" << "double"
		<< "enum" << "float" << "int" << "long"
		<< "namespace" << "short" << "signed" << "size_t"
		<< "struct" << "union" << "unsigned" << "void";

	QRegExp directiveRegExp = QRegExp("#[a-z]+\\b");
	QRegExp numberRegExp = QRegExp("\\b[0-9]+\\b");
	QRegExp functionRegExp = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	QRegExp quotationRegExp = QRegExp("\"([^\"]|\\\\.)*\"");
	QRegExp quotation2RegExp = QRegExp(" <.*>");
	QRegExp commentRegExp = QRegExp("//[^\n]*");

	ColorScheme* scheme = ColorScheme::getInstance().get();

	QColor directiveColor(scheme->getSyntaxColor("directive").c_str());
	QColor keywordColor(scheme->getSyntaxColor("keyword").c_str());
	QColor typeColor(scheme->getSyntaxColor("type").c_str());
	QColor numberColor(scheme->getSyntaxColor("number").c_str());
	QColor functionColor(scheme->getSyntaxColor("function").c_str());
	QColor quotationColor(scheme->getSyntaxColor("quotation").c_str());
	QColor commentColor = scheme->getSyntaxColor("comment").c_str();

	foreach (const QString &pattern, keywordPatterns)
	{
		addHighlightingRule(keywordColor, QRegExp("\\b" + pattern + "\\b"));
	}

	foreach (const QString &pattern, typePatterns)
	{
		addHighlightingRule(typeColor, QRegExp("\\b" + pattern + "\\b"));
	}

	addHighlightingRule(directiveColor, directiveRegExp);
	addHighlightingRule(numberColor, numberRegExp);
	addHighlightingRule(functionColor, functionRegExp);
	addHighlightingRule(quotationColor, quotation2RegExp);

	m_quotationRule = HighlightingRule(quotationColor, quotationRegExp);
	m_commentRule = HighlightingRule(commentColor, commentRegExp);
}

void QtHighlighter::highlightBlock(const QString& text)
{
	if (currentBlock().blockNumber() == 0)
	{
		highlightDocument();
	}
}

void QtHighlighter::highlightDocument()
{
	QTextDocument* doc = document();

	std::vector<std::pair<int, int>> ranges;

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		formatBlock(it, m_quotationRule, &ranges, true);
	}

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		foreach (const HighlightingRule &rule, m_highlightingRules)
		{
			formatBlock(it, rule, &ranges, false);
		}
	}

	highlightMultiLineComments(&ranges);

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		formatBlock(it, m_commentRule, &ranges, true);
	}
}

void QtHighlighter::highlightMultiLineComments(std::vector<std::pair<int, int>>* ranges)
{
	QTextDocument* doc = document();

	QRegExp commentStartExpression = QRegExp("(^([^/]|/[^/])*)/\\*");
	QRegExp commentEndExpression = QRegExp("\\*/");

	QTextCursor cursorStart(doc);
	QTextCursor cursorEnd(doc);

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
		while (isInRange(cursorStart.position(), *ranges));

		if (cursorStart.isNull())
		{
			break;
		}

		cursorEnd = document()->find(commentEndExpression, cursorStart);
		if (cursorEnd.isNull())
		{
			break;
		}

		applyFormat(cursorStart.selectionStart(), cursorEnd.position(), m_commentRule.format);
		ranges->push_back(std::pair<int, int>(cursorStart.selectionStart(), cursorEnd.position()));

		cursorStart = cursorEnd;
	}
}

QtHighlighter::HighlightingRule::HighlightingRule()
{
}

QtHighlighter::HighlightingRule::HighlightingRule(const QColor& color, const QRegExp& regExp)
{
	format.setForeground(color);
	pattern = regExp;
}

void QtHighlighter::addHighlightingRule(const QColor& color, const QRegExp& regExp)
{
	m_highlightingRules.append(HighlightingRule(color, regExp));
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

void QtHighlighter::formatBlock(
	const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges, bool saveRange
){
	QRegExp expression(rule.pattern);
	int pos = block.position();
	int index = expression.indexIn(block.text());
	std::vector<std::pair<int, int>> newRanges;

	while (index >= 0)
	{
		int length = expression.matchedLength();

		if (!isInRange(pos + index, *ranges))
		{
			applyFormat(pos + index, pos + index + length, rule.format);
		}

		newRanges.push_back(std::pair<int, int>(pos + index, pos + index + length));
		index = expression.indexIn(block.text(), index + length);
	}

	if (saveRange)
	{
		ranges->insert(ranges->end(), newRanges.begin(), newRanges.end());
	}
}

void QtHighlighter::applyFormat(int startPosition, int endPosition, const QTextCharFormat& format)
{
	QTextCursor cursor(document());
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
	cursor.setCharFormat(format);
}
