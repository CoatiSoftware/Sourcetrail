#ifndef TASK_PARSE_JAVA_H
#define TASK_PARSE_JAVA_H

#include <mutex>

#include "data/parser/Parser.h"
#include "utility/scheduling/Task.h"

class FileRegister;
class PersistentStorage;

class TaskParseJava
	: public Task
{
public:
	TaskParseJava(
		PersistentStorage* storage,
		std::shared_ptr<std::mutex> storageMutex,
		std::shared_ptr<FileRegister> fileRegister,
		const Parser::Arguments& arguments
	);

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	PersistentStorage* m_storage;
	std::shared_ptr<std::mutex> m_storageMutex;
	std::shared_ptr<FileRegister> m_fileRegister;
	Parser::Arguments m_arguments;
};

#endif // TASK_PARSE_JAVA_H
