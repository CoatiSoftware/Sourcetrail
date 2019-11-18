#include "CxxDiagnosticConsumer.h"

#include <clang/Basic/SourceManager.h>
#include <clang/Tooling/Tooling.h>

#include "CanonicalFilePathCache.h"
#include "ParseLocation.h"
#include "ParserClient.h"
#include "utilityClang.h"
#include "utilityString.h"

CxxDiagnosticConsumer::CxxDiagnosticConsumer(
	clang::raw_ostream& os,
	clang::DiagnosticOptions* diags,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	const FilePath& sourceFilePath,
	bool useLogging)
	: clang::TextDiagnosticPrinter(os, diags)
	, m_client(client)
	, m_canonicalFilePathCache(canonicalFilePathCache)
	, m_sourceFilePath(sourceFilePath)
	, m_useLogging(useLogging)
{
}

void CxxDiagnosticConsumer::BeginSourceFile(
	const clang::LangOptions& langOptions, const clang::Preprocessor* preProcessor)
{
	if (m_useLogging)
	{
		clang::TextDiagnosticPrinter::BeginSourceFile(langOptions, preProcessor);
	}
}

void CxxDiagnosticConsumer::EndSourceFile()
{
	if (m_useLogging)
	{
		clang::TextDiagnosticPrinter::EndSourceFile();
	}
}

void CxxDiagnosticConsumer::HandleDiagnostic(
	clang::DiagnosticsEngine::Level level, const clang::Diagnostic& info)
{
	if (m_useLogging)
	{
		clang::TextDiagnosticPrinter::HandleDiagnostic(level, info);
	}

	if (level >= clang::DiagnosticsEngine::Error)
	{
		llvm::SmallString<100> messageStr;
		info.FormatDiagnostic(messageStr);
		std::string message = messageStr.str();

		if (message ==
			"MS-style inline assembly is not available: Unable to find target for this triple (no "
			"targets are registered)")
		{
			return;
		}
		if (utility::isPrefix<std::string>("unknown argument:", message))
		{
			return;
		}

		Id fileId = 0;
		FilePath filePath;
		size_t lineNumber = 0;
		size_t columnNumber = 0;
		if (info.getLocation().isValid() && info.hasSourceManager())
		{
			const clang::SourceManager& sourceManager = info.getSourceManager();

			clang::SourceLocation loc = sourceManager.getExpansionLoc(info.getLocation());
			if (loc.isInvalid())
			{
				loc = info.getLocation();
			}

			clang::FileID clangFileId = sourceManager.getFileID(loc);
			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(clangFileId);

			if (fileEntry != nullptr && fileEntry->isValid())
			{
				ParseLocation location = utility::getParseLocation(
					loc, sourceManager, nullptr, m_canonicalFilePathCache);
				fileId = location.fileId;
				filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileId);
				lineNumber = location.startLineNumber;
				columnNumber = location.startColumnNumber;
			}
			else
			{
				fileEntry = sourceManager.getFileEntryForID(sourceManager.getMainFileID());
				if (fileEntry != nullptr && fileEntry->isValid())
				{
					filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
					fileId = m_client->recordFile(
						filePath, false /*keeps the "indexed" state if the file already exists*/);
					lineNumber = 1;
					columnNumber = 1;
				}
			}
		}
		else
		{
			filePath = m_sourceFilePath;
			fileId = m_client->recordFile(
				filePath, false /*keeps the "indexed" state if the file already exists*/);
			lineNumber = 1;
			columnNumber = 1;
		}

		if (fileId != 0)
		{
			m_client->recordError(
				utility::decodeFromUtf8(message),
				level == clang::DiagnosticsEngine::Fatal,
				m_canonicalFilePathCache->getFileRegister()->hasFilePath(filePath),
				m_sourceFilePath,
				ParseLocation(fileId, lineNumber, columnNumber));
		}
	}
}
