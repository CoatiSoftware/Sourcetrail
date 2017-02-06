#include "qt/utility/QtHighlighter.h"

#include <QTextCursor>
#include <QTextDocument>

#include "settings/ColorScheme.h"
#include "Application.h"

QVector<QtHighlighter::HighlightingRule> QtHighlighter::s_highlightingRules;
QtHighlighter::HighlightingRule QtHighlighter::s_quotationRule;
QtHighlighter::HighlightingRule QtHighlighter::s_commentRule;
QTextCharFormat QtHighlighter::s_textFormat;

void QtHighlighter::createHighlightingRules()
{
	Project* project = Application::getInstance()->getCurrentProject().get();
	LanguageType language = LANGUAGE_UNKNOWN;
	if (project)
	{
		language = project->getLanguage();
	}

	QStringList keywordPatterns;
	if (language == LANGUAGE_JAVA)
	{
		keywordPatterns
			<< "abstract" << "assert" << "break" << "case" << "catch" << "const" << "continue" << "default"
			<< "do" << "else" << "extends" << "false" << "final" << "finally" << "for"
			<< "friend" << "goto" << "if" << "implements" << "import" << "instanceof" << "interface" << "native"
			<< "new" << "package" << "private" << "protected" << "public" << "return" << "static" << "strictfp"
			<< "super" << "switch" << "synchronized" << "this" << "true" << "throw" << "throws" << "transient"
			<< "try" << "volatile" << "while";
	}
	else
	{
		keywordPatterns
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
	}

	QStringList typePatterns;
	if (language == LANGUAGE_JAVA)
	{
		typePatterns
			<< "boolean" << "byte" << "char" << "class" << "double" << "enum" << "float" << "int" << "long" << "package"
			<< "short" << "void";
	}
	else
	{
		typePatterns
			<< "bool" << "char" << "char16_t" << "char32_t" << "class" << "double" << "enum" << "float" << "int"
			<< "long" << "namespace" << "short" << "signed" << "size_t" << "struct" << "union" << "unsigned" << "void"
			<< "wchar_t";
	}

	QRegExp directiveRegExp = QRegExp("#[a-z]+\\b");
	QRegExp numberRegExp = QRegExp("\\b[0-9]+\\b");
	QRegExp functionRegExp = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	QRegExp quotationRegExp = QRegExp("\"([^\"]|\\\\.)*\"");
	QRegExp quotation2RegExp = QRegExp(" <[^<>\\s]*>$");
	QRegExp commentRegExp = QRegExp("//[^\n]*");

	ColorScheme* scheme = ColorScheme::getInstance().get();

	s_textFormat.setForeground(QColor(scheme->getSyntaxColor("normal").c_str()));

	QColor directiveColor(scheme->getSyntaxColor("directive").c_str());
	QColor keywordColor(scheme->getSyntaxColor("keyword").c_str());
	QColor typeColor(scheme->getSyntaxColor("type").c_str());
	QColor numberColor(scheme->getSyntaxColor("number").c_str());
	QColor functionColor(scheme->getSyntaxColor("function").c_str());
	QColor quotationColor(scheme->getSyntaxColor("quotation").c_str());
	QColor commentColor = scheme->getSyntaxColor("comment").c_str();

	s_highlightingRules.clear();

	foreach (const QString &pattern, keywordPatterns)
	{
		s_highlightingRules.append(HighlightingRule(keywordColor, QRegExp("\\b" + pattern + "\\b")));
	}

	foreach (const QString &pattern, typePatterns)
	{
		s_highlightingRules.append(HighlightingRule(typeColor, QRegExp("\\b" + pattern + "\\b")));
	}

	s_highlightingRules.append(HighlightingRule(directiveColor, directiveRegExp));
	s_highlightingRules.append(HighlightingRule(numberColor, numberRegExp));
	s_highlightingRules.append(HighlightingRule(functionColor, functionRegExp));
	s_highlightingRules.append(HighlightingRule(quotationColor, quotation2RegExp));

	s_quotationRule = HighlightingRule(quotationColor, quotationRegExp);
	s_commentRule = HighlightingRule(commentColor, commentRegExp);
}

void QtHighlighter::clearHighlightingRules()
{
	s_highlightingRules.clear();
}

QtHighlighter::QtHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
}

void QtHighlighter::highlightBlock(const QString& text)
{
}

void QtHighlighter::highlightDocument()
{
	if (!s_highlightingRules.size())
	{
		createHighlightingRules();
	}

	QTextDocument* doc = document();

	int docStart = 0;
	int docEnd = 0;
	for (int i = 0; i < doc->blockCount(); i++)
	{
		docEnd += doc->findBlockByLineNumber(i).length();
	}
	docEnd -= 1;
	applyFormat(docStart, docEnd, s_textFormat);

	std::vector<std::pair<int, int>> ranges;

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		formatBlock(it, s_quotationRule, &ranges, true);
	}

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		foreach (const HighlightingRule &rule, s_highlightingRules)
		{
			formatBlock(it, rule, &ranges, false);
		}
	}

	highlightMultiLineComments(&ranges);

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		formatBlock(it, s_commentRule, &ranges, true);
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

		applyFormat(cursorStart.selectionStart(), cursorEnd.position(), s_commentRule.format);
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

QTextCharFormat QtHighlighter::getFormat(int startPosition, int endPosition) const
{
	QTextCursor cursor(document());
	cursor.setPosition(endPosition);
	return cursor.charFormat();
}
