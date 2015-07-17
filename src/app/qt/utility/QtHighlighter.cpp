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
	QRegExp commentRegExp = QRegExp("//[^\n]*");
	QRegExp quotationRegExp = QRegExp("\".*\"");
	QRegExp quotation2RegExp = QRegExp("<.*>");
	QRegExp numberRegExp = QRegExp("\\b[0-9]+\\b");
	QRegExp functionRegExp = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");

	ColorScheme* scheme = ColorScheme::getInstance().get();

	QColor directiveColor(scheme->getSyntaxColor("directive").c_str());
	QColor keywordColor(scheme->getSyntaxColor("keyword").c_str());
	QColor typeColor(scheme->getSyntaxColor("type").c_str());
	QColor commentColor(scheme->getSyntaxColor("comment").c_str());
	QColor numberColor(scheme->getSyntaxColor("number").c_str());
	QColor quotationColor(scheme->getSyntaxColor("quotation").c_str());
	QColor functionColor(scheme->getSyntaxColor("function").c_str());

	foreach (const QString &pattern, keywordPatterns)
	{
		addHighlightingRule(keywordColor, QRegExp("\\b" + pattern + "\\b"));
	}

	foreach (const QString &pattern, typePatterns)
	{
		addHighlightingRule(typeColor, QRegExp("\\b" + pattern + "\\b"));
	}

	addHighlightingRule(directiveColor, directiveRegExp);
	addHighlightingRule(commentColor, commentRegExp);
	addHighlightingRule(quotationColor, quotationRegExp);
	addHighlightingRule(quotationColor, quotation2RegExp);
	addHighlightingRule(numberColor, numberRegExp);
	addHighlightingRule(functionColor, functionRegExp);
}

void QtHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);

	QRegExp commentStartExpression = QRegExp("/\\*");
	QRegExp commentEndExpression = QRegExp("\\*/");

	QTextCursor cursorStart = document()->find(commentStartExpression);
	QTextCursor cursorEnd = document()->find(commentEndExpression);

	int startIndex = 0;
	if (!cursorEnd.isNull() && (cursorStart.isNull() || cursorEnd < cursorStart)
		&& currentBlock().blockNumber() <= cursorEnd.blockNumber())
	{
		startIndex = 0;
	}
	else if (previousBlockState() != 1)
	{
		startIndex = commentStartExpression.indexIn(text);
	}

	QTextCharFormat multiLineCommentFormat;
	multiLineCommentFormat.setForeground(Qt::gray);

	while (startIndex >= 0)
	{
		int endIndex = commentEndExpression.indexIn(text, startIndex);
		int commentLength;

		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}

		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}

void QtHighlighter::addHighlightingRule(const QColor& color, const QRegExp& regExp)
{
	HighlightingRule rule;
	rule.format.setForeground(color);
	rule.pattern = regExp;
	highlightingRules.append(rule);
}
