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
	QtHighlighter(QTextDocument *parent = 0);

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

	void highlightDocument();
	void highlightMultiLineComments(std::vector<std::pair<int, int>>* ranges);

	void addHighlightingRule(const QColor& color, const QRegExp& regExp);

	bool isInRange(int index, const std::vector<std::pair<int, int>>& ranges) const;
	void formatBlock(const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges, bool saveRange);
	void applyFormat(int startPosition, int endPosition, const QTextCharFormat& format);

	QVector<HighlightingRule> m_highlightingRules;
	HighlightingRule m_quotationRule;
	HighlightingRule m_commentRule;
};

#endif // QT_HIGHLIGHTER_H
