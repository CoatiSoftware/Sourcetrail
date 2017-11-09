#include "data/parser/cxx/CxxDiagnosticConsumer.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Tooling.h"

#include "data/parser/cxx/utilityClang.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"

CxxDiagnosticConsumer::CxxDiagnosticConsumer(
	clang::raw_ostream &os,
	clang::DiagnosticOptions *diags,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<FilePathCache> canonicalFilePathCache,
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

		std::string filePath;
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

			const clang::FileEntry *fileEntry = sourceManager.getFileEntryForID(sourceManager.getFileID(info.getLocation()));
			if (fileEntry)
			{
				filePath = m_canonicalFilePathCache->getValue(utility::getFileNameOfFileEntry(fileEntry)).str();
			}
		}

		ParseLocation location(m_canonicalFilePathCache->getValue(filePath), line, column);

		m_client->onErrorParsed(
			location,
			message,
			m_commandline,
			level == clang::DiagnosticsEngine::Fatal,
			m_register->hasFilePath(location.filePath)
		);
	}
}

void CxxDiagnosticConsumer::setCommandLine(const std::string& commandline)
{
	m_commandline = commandline;
}
