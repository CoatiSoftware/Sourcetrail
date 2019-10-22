#ifndef QT_INDEXING_DIALOG_H
#define QT_INDEXING_DIALOG_H

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QSizeGrip>
#include <QWidget>

#include "QtWindowStack.h"
#include "QtWindowBase.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

class QtIndexingDialog
	: public QtWindowBase
{
	Q_OBJECT

protected:
	static QBoxLayout* createLayout(QWidget* parent);
	static QLabel* createTitleLabel(QString title, QBoxLayout* layout);
	static QLabel* createMessageLabel(QBoxLayout* layout);
	static QWidget* createErrorWidget(QBoxLayout* layout);
	static QLabel* QtIndexingDialog::createFlagLabel(QWidget* parent);

public:
	QtIndexingDialog(bool isSubWindow, QWidget* parent = nullptr);
	virtual QSize sizeHint() const = 0;

signals:
	void finished();
	void canceled();
	void visibleChanged(bool visible);

protected:
	void resizeEvent(QResizeEvent* event) override;

	void setupDone();

	QVBoxLayout* m_layout;
};

#endif // QT_INDEXING_DIALOG_H
