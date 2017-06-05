#ifndef QT_HIGHLIGHTER_H
#define QT_HIGHLIGHTER_H

#include <QTextCharFormat>

#include "settings/LanguageType.h"

class QTextBlock;
class QTextDocument;

class QtHighlighter
{
public:
	static void createHighlightingRules();
	static void clearHighlightingRules();

	QtHighlighter(QTextDocument *parent, LanguageType language);
	void highlightDocument();
	void highlightRange(int startLine, int endLine, std::vector<std::pair<int, int>> ranges);

	void rehighlightLines(const std::vector<int>& lines);

	void applyFormat(int startPosition, int endPosition, const QTextCharFormat& format);
	QTextCharFormat getFormat(int startPosition, int endPosition) const;

private:
	struct HighlightingRule
	{
		HighlightingRule();
		HighlightingRule(const QColor& color, const QRegExp& regExp);

		QRegExp pattern;
		QTextCharFormat format;
	};

	void highlightMultiLineComments(std::vector<std::pair<int, int>>* ranges);

	bool isInRange(int index, const std::vector<std::pair<int, int>>& ranges) const;
	void formatBlock(const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges, bool saveRange);

	QTextDocument* document() const;

	static QVector<HighlightingRule> s_highlightingRules;
	static QVector<HighlightingRule> s_highlightingRulesCpp;
	static QVector<HighlightingRule> s_highlightingRulesJava;
	static HighlightingRule s_quotationRule;
	static HighlightingRule s_commentRule;
	static QTextCharFormat s_textFormat;

	QTextDocument* m_document;

	LanguageType m_language;

	QVector<HighlightingRule> m_highlightingRules;
	std::vector<std::pair<int, int>> m_ranges;
	std::vector<bool> m_highlightedLines;
};

#endif // QT_HIGHLIGHTER_H
