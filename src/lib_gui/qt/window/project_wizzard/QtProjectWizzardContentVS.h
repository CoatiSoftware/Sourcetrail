#ifndef QT_PROJECT_WIZZARD_CONTENT_VS_H
#define QT_PROJECT_WIZZARD_CONTENT_VS_H

#include "QtProjectWizzardContent.h"

class QtProjectWizzardContentVS
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentVS(QtProjectWizzardWindow* window);

	virtual void populate(QGridLayout* layout, int& row) override;

private slots:
	void handleVSCDBClicked();
};

#endif // QT_PROJECT_WIZZARD_CONTENT_VS_H
