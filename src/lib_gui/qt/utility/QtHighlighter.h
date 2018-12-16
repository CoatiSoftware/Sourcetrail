#ifndef QT_HIGHLIGHTER_H
#define QT_HIGHLIGHTER_H

#include <QTextCharFormat>

class QTextBlock;
class QTextDocument;

class QtHighlighter
{
public:
	enum class HighlightType
	{
		COMMENT,
		DIRECTIVE,
		FUNCTION,
		KEYWORD,
		NUMBER,
		QUOTATION,
		TEXT,
		TYPE
	};

	static std::string highlightTypeToString(HighlightType type);
	static HighlightType highlightTypeFromString(const std::string typeStr);

	static void loadHighlightingRules();
	static void clearHighlightingRules();

	QtHighlighter(QTextDocument *parent, const std::wstring& language);
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
		HighlightingRule(HighlightType type, const QColor& color, const QRegExp& regExp, bool priority, bool multiLine = false);

		HighlightType type = HighlightType::TEXT;
		QRegExp pattern;
		QTextCharFormat format;
		bool priority = false;
		bool multiLine = false;
	};

	void createRanges(QTextDocument* doc, const std::vector<HighlightingRule>& quotationRules);
	std::vector<std::pair<int, int>> createMultiLineCommentRanges(
		QTextDocument* doc, std::vector<std::pair<int, int>>* ranges);

	bool isInRange(int index, const std::vector<std::pair<int, int>>& ranges) const;
	std::vector<std::pair<int, int>> getRangesForRule(const QTextBlock& block, const HighlightingRule& rule) const;

	void formatBlockForRule(
		const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges = nullptr);
	void formatBlockIfInRange(
		const QTextBlock& block, const QTextCharFormat& format, std::vector<std::pair<int, int>>* ranges);

	QTextDocument* document() const;

	static std::map<std::wstring, std::vector<HighlightingRule>> s_highlightingRules;
	static QTextCharFormat s_textFormat;

	QTextDocument* m_document;

	std::vector<HighlightingRule> m_highlightingRules;
	std::vector<std::pair<int, int>> m_quotationRanges;
	std::vector<std::pair<int, int>> m_multiLineCommentRanges;
	std::vector<bool> m_highlightedLines;
};

#endif // QT_HIGHLIGHTER_H
