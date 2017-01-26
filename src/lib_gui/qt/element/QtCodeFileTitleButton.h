#ifndef QT_CODE_FILE_TITLE_BUTTON_H
#define QT_CODE_FILE_TITLE_BUTTON_H

#include <QPushButton>

#include "utility/file/FilePath.h"
#include "utility/TimePoint.h"

class QtCodeFileTitleButton
	: public QPushButton
{
	Q_OBJECT

public:
	QtCodeFileTitleButton(QWidget* parent = nullptr);
	virtual ~QtCodeFileTitleButton();

	void setFilePath(const FilePath& filePath);
	void setModificationTime(const TimePoint modificationTime);
	void setProject(const std::string& name);

	void checkModification();

protected:
	void contextMenuEvent(QContextMenuEvent* event);

private slots:
	void clickedTitle();

private:
	FilePath m_filePath;
	TimePoint m_modificationTime;
};

#endif // QT_CODE_FILE_TITLE_BUTTON_H
