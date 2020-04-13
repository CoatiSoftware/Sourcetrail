#ifndef QT_ICON_BUTTON_H
#define QT_ICON_BUTTON_H

#include <QPushButton>

#include "../../../../lib/utility/file/FilePath.h"

class QtIconButton: public QPushButton
{
	Q_OBJECT
public:
	QtIconButton(const FilePath& iconPath, const FilePath& hoveredIconPath, QWidget* parent = nullptr);
	~QtIconButton() = default;

	void setColor(QColor color);

protected:
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);

private:
	void setIconFromPath(const FilePath& path);

	const FilePath m_iconPath;
	const FilePath m_hoveredIconPath;

	QColor m_color;
};

#endif	  // QT_ICON_BUTTON_H
