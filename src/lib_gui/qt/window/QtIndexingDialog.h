#ifndef QT_INDEXING_DIALOG_H
#define QT_INDEXING_DIALOG_H

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QSizeGrip>
#include <QWidget>

#include "QtWindowBase.h"
#include "QtWindowStack.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

class QtIndexingDialog: public QtWindowBase
{
	Q_OBJECT

protected:
	static QBoxLayout* createLayout(QWidget* parent);
	static QLabel* createTitleLabel(const QString &title, QBoxLayout* layout);
	static QLabel* createMessageLabel(QBoxLayout* layout);
	static QWidget* createErrorWidget(QBoxLayout* layout);
	static QLabel* createFlagLabel(QWidget* parent);

public:
	QtIndexingDialog(bool isSubWindow, QWidget* parent = nullptr);
	QSize sizeHint() const override = 0;

signals:
	void finished();
	void canceled();
	void visibleChanged(bool visible);

protected:
	void resizeEvent(QResizeEvent* event) override;

	void setupDone();

	QVBoxLayout* m_layout;
};

#endif	  // QT_INDEXING_DIALOG_H
