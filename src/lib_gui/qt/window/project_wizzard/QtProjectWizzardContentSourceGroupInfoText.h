#ifndef QT_PROJECT_WIZZARD_CONTENT_SOURCE_GROUP_INFO_TEXT_H
#define QT_PROJECT_WIZZARD_CONTENT_SOURCE_GROUP_INFO_TEXT_H

#include "QtProjectWizzardContent.h"

class QtProjectWizzardContentSourceGroupInfoText
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSourceGroupInfoText(QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SOURCE_GROUP_INFO_TEXT_H
