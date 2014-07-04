#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QWidget>

class QtCodeSnippet;
class QtCodeView;
class TokenLocationFile;

class QtCodeFile : public QWidget
{
public:
	QtCodeFile(QtCodeView* parentView, const std::string& fileName, QWidget *parent);
	virtual ~QtCodeFile();

	const std::string& getFileName() const;

	void addCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber);

private:
	QtCodeView* m_parentView;
	std::vector<std::shared_ptr<QtCodeSnippet> > m_snippets;
	const std::string m_fileName;
};

#endif // QT_CODE_FILE_H
