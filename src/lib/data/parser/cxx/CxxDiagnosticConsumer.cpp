#include "data/parser/cxx/CxxDiagnosticConsumer.h"

#include "clang/Basic/SourceManager.h"

#include "utility/file/FileManager.h"

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

CxxDiagnosticConsumer::CxxDiagnosticConsumer(
	clang::raw_ostream &os,
	clang::DiagnosticOptions *diags,
	ParserClient* client,
	const FileManager* fileManager,
	bool useLogging
)
	: clang::TextDiagnosticPrinter(os, diags)
	, m_client(client)
	, m_fileManager(fileManager)
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

	if (level == clang::DiagnosticsEngine::Error || level == clang::DiagnosticsEngine::Fatal)
	{
		llvm::SmallString<100> messageStr;
		info.FormatDiagnostic(messageStr);
		std::string message = messageStr.str();

		std::string filePath;
		uint line = 0;
		uint column = 0;

		if (info.getLocation().isValid() && info.hasSourceManager())
		{
			const clang::SourceManager& sourceManager = info.getSourceManager();
			clang::PresumedLoc presumedLocation = sourceManager.getPresumedLoc(info.getLocation());

			filePath = presumedLocation.getFilename();
			line = presumedLocation.getLine();
			column = presumedLocation.getColumn();
		}

		// if (m_fileManager->hasFilePath(filePath))
		if (m_fileManager)
		{
			m_client->onError(ParseLocation(filePath, line, column), message);
		}
	}
}
