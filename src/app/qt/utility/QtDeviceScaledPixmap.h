#ifndef QT_DEVICE_SCALED_PIXMAP_H
#define QT_DEVICE_SCALED_PIXMAP_H

#include <QPixmap>

class QtDeviceScaledPixmap
{
public:
	static qreal devicePixelRatio();

	explicit QtDeviceScaledPixmap(QString filePath);
	virtual ~QtDeviceScaledPixmap();

	const QPixmap& pixmap() const;

	qreal width() const;
	qreal height() const;

	void scaleToWidth(int width);
	void scaleToHeight(int height);

	void mirror(bool horizontal = false, bool vertical = true);

private:
	QPixmap m_pixmap;
};

#endif // QT_DEVICE_SCALED_PIXMAP_H
