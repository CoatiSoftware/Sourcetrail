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
class QtCodeNavigator;
class TokenLocationFile;

class QtCodeSnippet
	: public QFrame
{
	Q_OBJECT

public:
	static std::shared_ptr<QtCodeSnippet> merged(
		QtCodeSnippet* a, QtCodeSnippet* b, QtCodeNavigator* navigator, QtCodeFile* file);

	QtCodeSnippet(const CodeSnippetParams& params, QtCodeNavigator* navigator, QtCodeFile* file);
	virtual ~QtCodeSnippet();

	QtCodeFile* getFile() const;
	QtCodeArea* getArea() const;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	int lineNumberDigits() const;

	void updateLineNumberAreaWidthForDigits(int digits);
	void updateContent();

	void setIsActiveFile(bool isActiveFile);

	uint getLineNumberForLocationId(Id locationId) const;
	uint getStartLineNumberOfFirstActiveLocationOfTokenId(Id tokenId) const;
	QRectF getLineRectForLineNumber(uint lineNumber) const;

	std::string getCode() const;

private slots:
	void clickedTitle();
	void clickedFooter();

private:
	QPushButton* createScopeLine(QBoxLayout* layout);
	void updateDots();

	QtCodeNavigator* m_navigator;
	QtCodeFile* m_file;

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
