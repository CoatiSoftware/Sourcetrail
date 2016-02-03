#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>
#include <memory>

#include <QFrame>

#include "utility/types.h"

#include "qt/element/QtCodeArea.h"

#include "component/view/helper/CodeSnippetParams.h"

class QBoxLayout;
class QPushButton;
class QtCodeFile;
class TokenLocationFile;

class QtCodeSnippet
	: public QFrame
{
	Q_OBJECT

public:
	static std::shared_ptr<QtCodeSnippet> merged(QtCodeSnippet* a, QtCodeSnippet* b, QtCodeFile* file);

	QtCodeSnippet(const CodeSnippetParams& params, QtCodeFile* file);
	virtual ~QtCodeSnippet();

	QtCodeFile* getFile() const;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	int lineNumberDigits() const;

	void updateLineNumberAreaWidthForDigits(int digits);
	void updateContent();

	bool isActive() const;

	void setIsActiveFile(bool isActiveFile);

	uint getFirstActiveLineNumber() const;
	QRectF getLineRectForLineNumber(uint lineNumber) const;

	std::string getCode() const;

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

private slots:
	void clickedTitle();
	void clickedFooter();

private:
	QPushButton* createScopeLine(QBoxLayout* layout);
	void updateDots();

	Id m_titleId;
	std::string m_titleString;

	Id m_footerId;
	std::string m_footerString;

	std::vector<QPushButton*> m_dots;

	QPushButton* m_title;
	QPushButton* m_footer;
	std::shared_ptr<QtCodeArea> m_codeArea;
};

#endif // QT_CODE_SNIPPET_H
