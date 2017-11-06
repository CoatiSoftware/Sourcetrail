#include "data/parser/java/JavaParser.h"

#include <jni.h>

#include "data/name/NameHierarchy.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ReferenceKind.h"
#include "data/parser/ParserClient.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/text/TextAccess.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityJava.h"
#include "utility/utilityString.h"

void JavaParser::clearCaches()
{
	std::shared_ptr<JavaEnvironmentFactory> factory = JavaEnvironmentFactory::getInstance();
	if (factory)
	{
		std::shared_ptr<JavaEnvironment> environment = factory->createEnvironment();
		if (environment)
		{
			environment->callStaticVoidMethod(
				"com/sourcetrail/JavaIndexer",
				"clearCaches"
			);
		}
	}
}

JavaParser::JavaParser(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister)
	: Parser(client)
	, m_id(s_nextParserId++)
{
	const std::string errorString = utility::prepareJavaEnvironment();
	if (errorString.size() > 0)
	{
		LOG_ERROR(errorString);
	}

	std::shared_ptr<JavaEnvironmentFactory> factory = JavaEnvironmentFactory::getInstance();
	if (factory)
	{
		m_javaEnvironment = factory->createEnvironment();

		std::vector<JavaEnvironment::NativeMethod> methods;

		methods.push_back({"getInterrupted", "(I)Z", (void*)&JavaParser::GetInterrupted});
		methods.push_back({"logInfo", "(ILjava/lang/String;)V", (void*)&JavaParser::LogInfo});
		methods.push_back({"logWarning", "(ILjava/lang/String;)V", (void*)&JavaParser::LogWarning});
		methods.push_back({"logError", "(ILjava/lang/String;)V", (void*)&JavaParser::LogError});
		methods.push_back({"recordSymbol", "(ILjava/lang/String;III)V", (void*)&JavaParser::RecordSymbol});
		methods.push_back({"recordSymbolWithLocation", "(ILjava/lang/String;IIIIIII)V", (void*)&JavaParser::RecordSymbolWithLocation});
		methods.push_back({"recordSymbolWithLocationAndScope", "(ILjava/lang/String;IIIIIIIIIII)V", (void*)&JavaParser::RecordSymbolWithLocationAndScope});
		methods.push_back({"recordReference", "(IILjava/lang/String;Ljava/lang/String;IIII)V", (void*)&JavaParser::RecordReference});
		methods.push_back({"recordQualifierLocation", "(ILjava/lang/String;IIII)V", (void*)&JavaParser::RecordQualifierLocation});
		methods.push_back({"recordLocalSymbol", "(ILjava/lang/String;IIII)V", (void*)&JavaParser::RecordLocalSymbol});
		methods.push_back({"recordComment", "(IIIII)V", (void*)&JavaParser::RecordComment});
		methods.push_back({"recordError", "(ILjava/lang/String;IIIIII)V", (void*)&JavaParser::RecordError});

		m_javaEnvironment->registerNativeMethods("com/sourcetrail/JavaIndexer", methods);
	}
	{
		std::lock_guard<std::mutex> lock(s_parsersMutex);
		s_parsers[m_id] = this;
	}
}

JavaParser::~JavaParser()
{
	s_parsers.erase(m_id);
}

void JavaParser::buildIndex(std::shared_ptr<IndexerCommandJava> indexerCommand)
{
	std::string classPath = "";
	for (const FilePath& path: indexerCommand->getClassPath())
	{
		// the separator used here should be the same as the one used in JavaIndexer.java
		classPath += path.str() + ";";
	}

	buildIndex(indexerCommand->getSourceFilePath(), indexerCommand->getLanguageStandard(), classPath, TextAccess::createFromFile(indexerCommand->getSourceFilePath()));
}

void JavaParser::buildIndex(const FilePath& filePath, std::shared_ptr<TextAccess> textAccess)
{
	buildIndex(filePath, "8", "", textAccess);
}

void JavaParser::buildIndex(
	const FilePath& sourceFilePath,
	const std::string& languageStandard,
	const std::string& classPath,
	std::shared_ptr<TextAccess> textAccess)
{
	if (m_javaEnvironment)
	{
		m_currentFilePath = sourceFilePath;

		m_client->onFileParsed(FileSystem::getFileInfoForPath(sourceFilePath));

		// remove tabs because they screw with javaparser's location resolver
		std::string fileContent = utility::replace(textAccess->getText(), "\t", " ");

		int verbose = ApplicationSettings::getInstance()->getLoggingEnabled() &&
			ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled() ? 1 : 0;

		m_javaEnvironment->callStaticVoidMethod(
			"com/sourcetrail/JavaIndexer",
			"processFile",
			m_id,
			m_currentFilePath.str(),
			fileContent,
			languageStandard,
			classPath,
			verbose
		);
	}
}

