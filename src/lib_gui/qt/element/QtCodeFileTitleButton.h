#ifndef QT_CODE_FILE_TITLE_BUTTON_H
#define QT_CODE_FILE_TITLE_BUTTON_H

#include <QPushButton>

#include "utility/file/FilePath.h"
#include "utility/TimeStamp.h"

class QtCodeFileTitleButton
	: public QPushButton
{
	Q_OBJECT

public:
	QtCodeFileTitleButton(QWidget* parent = nullptr);
	virtual ~QtCodeFileTitleButton();

	void setFilePath(const FilePath& filePath);
	void setModificationTime(const TimeStamp modificationTime);
	void setIsComplete(bool isComplete);
	void setProject(const std::wstring& name);

	void updateTexts();

	void updateFromOther(const QtCodeFileTitleButton* other);

protected:
	void contextMenuEvent(QContextMenuEvent* event);

private slots:
	void clickedTitle();

private:
	FilePath m_filePath;
	TimeStamp m_modificationTime;
	bool m_isComplete;
};

#endif // QT_CODE_FILE_TITLE_BUTTON_H
