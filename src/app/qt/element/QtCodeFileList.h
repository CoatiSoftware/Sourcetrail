#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <memory>
#include <vector>

#include <QFrame>
#include <QScrollArea>

#include "utility/types.h"

class QtCodeFile;
class TokenLocationFile;

class QtCodeFileList: public QScrollArea
{
	Q_OBJECT

public:
	QtCodeFileList(QWidget* parent = 0);
	virtual ~QtCodeFileList();

	void addCodeSnippet(
		uint startLineNumber,
		const std::string& code,
		const TokenLocationFile& locationFile,
		const std::vector<Id>& activeTokenIds
	);

	void clearCodeSnippets();

	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);
	void setShowMaximizeButton(bool show);

private:
	std::shared_ptr<QFrame> m_frame;
	std::vector<std::shared_ptr<QtCodeFile> > m_files;

	bool m_showMaximizeButton;
};

#endif // QT_CODE_FILE_LIST
