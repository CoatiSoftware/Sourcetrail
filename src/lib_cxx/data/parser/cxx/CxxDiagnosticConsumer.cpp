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

		ParseLocation location(FilePath(), 0, 0);
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
				location = utility::getParseLocation(loc, sourceManager, nullptr, m_canonicalFilePathCache);
			}
			else
			{
				fileEntry = sourceManager.getFileEntryForID(sourceManager.getMainFileID());
				if (fileEntry != nullptr && fileEntry->isValid())
				{
					location = ParseLocation(m_canonicalFilePathCache->getCanonicalFilePath(fileEntry), 1, 1);
				}
			}
		}
		else
		{
			location = ParseLocation(m_canonicalFilePathCache->getCanonicalFilePath(m_sourceFilePath.wstr()), 1, 1);
		}
		if (location.isValid())
		{
			m_client->recordError(
				location,
				utility::decodeFromUtf8(message),
				level == clang::DiagnosticsEngine::Fatal,
				m_canonicalFilePathCache->getFileRegister()->hasFilePath(location.filePath),
				m_sourceFilePath
			);
		}
	}
}
