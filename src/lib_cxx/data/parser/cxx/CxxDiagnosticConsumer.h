#ifndef CXX_DIAGNOSTIC_CONSUMER
#define CXX_DIAGNOSTIC_CONSUMER

#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "data/parser/cxx/cxxCacheTypes.h"

class FileRegister;
class ParserClient;

class CxxDiagnosticConsumer
	: public clang::TextDiagnosticPrinter
{
public:
	CxxDiagnosticConsumer(
		clang::raw_ostream &os,
		clang::DiagnosticOptions *diags,
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<FileRegister> fileRegister,
		std::shared_ptr<FilePathCache> canonicalFilePathCache,
		bool useLogging = true
	);

	void BeginSourceFile(const clang::LangOptions& langOptions, const clang::Preprocessor* preProcessor);
	void EndSourceFile();

	void HandleDiagnostic(clang::DiagnosticsEngine::Level level, const clang::Diagnostic& info);

	void setCommandLine(const std::string& commandline);

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_register;
	std::shared_ptr<FilePathCache> m_canonicalFilePathCache;

	std::string m_commandline;

	bool m_isParsingFile;
	bool m_useLogging;
};

#endif // CXX_DIAGNOSTIC_CONSUMER
