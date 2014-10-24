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
	QtCodeFile(const std::string& filePath, QWidget *parent = 0);
	virtual ~QtCodeFile();

	std::string getFileName() const;

	void addCodeSnippet(
		uint startLineNumber,
		const std::string& code,
		const TokenLocationFile& locationFile,
		const std::vector<Id>& activeTokenIds
	);

	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);
	void setShowMaximizeButton(bool show);

private:
	std::vector<std::shared_ptr<QtCodeSnippet> > m_snippets;
	const std::string m_filePath;
	bool m_showMaximizeButton;
};

#endif // QT_CODE_FILE_H