int JavaParser::s_nextParserId = 0;

std::map<int, JavaParser*> JavaParser::s_parsers;

std::mutex JavaParser::s_parsersMutex;










// definition of native methods

bool JavaParser::doGetInterrupted()
{
	return m_interruptCounter.getCount() > 0;
}

void JavaParser::doLogInfo(jstring jInfo)
{
	LOG_INFO_STREAM_BARE(<< "Indexer - " << m_javaEnvironment->toStdString(jInfo));
}

void JavaParser::doLogWarning(jstring jWarning)
{
	LOG_WARNING_STREAM_BARE(<< "Indexer - " << m_javaEnvironment->toStdString(jWarning));
}

void JavaParser::doLogError(jstring jError)
{
	LOG_ERROR_STREAM_BARE(<< "Indexer - " << m_javaEnvironment->toStdString(jError));
}

void JavaParser::doRecordSymbol(
	jstring jSymbolName, jint jSymbolKind,
	jint jAccess, jint jDefinitionKind
)
{
	AccessKind access = intToAccessKind(jAccess);
	DefinitionKind definitionKind = intToDefinitionKind(jDefinitionKind);

	m_client->recordSymbol(
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jSymbolName)),
		intToSymbolKind(jSymbolKind),
		access,
		definitionKind
	);
}

void JavaParser::doRecordSymbolWithLocation(
	jstring jSymbolName, jint jSymbolKind,
	jint beginLine, jint beginColumn, jint endLine, jint endColumn,
	jint jAccess, jint jDefinitionKind
)
{
	AccessKind access = intToAccessKind(jAccess);
	DefinitionKind definitionKind = intToDefinitionKind(jDefinitionKind);

	m_client->recordSymbol(
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jSymbolName)),
		intToSymbolKind(jSymbolKind),
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn),
		access,
		definitionKind
	);
}

void JavaParser::doRecordSymbolWithLocationAndScope(
	jstring jSymbolName, jint jSymbolKind,
	jint beginLine, jint beginColumn, jint endLine, jint endColumn,
	jint scopeBeginLine, jint scopeBeginColumn, jint scopeEndLine, jint scopeEndColumn,
	jint jAccess, jint jDefinitionKind
)
{
	AccessKind access = intToAccessKind(jAccess);
	DefinitionKind definitionKind = intToDefinitionKind(jDefinitionKind);

	m_client->recordSymbol(
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jSymbolName)),
		intToSymbolKind(jSymbolKind),
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn),
		ParseLocation(m_currentFilePath, scopeBeginLine, scopeBeginColumn, scopeEndLine, scopeEndColumn),
		access,
		definitionKind
	);
}

void JavaParser::doRecordReference(
	jint jReferenceKind, jstring jReferencedName, jstring jContextName,
	jint beginLine, jint beginColumn, jint endLine, jint endColumn
)
{
	m_client->recordReference(
		intToReferenceKind(jReferenceKind),
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jReferencedName)),
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jContextName)),
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn)
	);
}

void JavaParser::doRecordQualifierLocation(
	jstring jQualifierName,
	jint beginLine, jint beginColumn, jint endLine, jint endColumn
)
{
	m_client->recordQualifierLocation(
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jQualifierName)),
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn)
	);
}

void JavaParser::doRecordLocalSymbol(jstring jSymbolName, jint beginLine, jint beginColumn, jint endLine, jint endColumn)
{
	m_client->onLocalSymbolParsed(
		NameHierarchy::deserialize(m_javaEnvironment->toStdString(jSymbolName)).getQualifiedName(),
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn)
	);
}

void JavaParser::doRecordComment(
	jint beginLine, jint beginColumn, jint endLine, jint endColumn
)
{
	m_client->onCommentParsed(
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn)
	);
}

void JavaParser::doRecordError(
	jstring jMessage, jint jFatal, jint jIndexed,
	jint beginLine, jint beginColumn, jint endLine, jint endColumn
)
{
	bool fatal = jFatal;
	bool indexed = jIndexed;

	m_client->onError(
		ParseLocation(m_currentFilePath, beginLine, beginColumn, endLine, endColumn),
		m_javaEnvironment->toStdString(jMessage),
		"",
		fatal, indexed
	);
}

