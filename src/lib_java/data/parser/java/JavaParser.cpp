#include "JavaParser.h"

#include <jni.h>

#include "ApplicationSettings.h"
#include "IndexerStateInfo.h"
#include "JavaEnvironmentFactory.h"
#include "NameHierarchy.h"
#include "ParseLocation.h"
#include "ParserClient.h"
#include "ReferenceKind.h"
#include "ResourcePaths.h"
#include "TextAccess.h"
#include "utilityJava.h"
#include "utilityString.h"

void JavaParser::clearCaches()
{
	std::shared_ptr<JavaEnvironmentFactory> factory = JavaEnvironmentFactory::getInstance();
	if (factory)
	{
		std::shared_ptr<JavaEnvironment> environment = factory->createEnvironment();
		if (environment)
		{
			environment->callStaticVoidMethod("com/sourcetrail/JavaIndexer", "clearCaches");
		}
	}
}

JavaParser::JavaParser(
	std::shared_ptr<ParserClient> client, std::shared_ptr<IndexerStateInfo> indexerStateInfo)
	: Parser(client), m_indexerStateInfo(indexerStateInfo), m_id(s_nextParserId++), m_currentFileId(0)
{
	const std::string errorString = utility::prepareJavaEnvironment();
	if (!errorString.empty())
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
		methods.push_back(
			{"recordSymbol", "(ILjava/lang/String;III)V", (void*)&JavaParser::RecordSymbol});
		methods.push_back(
			{"recordSymbolWithLocation",
			 "(ILjava/lang/String;IIIIIII)V",
			 (void*)&JavaParser::RecordSymbolWithLocation});
		methods.push_back(
			{"recordSymbolWithLocationAndScope",
			 "(ILjava/lang/String;IIIIIIIIIII)V",
			 (void*)&JavaParser::RecordSymbolWithLocationAndScope});
		methods.push_back(
			{"recordSymbolWithLocationAndScopeAndSignature",
			 "(ILjava/lang/String;IIIIIIIIIIIIIII)V",
			 (void*)&JavaParser::RecordSymbolWithLocationAndScopeAndSignature});
		methods.push_back(
			{"recordReference",
			 "(IILjava/lang/String;Ljava/lang/String;IIII)V",
			 (void*)&JavaParser::RecordReference});
		methods.push_back(
			{"recordQualifierLocation",
			 "(ILjava/lang/String;IIII)V",
			 (void*)&JavaParser::RecordQualifierLocation});
		methods.push_back(
			{"recordLocalSymbol", "(ILjava/lang/String;IIII)V", (void*)&JavaParser::RecordLocalSymbol});
		methods.push_back({"recordComment", "(IIIII)V", (void*)&JavaParser::RecordComment});
		methods.push_back(
			{"recordError", "(ILjava/lang/String;IIIIII)V", (void*)&JavaParser::RecordError});

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

	buildIndex(
		indexerCommand->getSourceFilePath(),
		indexerCommand->getLanguageStandard(),
		classPath,
		TextAccess::createFromFile(indexerCommand->getSourceFilePath()));
}

void JavaParser::buildIndex(const FilePath& filePath, std::shared_ptr<TextAccess> textAccess)
{
	buildIndex(filePath, L"12", "", textAccess);
}

void JavaParser::buildIndex(
	const FilePath& sourceFilePath,
	const std::wstring& languageStandard,
	const std::string& classPath,
	std::shared_ptr<TextAccess> textAccess)
{
	if (m_javaEnvironment)
	{
		m_currentFilePath = sourceFilePath;
		m_currentFileId = m_client->recordFile(sourceFilePath, true);
		m_client->recordFileLanguage(m_currentFileId, L"java");

		// remove tabs because they screw with javaparser's location resolver
		std::string fileContent = utility::replace(textAccess->getText(), "\t", " ");

		int verbose = ApplicationSettings::getInstance()->getLoggingEnabled() &&
				ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled()
			? 1
			: 0;

		m_javaEnvironment->callStaticVoidMethod(
			"com/sourcetrail/JavaIndexer",
			"processFile",
			m_id,
			m_currentFilePath.str(),
			fileContent,
			utility::encodeToUtf8(languageStandard),
			classPath,
			verbose);
	}
}

int JavaParser::s_nextParserId = 0;

std::map<int, JavaParser*> JavaParser::s_parsers;

std::mutex JavaParser::s_parsersMutex;


// definition of native methods

bool JavaParser::doGetInterrupted()
{
	return m_indexerStateInfo->indexingInterrupted;
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

void JavaParser::doRecordSymbol(jstring jSymbolName, jint jSymbolKind, jint jAccess, jint jDefinitionKind)
{
	Id symbolId = getOrCreateSymbolId(jSymbolName);
	m_client->recordSymbolKind(symbolId, intToSymbolKind(jSymbolKind));
	m_client->recordAccessKind(symbolId, intToAccessKind(jAccess));
	m_client->recordDefinitionKind(symbolId, intToDefinitionKind(jDefinitionKind));
}

void JavaParser::doRecordSymbolWithLocation(
	jstring jSymbolName,
	jint jSymbolKind,
	jint beginLine,
	jint beginColumn,
	jint endLine,
	jint endColumn,
	jint jAccess,
	jint jDefinitionKind)
{
	Id symbolId = getOrCreateSymbolId(jSymbolName);
	m_client->recordSymbolKind(symbolId, intToSymbolKind(jSymbolKind));
	m_client->recordLocation(
		symbolId,
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn),
		ParseLocationType::TOKEN);
	m_client->recordAccessKind(symbolId, intToAccessKind(jAccess));
	m_client->recordDefinitionKind(symbolId, intToDefinitionKind(jDefinitionKind));
}

