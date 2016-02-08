#ifndef MESSAGE_NEW_PROJECT_H
#define MESSAGE_NEW_PROJECT_H

#include "utility/messaging/Message.h"

class MessageNewProject : public Message<MessageNewProject>
{
public:
	MessageNewProject(const std::string& projectName, const std::string projectLocation,
		const std::vector<std::string>& sourceFiles, const std::vector<std::string>& includePaths)
		: projectName(projectName)
		, projectLocation(projectLocation)
		, projectSourceFiles(sourceFiles)
		, projectIncludePaths(includePaths)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageNewProject";
	}

	virtual void print(std::ostream& os) const
	{
	}

	const std::string projectName;
	const std::string projectLocation;
	const std::vector<std::string> projectSourceFiles;
	const std::vector<std::string> projectIncludePaths;
};

#endif // MESSAGE_NEW_PROJECT_H