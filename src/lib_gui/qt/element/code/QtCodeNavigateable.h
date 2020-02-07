#ifndef QT_CODE_NAVIGATEABLE_H
#define QT_CODE_NAVIGATEABLE_H

#include <set>

#include "CodeScrollParams.h"
#include "CodeSnippetParams.h"
#include "types.h"

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
		bool animated,
		CodeScrollParams::Target target) = 0;

	virtual void onWindowFocus() = 0;

	virtual void findScreenMatches(
		const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches) = 0;

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
