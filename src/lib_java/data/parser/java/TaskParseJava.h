#ifndef TASK_PARSE_JAVA_H
#define TASK_PARSE_JAVA_H

#include "data/parser/TaskParse.h"

class TaskParseJava
	: public TaskParse
{
public:
	TaskParseJava(
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<FileRegister> fileRegister,
		const Parser::Arguments& arguments,
		DialogView* dialogView
	);

private:
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
};

#endif // TASK_PARSE_JAVA_H
