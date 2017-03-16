#ifndef MESSAGE_CHANGE_FILE_VIEW_H
#define MESSAGE_CHANGE_FILE_VIEW_H

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageChangeFileView
	: public Message<MessageChangeFileView>
{
public:
	enum FileState
	{
		FILE_MINIMIZED,
		FILE_SNIPPETS,
		FILE_MAXIMIZED,
		FILE_DEFAULT_FOR_MODE
	};

	MessageChangeFileView(
		const FilePath& filePath,
		FileState state,
		bool needsData,
		bool showErrors
	)
		: filePath(filePath)
		, state(state)
		, needsData(needsData)
		, showErrors(showErrors)
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
		case FILE_DEFAULT_FOR_MODE: os << "default"; break;
		}

		if (needsData)
		{
			os << ", needs data";
		}
	}

	FilePath filePath;
	FileState state;

	bool needsData;
	bool showErrors;
};

#endif // MESSAGE_CHANGE_FILE_VIEW_H
