#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QWidget>

#include "utility/types.h"

class QtCodeSnippet;
class TokenLocationFile;

class QtCodeFile : public QWidget
{
public:
	QtCodeFile(const std::string& fileName, QWidget *parent = 0);
	virtual ~QtCodeFile();

	const std::string& getFileName() const;

	void addCodeSnippet(
		int startLineNumber,
		const std::string& code,
		const TokenLocationFile& locationFile,
		const std::vector<Id>& activeTokenIds
	);

private:
	std::vector<std::shared_ptr<QtCodeSnippet> > m_snippets;
	const std::string m_fileName;
};

#endif // QT_CODE_FILE_H
