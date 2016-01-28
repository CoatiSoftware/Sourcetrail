#ifndef MESSAGE_CHANGE_FILE_VIEW_H
#define MESSAGE_CHANGE_FILE_VIEW_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/location/TokenLocationFile.h"

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

	MessageChangeFileView(
		const FilePath filePath,
		FileState state,
		bool needsData,
		bool showErrors,
		std::shared_ptr<TokenLocationFile> locationFile
	)
		: filePath(filePath)
		, state(state)
		, needsData(needsData)
		, showErrors(showErrors)
		, locationFile(locationFile)
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

		if (needsData)
		{
			os << ", needs data";
		}
	}

	FilePath filePath;
	FileState state;

	bool needsData;
	bool showErrors;

	std::shared_ptr<TokenLocationFile> locationFile;
};

#endif // MESSAGE_CHANGE_FILE_VIEW_H
