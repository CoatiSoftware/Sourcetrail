#ifndef QT_CODE_NAVIGATEABLE_H
#define QT_CODE_NAVIGATEABLE_H

#include <set>

#include "CodeFocusHandler.h"
#include "../../../../lib/component/view/helper/CodeScrollParams.h"
#include "../../../../lib/component/view/helper/CodeSnippetParams.h"
#include "../../../../lib/utility/types.h"

class FilePath;
class QRectF;
class QAbstractScrollArea;
class QRect;
class QtCodeArea;
class QWidget;

class QtCodeNavigateable
{
public:
	virtual ~QtCodeNavigateable();

	virtual QAbstractScrollArea* getScrollArea() = 0;

	virtual void updateSourceLocations(const CodeSnippetParams& params) = 0;
	virtual void updateFiles() = 0;

	virtual void scrollTo(
		const FilePath& filePath,
		size_t lineNumber,
		Id locationId,
		Id scopeLocationId,
		bool animated,
		CodeScrollParams::Target target,
		bool focusTarget) = 0;

	virtual void onWindowFocus() = 0;

	virtual void findScreenMatches(
		const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches) = 0;

	virtual void setFocus(Id locationId) = 0;
	virtual void setFocusOnTop() = 0;
	virtual void moveFocus(
		const CodeFocusHandler::Focus& focus, CodeFocusHandler::Direction direction) = 0;

	virtual void copySelection() = 0;

protected:
	void ensureWidgetVisibleAnimated(
		const QWidget* parentWidget,
		const QWidget* childWidget,
		const QRectF& rect,
		bool animated,
		CodeScrollParams::Target target);
	void ensurePercentVisibleAnimated(
		double percentA, double percentB, bool animated, CodeScrollParams::Target target);

	QRect getFocusRectForWidget(const QWidget* childWidget, const QWidget* parentWidget) const;
};

#endif	  // QT_CODE_NAVIGATEABLE_H
