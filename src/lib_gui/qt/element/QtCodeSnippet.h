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
	static std::shared_ptr<QtCodeSnippet> merged(QtCodeSnippet* a, QtCodeSnippet* b, QtCodeFile* file);

	QtCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		Id titleId,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		QtCodeFile* file
	);
	virtual ~QtCodeSnippet();

	QtCodeFile* getFile() const;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	int lineNumberDigits() const;

	void updateLineNumberAreaWidthForDigits(int digits);
	void updateContent();

	bool isActive() const;

	void setIsActiveFile(bool isActiveFile);

	QRectF getFirstActiveLineRect() const;

	std::string getCode() const;

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

private slots:
	void clickedTitle();

private:
	void updateDots();

	Id m_titleId;
	std::string m_titleString;

	QPushButton* m_dots;
	QPushButton* m_title;
	std::shared_ptr<QtCodeArea> m_codeArea;
};

#endif // QT_CODE_SNIPPET_H