void JavaParser::doRecordSymbolWithLocationAndScope(
	jstring jSymbolName,
	jint jSymbolKind,
	jint beginLine,
	jint beginColumn,
	jint endLine,
	jint endColumn,
	jint scopeBeginLine,
	jint scopeBeginColumn,
	jint scopeEndLine,
	jint scopeEndColumn,
	jint jAccess,
	jint jDefinitionKind)
{
	Id symbolId = getOrCreateSymbolId(jSymbolName);
	m_client->recordSymbolKind(symbolId, intToSymbolKind(jSymbolKind));
	m_client->recordLocation(
		symbolId,
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn),
		ParseLocationType::TOKEN);
	m_client->recordLocation(
		symbolId,
		ParseLocation(m_currentFileId, scopeBeginLine, scopeBeginColumn, scopeEndLine, scopeEndColumn),
		ParseLocationType::SCOPE);
	m_client->recordAccessKind(symbolId, intToAccessKind(jAccess));
	m_client->recordDefinitionKind(symbolId, intToDefinitionKind(jDefinitionKind));
}

void JavaParser::doRecordSymbolWithLocationAndScopeAndSignature(
	jstring jSymbolName,
	jint jSymbolKind,
	jint beginLine,
	jint beginColumn,
	jint endLine,
	jint endColumn,
	jint scopeBeginLine,
	jint scopeBeginColumn,
	jint scopeEndLine,
	jint scopeEndColumn,
	jint signatureBeginLine,
	jint signatureBeginColumn,
	jint signatureEndLine,
	jint signatureEndColumn,
	jint jAccess,
	jint jDefinitionKind)
{
	Id symbolId = getOrCreateSymbolId(jSymbolName);
	m_client->recordSymbolKind(symbolId, intToSymbolKind(jSymbolKind));
	m_client->recordLocation(
		symbolId,
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn),
		ParseLocationType::TOKEN);
	m_client->recordLocation(
		symbolId,
		ParseLocation(m_currentFileId, scopeBeginLine, scopeBeginColumn, scopeEndLine, scopeEndColumn),
		ParseLocationType::SCOPE);
	m_client->recordLocation(
		symbolId,
		ParseLocation(
			m_currentFileId, signatureBeginLine, signatureBeginColumn, signatureEndLine, signatureEndColumn),
		ParseLocationType::SIGNATURE);
	m_client->recordAccessKind(symbolId, intToAccessKind(jAccess));
	m_client->recordDefinitionKind(symbolId, intToDefinitionKind(jDefinitionKind));
}

void JavaParser::doRecordReference(
	jint jReferenceKind,
	jstring jReferencedName,
	jstring jContextName,
	jint beginLine,
	jint beginColumn,
	jint endLine,
	jint endColumn)
{
	m_client->recordReference(
		intToReferenceKind(jReferenceKind),
		getOrCreateSymbolId(jReferencedName),
		getOrCreateSymbolId(jContextName),
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn));
}

void JavaParser::doRecordQualifierLocation(
	jstring jQualifierName, jint beginLine, jint beginColumn, jint endLine, jint endColumn)
{
	Id symbolId = getOrCreateSymbolId(jQualifierName);
	m_client->recordLocation(
		symbolId,
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn),
		ParseLocationType::QUALIFIER);
}

void JavaParser::doRecordLocalSymbol(
	jstring jSymbolName, jint beginLine, jint beginColumn, jint endLine, jint endColumn)
{
	m_client->recordLocalSymbol(
		NameHierarchy::deserialize(
			utility::decodeFromUtf8(m_javaEnvironment->toStdString(jSymbolName)))
			.getQualifiedName(),
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn));
}

void JavaParser::doRecordComment(jint beginLine, jint beginColumn, jint endLine, jint endColumn)
{
	m_client->recordComment(
		ParseLocation(m_currentFileId, beginLine, beginColumn, endLine, endColumn));
}

void JavaParser::doRecordError(
	jstring jMessage,
	jint jFatal,
	jint jIndexed,
	jint beginLine,
	jint beginColumn,
	jint endLine,
	jint endColumn)
{
	bool fatal = jFatal;
	bool indexed = jIndexed;

	m_client->recordError(
		utility::decodeFromUtf8(m_javaEnvironment->toStdString(jMessage)),
		fatal,
		indexed,
		FilePath(),
		ParseLocation(m_currentFileId, beginLine, beginColumn));
}

Id JavaParser::getOrCreateSymbolId(jstring jSymbolName)
{
	std::string name = m_javaEnvironment->toStdString(jSymbolName);

	auto it = m_symbolNameToIdMap.find(name);
	if (it != m_symbolNameToIdMap.end())
	{
		return it->second;
	}

	Id symbolId = m_client->recordSymbol(NameHierarchy::deserialize(utility::decodeFromUtf8(name)));

	m_symbolNameToIdMap.emplace(name, symbolId);
	return symbolId;
}
