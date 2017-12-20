#ifndef CODE_VIEW_H
#define CODE_VIEW_H

#include <memory>

#include "data/ErrorInfo.h"

#include "component/controller/helper/ScreenSearchInterfaces.h"
#include "component/view/helper/CodeSnippetParams.h"
#include "component/view/View.h"

class CodeController;
class FilePath;
class SourceLocationCollection;

class CodeView
	: public View
	, public ScreenSearchResponder
{
public:
	static const char* VIEW_NAME;

	enum FileState
	{
		FILE_MINIMIZED,
		FILE_SNIPPETS,
		FILE_MAXIMIZED
	};

	struct CodeParams
	{
		CodeParams()
			: clearSnippets(false)
			, showContents(false)
		{}

		bool clearSnippets;
		bool showContents;
		std::vector<Id> activeTokenIds;
		std::vector<ErrorInfo> errorInfos;
	};

	struct ScrollParams
	{
		enum ScrollType
		{
			SCROLL_NONE,
			SCROLL_TO_DEFINITION,
			SCROLL_TO_LINE,
			SCROLL_TO_VALUE
		} type;

		ScrollParams(ScrollType type = SCROLL_NONE)
			: type(type)
			, line(0)
			, value(0)
			, animated(false)
			, ignoreActiveReference(false)
			, inListMode(false)
		{}

		FilePath filePath;
		size_t line;

		size_t value;

		bool animated;
		bool ignoreActiveReference;
		bool inListMode;
	};

	CodeView(ViewLayout* viewLayout);
	virtual ~CodeView();

	virtual std::string getName() const;

	virtual void clear() = 0;

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const CodeParams params) = 0;
	virtual void scrollTo(const ScrollParams params) = 0;

	virtual bool showsErrors() const = 0;

	virtual void setFileState(const FilePath filePath, FileState state) = 0;

	virtual void showActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<SourceLocationCollection> collection, bool scrollTo) = 0;
	virtual void showActiveTokenIds(const std::vector<Id>& activeTokenIds) = 0;
	virtual void showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds) = 0;

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds) = 0;
	virtual void defocusTokenIds() = 0;

	virtual void showContents() = 0;

	virtual bool isInListMode() const = 0;
	virtual void setMode(bool listMode) = 0;

	virtual bool hasSingleFileCached(const FilePath& filePath) const = 0;

private:
	CodeController* getController();
};

#endif // CODE_VIEW_H
