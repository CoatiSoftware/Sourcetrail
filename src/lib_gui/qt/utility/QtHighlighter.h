#ifndef QT_HIGHLIGHTER_H
#define QT_HIGHLIGHTER_H

#include <QTextCharFormat>

#include "LanguageType.h"

class QTextBlock;
class QTextDocument;

class QtHighlighter
{
public:
	static void createHighlightingRules();
	static void clearHighlightingRules();

	QtHighlighter(QTextDocument *parent, LanguageType language);
	~QtHighlighter() = default;

	void highlightDocument();
	void highlightRange(int startLine, int endLine);

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

	void createRanges(QTextDocument* doc, const HighlightingRule& stringRule, const HighlightingRule& charRule);
	std::vector<std::pair<int, int>> createMultiLineCommentRanges(
		QTextDocument* doc, std::vector<std::pair<int, int>>* ranges);

	bool isInRange(int index, const std::vector<std::pair<int, int>>& ranges) const;
	std::vector<std::pair<int, int>> getRangesForRule(const QTextBlock& block, const HighlightingRule& rule) const;

	void formatBlockForRule(
		const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges = nullptr);
	void formatBlockIfInRange(
		const QTextBlock& block, const QTextCharFormat& format, std::vector<std::pair<int, int>>* ranges);

	QTextDocument* document() const;

	static QVector<HighlightingRule> s_highlightingRules;
	static QVector<HighlightingRule> s_highlightingRulesCpp;
	static QVector<HighlightingRule> s_highlightingRulesJava;
	static HighlightingRule s_stringQuotationRule;
	static HighlightingRule s_charQuotationRule;
	static HighlightingRule s_commentRule;
	static QTextCharFormat s_textFormat;

	QTextDocument* m_document;

	LanguageType m_language;

	QVector<HighlightingRule> m_highlightingRules;
	std::vector<std::pair<int, int>> m_quotationRanges;
	std::vector<std::pair<int, int>> m_multiLineCommentRanges;
	std::vector<bool> m_highlightedLines;
};

#endif // QT_HIGHLIGHTER_H
