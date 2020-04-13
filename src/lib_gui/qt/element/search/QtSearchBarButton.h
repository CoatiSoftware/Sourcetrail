#ifndef QT_SEARCH_BAR_BUTTON_H
#define QT_SEARCH_BAR_BUTTON_H

#include "../button/QtSelfRefreshIconButton.h"

class QtSearchBarButton: public QtSelfRefreshIconButton
{
public:
	QtSearchBarButton(const FilePath& iconPath, bool small = false, QWidget* parent = nullptr);

protected:
	void refresh() override;

private:
	bool m_small;
};

#endif	  // QT_SEARCH_BAR_BUTTON_H
