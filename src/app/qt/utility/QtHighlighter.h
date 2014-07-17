#ifndef QT_HIGHLIGHTER_H
#define QT_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;

class QtHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	QtHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};

	void addHighlightingRule(const QColor& color, const QRegExp& regExp);

	QVector<HighlightingRule> highlightingRules;
};

#endif // QT_HIGHLIGHTER_H
