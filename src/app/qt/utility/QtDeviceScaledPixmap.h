#ifndef QT_DEVICE_SCALED_PIXMAP_H
#define QT_DEVICE_SCALED_PIXMAP_H

#include <QApplication>
#include <QPixmap>

class QtDeviceScaledPixmap
{
public:
	static qreal devicePixelRatio()
	{
		QApplication* app = dynamic_cast<QApplication*>(QCoreApplication::instance());
		return app->devicePixelRatio();
	}

	explicit QtDeviceScaledPixmap(QString filePath)
		: m_pixmap(filePath)
	{
		m_pixmap.setDevicePixelRatio(devicePixelRatio());
	}

	virtual ~QtDeviceScaledPixmap() {}

	const QPixmap& pixmap() const
	{
		return m_pixmap;
	}

	qreal width() const
	{
		return m_pixmap.width() / devicePixelRatio();
	}

	qreal height() const
	{
		return m_pixmap.height() / devicePixelRatio();
	}

	void scaleToWidth(int width)
	{
		m_pixmap = m_pixmap.scaledToWidth(width * devicePixelRatio(), Qt::SmoothTransformation);
		m_pixmap.setDevicePixelRatio(devicePixelRatio());
	}

	void scaleToHeight(int height)
	{
		m_pixmap = m_pixmap.scaledToHeight(height * devicePixelRatio(), Qt::SmoothTransformation);
		m_pixmap.setDevicePixelRatio(devicePixelRatio());
	}

	void mirror(bool horizontal = false, bool vertical = true)
	{
		m_pixmap = QPixmap::fromImage(m_pixmap.toImage().mirrored(horizontal, vertical));
		m_pixmap.setDevicePixelRatio(devicePixelRatio());
	}

private:
	QPixmap m_pixmap;
};

#endif // QT_DEVICE_SCALED_PIXMAP_H
