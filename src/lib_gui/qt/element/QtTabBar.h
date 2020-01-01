#ifndef QT_TAB_BAR_H
#define QT_TAB_BAR_H

#include <QTabBar>

class QtTabBar: public QTabBar
{
public:
	QtTabBar(QWidget* parent = nullptr);

protected:
	QSize minimumSizeHint() const override;

	QSize tabSizeHint(int index) const override;
	QSize minimumTabSizeHint(int index) const override;
    void contextMenuEvent(QContextMenuEvent* event) override;
private:
    QAction * m_closeTabsToRight;
   
    void closeTabsToRight(int tabNum);
signals:
    void signalCloseTabsToRight(int tabNum);
    
};

#endif	  // QT_TAB_BAR_H
