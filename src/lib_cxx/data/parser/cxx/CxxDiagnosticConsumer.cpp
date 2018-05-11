#include "data/parser/cxx/CxxDiagnosticConsumer.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"

#include "data/parser/cxx/CanonicalFilePathCache.h"
#include "data/parser/cxx/utilityClang.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"
#include "utility/utilityString.h"

CxxDiagnosticConsumer::CxxDiagnosticConsumer(
	clang::raw_ostream &os,
	clang::DiagnosticOptions *diags,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	bool useLogging
)
	: clang::TextDiagnosticPrinter(os, diags)
	, m_client(client)
	, m_register(fileRegister)
	, m_canonicalFilePathCache(canonicalFilePathCache)
	, m_isParsingFile(false)
	, m_useLogging(useLogging)
{
}

void CxxDiagnosticConsumer::BeginSourceFile(const clang::LangOptions& langOptions, const clang::Preprocessor* preProcessor)
{
	if (m_useLogging)
	{
		clang::TextDiagnosticPrinter::BeginSourceFile(langOptions, preProcessor);
	}

	m_isParsingFile = true;
}

void CxxDiagnosticConsumer::EndSourceFile()
{
	if (m_useLogging)
	{
		clang::TextDiagnosticPrinter::EndSourceFile();
	}

	m_isParsingFile = false;
}

void CxxDiagnosticConsumer::HandleDiagnostic(clang::DiagnosticsEngine::Level level, const clang::Diagnostic& info)
{
	if (m_useLogging)
	{
		clang::TextDiagnosticPrinter::HandleDiagnostic(level, info);
	}

	if (!m_isParsingFile)
	{
		return;
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

		FilePath filePath;
		uint line = 0;
		uint column = 0;

		if (info.getLocation().isValid() && info.hasSourceManager())
		{
			const clang::SourceManager& sourceManager = info.getSourceManager();
			{
				const clang::PresumedLoc presumedLocation = sourceManager.getPresumedLoc(info.getLocation());
				line = presumedLocation.getLine();
				column = presumedLocation.getColumn();
			}

			clang::SourceLocation loc = sourceManager.getExpansionLoc(info.getLocation());
			if (loc.isInvalid())
			{
				loc = info.getLocation();
			}

			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(sourceManager.getFileID(loc));
			if (fileEntry == nullptr || !fileEntry->isValid())
			{
				fileEntry = sourceManager.getFileEntryForID(sourceManager.getMainFileID());
			}

			if (fileEntry != nullptr && fileEntry->isValid())
			{
				filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
			}
		}

		ParseLocation location(filePath, line, column);

		m_client->recordError(
			location,
			utility::decodeFromUtf8(message),
			level == clang::DiagnosticsEngine::Fatal,
			m_register->hasFilePath(location.filePath)
		);
	}
}
