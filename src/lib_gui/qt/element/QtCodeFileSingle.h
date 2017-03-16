#ifndef QT_CODE_FILE_SINGLE_H
#define QT_CODE_FILE_SINGLE_H

#include <deque>
#include <map>

#include <QFrame>

#include "utility/TimePoint.h"

#include "qt/element/QtCodeNavigateable.h"

class QLabel;
class QPushButton;
class QtCodeArea;
class QtCodeFileTitleButton;
class QtCodeNavigator;

class QtCodeFileSingle
	: public QFrame
	, public QtCodeNavigateable
{
	Q_OBJECT

public:
	QtCodeFileSingle(QtCodeNavigator* navigator, QWidget* parent = nullptr);
	virtual ~QtCodeFileSingle();

	void clearCache();

	// QtCodeNaviatebale implementation
	virtual QAbstractScrollArea* getScrollArea() override;

	virtual void addCodeSnippet(const CodeSnippetParams& params, bool insert = false) override;

	void requestFileContent(const FilePath& filePath);
	virtual bool requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, bool onTop) override;

	virtual void updateFiles() override;
	virtual void showContents() override;

	virtual void onWindowFocus() override;

	const FilePath& getCurrentFilePath() const;

	Id getLocationIdOfFirstActiveLocationOfTokenId(Id tokenId) const;

private:
	struct FileData
	{
		FilePath filePath;
		TimePoint modificationTime;
		std::string title;

		std::shared_ptr<QtCodeArea> area;
	};

	FileData getFileData(const FilePath& filePath) const;
	void setFileData(const FileData& file);

	void updateRefCount(int refCount);

	QtCodeNavigator* m_navigator;

	QWidget* m_areaWrapper;
	FilePath m_currentFilePath;

	QtCodeFileTitleButton* m_title;
	QLabel* m_referenceCount;

	QtCodeArea* m_area;
	std::map<FilePath, FileData> m_fileDatas;
	std::deque<FilePath> m_filePaths;

	bool m_contentRequested;
};

#endif // QT_CODE_FILE_SINGLE_H
