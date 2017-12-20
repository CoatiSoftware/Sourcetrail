#ifndef MESSAGE_CHANGE_FILE_VIEW_H
#define MESSAGE_CHANGE_FILE_VIEW_H

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"

class MessageChangeFileView
	: public Message<MessageChangeFileView>
{
public:
	enum FileState
	{
		FILE_MINIMIZED,
		FILE_SNIPPETS,
		FILE_MAXIMIZED
	};

	enum ViewMode
	{
		VIEW_LIST,
		VIEW_SINGLE,
		VIEW_CURRENT
	};

	MessageChangeFileView(
		const FilePath& filePath,
		FileState state,
		ViewMode viewMode,
		bool needsData,
		bool showErrors,
		bool switchesViewMode = false
	)
		: filePath(filePath)
		, state(state)
		, viewMode(viewMode)
		, needsData(needsData)
		, showErrors(showErrors)
		, switchesViewMode(switchesViewMode)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageChangeFileView";
	}

	virtual void print(std::ostream& os) const
	{
		switch (state)
		{
		case FILE_MINIMIZED: os << "minimize"; break;
		case FILE_SNIPPETS: os << "snippets"; break;
		case FILE_MAXIMIZED: os << "maximize"; break;
		}

		switch (viewMode)
		{
		case VIEW_LIST: os << ", list"; break;
		case VIEW_SINGLE: os << ", single"; break;
		case VIEW_CURRENT: os << ", current"; break;
		}

		if (needsData)
		{
			os << ", needs data";
		}
	}

	const FilePath filePath;
	const FileState state;
	const ViewMode viewMode;
	const bool needsData;
	const bool showErrors;
	const bool switchesViewMode;
};

#endif // MESSAGE_CHANGE_FILE_VIEW_H
