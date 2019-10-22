#ifndef QT_KNOWN_PROGRESS_DIALOG_H
#define QT_KNOWN_PROGRESS_DIALOG_H

#include "QtProgressBarDialog.h"

class QtKnownProgressDialog
	: public QtProgressBarDialog
{
	Q_OBJECT

public:
	QtKnownProgressDialog(bool hideable, QWidget* parent = 0);
	QSize sizeHint() const override;

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void onHidePressed();
};

#endif // QT_KNOWN_PROGRESS_DIALOG_H
