#ifndef QT_CODE_NAVIGATEABLE_H
#define QT_CODE_NAVIGATEABLE_H

#include <set>

#include "utility/types.h"

#include "component/view/helper/CodeSnippetParams.h"

class FilePath;
class QRectF;
class QAbstractScrollArea;
class QRect;
class QtCodeArea;
class QWidget;

class QtCodeNavigateable
{
public:
	enum ScrollTarget
	{
		SCROLL_VISIBLE,
		SCROLL_CENTER,
		SCROLL_TOP
	};

	virtual ~QtCodeNavigateable();

	virtual QAbstractScrollArea* getScrollArea() = 0;

	virtual void addCodeSnippet(const CodeSnippetParams& params) = 0;
	virtual void updateCodeSnippet(const CodeSnippetParams& params) = 0;

	virtual void requestFileContent(const FilePath& filePath) = 0;
	virtual bool requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, ScrollTarget target) = 0;

	virtual void updateFiles() = 0;
	virtual void showContents() = 0;

	virtual void onWindowFocus() = 0;

	virtual void findScreenMatches(const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches) = 0;

	virtual std::vector<std::pair<FilePath, Id>> getLocationIdsForTokenIds(const std::set<Id>& tokenIds) const = 0;

protected:
	void ensureWidgetVisibleAnimated(const QWidget* parentWidget, const QWidget *childWidget, QRectF rect, bool animated, ScrollTarget target);
	void ensurePercentVisibleAnimated(double percentA, double percentB, bool animated, ScrollTarget target);

	QRect getFocusRectForWidget(const QWidget* childWidget, const QWidget* parentWidget) const;
};

#endif // QT_CODE_NAVIGATEABLE_H
