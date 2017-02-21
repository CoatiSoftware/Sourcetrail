#include "InterprocessDataManager.h"

#include <boost/interprocess/managed_shared_memory.hpp>

#include "utility/logging/logging.h"
#include "SharedUUIDManager.h"

#include "InterprocessUtility.h"
#include "SharedQueue.h"

std::string InterprocessDataManager::m_sharedArgumentQueueName = "coati_parser_arguments";

InterprocessDataManager::InterprocessDataManager(const bool isOwner)
	: m_isOwner(isOwner)
	, m_initialized(false)
{
}

InterprocessDataManager::~InterprocessDataManager()
{
	SharedUUIDManager::getInstance()->removeUUIDsForInstance(
		SharedUUIDManager::getInstance()->getInstanceUUID());
}

void InterprocessDataManager::initialize()
{
	try
	{
		m_sharedArgumentQueueName = UUIDUtility::getUUIDString() + "_" + m_sharedArgumentQueueName;

		std::string uuid = SharedUUIDManager::getInstance()->getNewUUID();
		m_parserArguments.initialize(m_isOwner, uuid.c_str());

		m_initialized = true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR_STREAM(<< e.what());
	}
}

void InterprocessDataManager::pushParserArguments(const Parser::Arguments& arguments)
{
	IF_INITIALIZED()
	{
		SharedParserArguments::VoidAllocator allocator(m_parserArguments.getSegmentManager());
		SharedParserArguments args(allocator);

		args.setCompilationDatabasePath(arguments.compilationDatabasePath.str());
		args.setCompilerFlags(arguments.compilerFlags);
		args.setFrameworkSearchPaths(arguments.frameworkSearchPaths);
		args.setHeaderSearchPaths(arguments.headerSearchPaths);
		args.setJavaClassPaths(arguments.javaClassPaths);
		args.setLanguage(arguments.language);
		args.setLanguageStandard(arguments.languageStandard);
		args.setLogErrors(arguments.logErrors);
		args.setSystemHeaderSearchPaths(arguments.systemHeaderSearchPaths);

		m_parserArguments.pushValue(args);
	}
}

Parser::Arguments InterprocessDataManager::popParserArguments()
{
	IF_INITIALIZED(Parser::Arguments())
	{
		if (m_parserArguments.size() > 0)
		{
			SharedParserArguments args = m_parserArguments.popValue();

			Parser::Arguments result;

			result.compilationDatabasePath = FilePath(args.getCompilationDatabasePath());
			result.compilerFlags = args.getCompilerFlags();
			result.frameworkSearchPaths = args.getFrameworkSearchPaths();
			result.headerSearchPaths = args.getHeaderSearchPaths();
			result.javaClassPaths = args.getJavaClassPaths();
			result.language = args.getLanguage();
			result.languageStandard = args.getLanguageStandard();
			result.logErrors = args.getLogErrors();
			result.systemHeaderSearchPaths = args.getSystemHeaderSearchPaths();

			return result;
		}
	}
}

unsigned int InterprocessDataManager::parserArgumentCount() const
{
	IF_INITIALIZED(0)
	{
		return m_parserArguments.size();
	}
}

void InterprocessDataManager::cleanSharedMemory()
{
	std::vector<std::string> instances = SharedUUIDManager::getInstance()->getStoredInstanceUUIDs();

	for (unsigned int i = 0; i < instances.size(); i++)
	{
		std::vector<std::string> uuids = SharedUUIDManager::getInstance()->getUUIDsForInstance(instances[i]);

		if (uuids.size() == 1) // uuid list has to equal the count of used containers
		{
			std::string parserArgumentMemName = SharedQueue<SharedParserArguments>::getMemoryNamePrefix() + uuids[0];
			boost::interprocess::shared_memory_object::remove(parserArgumentMemName.c_str());
		}
	}
}
