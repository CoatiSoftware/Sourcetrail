#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>
#include <memory>

#include <QFrame>

#include "utility/types.h"

#include "qt/element/QtCodeArea.h"

class QPushButton;
class QtCodeFile;
class TokenLocationFile;

class QtCodeSnippet
	: public QFrame
{
	Q_OBJECT

public:
	QtCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		QtCodeFile* file
	);
	virtual ~QtCodeSnippet();

	int lineNumberDigits() const;

	void updateLineNumberAreaWidthForDigits(int digits);
	void updateContent();

private:
	void updateDots();

	QPushButton* m_dots;
	QPushButton* m_title;
	std::shared_ptr<QtCodeArea> m_codeArea;
};

#endif // QT_CODE_SNIPPET_H
