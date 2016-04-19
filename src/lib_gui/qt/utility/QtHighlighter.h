#ifndef QT_HIGHLIGHTER_H
#define QT_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;

class QtHighlighter
	: public QSyntaxHighlighter
{
	Q_OBJECT

public:
	static void createHighlightingRules();
	static void clearHighlightingRules();

	QtHighlighter(QTextDocument *parent = 0);
	void highlightDocument();

	void applyFormat(int startPosition, int endPosition, const QTextCharFormat& format);
	QTextCharFormat getFormat(int startPosition, int endPosition) const;

protected:
	void highlightBlock(const QString& text);

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

	static QVector<HighlightingRule> s_highlightingRules;
	static HighlightingRule s_quotationRule;
	static HighlightingRule s_commentRule;
	static QTextCharFormat s_textFormat;
};

#endif // QT_HIGHLIGHTER_H
