// from: http://doc.qt.io/qt-5/qtwidgets-layouts-flowlayout-flowlayout-h.html

#ifndef QT_FLOW_LAYOUT_H
#define QT_FLOW_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>

class QtFlowLayout: public QLayout
{
public:
	explicit QtFlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
	explicit QtFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
	~QtFlowLayout();

	int horizontalSpacing() const;
	int verticalSpacing() const;

	Qt::Orientations expandingDirections() const override;

	QSize minimumSize() const override;
	void setGeometry(const QRect& rect) override;
	QSize sizeHint() const override;

	bool hasHeightForWidth() const override;
	int heightForWidth(int) const override;


	void addItem(QLayoutItem* item) override;
	QLayoutItem* itemAt(int index) const override;
	QLayoutItem* takeAt(int index) override;

	int count() const override;

private:
	int doLayout(QRect rect, bool testOnly) const;
	int smartSpacing(QStyle::PixelMetric pm) const;

	QList<QLayoutItem*> itemList;

	int m_hSpace;
	int m_vSpace;
};

#endif	  // QT_FLOW_LAYOUT_H
