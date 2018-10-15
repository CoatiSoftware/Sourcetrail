#include "CxxDiagnosticConsumer.h"

#include <clang/Basic/SourceManager.h>
#include <clang/Tooling/Tooling.h>

#include "CanonicalFilePathCache.h"
#include "utilityClang.h"
#include "ParseLocation.h"
#include "ParserClient.h"
#include "utilityString.h"

CxxDiagnosticConsumer::CxxDiagnosticConsumer(
	clang::raw_ostream &os,
	clang::DiagnosticOptions *diags,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	const FilePath& sourceFilePath,
	bool useLogging
)
	: clang::TextDiagnosticPrinter(os, diags)
	, m_client(client)
	, m_canonicalFilePathCache(canonicalFilePathCache)
	, m_sourceFilePath(sourceFilePath)
	, m_useLogging(useLogging)
{
}

void CxxDiagnosticConsumer::BeginSourceFile(const clang::LangOptions& langOptions, const clang::Preprocessor* preProcessor)
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

void CxxDiagnosticConsumer::HandleDiagnostic(clang::DiagnosticsEngine::Level level, const clang::Diagnostic& info)
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

		if (message == "MS-style inline assembly is not available: Unable to find target for this triple (no targets are registered)")
		{
			return;
		}
		if (utility::isPrefix<std::string>("unknown argument:", message))
		{
			return;
		}

		FilePath filePath;
		uint lineNumber = 0;
		uint columnNumber = 0;
		if (info.getLocation().isValid() && info.hasSourceManager())
		{
			const clang::SourceManager& sourceManager = info.getSourceManager();

			clang::SourceLocation loc = sourceManager.getExpansionLoc(info.getLocation());
			if (loc.isInvalid())
			{
				loc = info.getLocation();
			}

			clang::FileID fileId = sourceManager.getFileID(loc);
			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);

			if (fileEntry != nullptr && fileEntry->isValid())
			{
				ParseLocation location = utility::getParseLocation(loc, sourceManager, nullptr, m_canonicalFilePathCache);
				filePath = m_canonicalFilePathCache->getCanonicalFilePath(location.fileId);
				lineNumber = location.startLineNumber;
				columnNumber = location.startColumnNumber;
			}
			else
			{
				fileEntry = sourceManager.getFileEntryForID(sourceManager.getMainFileID());
				if (fileEntry != nullptr && fileEntry->isValid())
				{
					filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
					lineNumber = 1;
					columnNumber = 1;
				}
			}
		}
		else
		{
			filePath = m_sourceFilePath;
			lineNumber = 1;
			columnNumber = 1;
		}

		if (!filePath.empty())
		{
			m_client->recordError(
				filePath,
				lineNumber,
				columnNumber,
				utility::decodeFromUtf8(message),
				level == clang::DiagnosticsEngine::Fatal,
				m_canonicalFilePathCache->getFileRegister()->hasFilePath(filePath),
				m_sourceFilePath
			);
		}
	}
}
