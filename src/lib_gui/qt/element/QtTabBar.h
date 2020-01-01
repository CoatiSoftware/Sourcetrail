#ifndef QT_TAB_BAR_H
#define QT_TAB_BAR_H

#include <QTabBar>

class QtTabBar: public QTabBar
{
    Q_OBJECT
    
public:
	QtTabBar(QWidget* parent = nullptr);

signals:
    void signalCloseTabsToRight(int tabNum);
    
protected:
	QSize minimumSizeHint() const override;

	QSize tabSizeHint(int index) const override;
	QSize minimumTabSizeHint(int index) const override;
    void contextMenuEvent(QContextMenuEvent* event) override;

    QAction * m_closeTabsToRight;

private slots:
    void handleCloseTabsToRight();

    
};

#endif	  // QT_TAB_BAR_H
