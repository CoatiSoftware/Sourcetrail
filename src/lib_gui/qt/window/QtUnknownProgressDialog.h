#ifndef QT_UNKNOWN_PROGRESS_DIALOG_H
#define QT_UNKNOWN_PROGRESS_DIALOG_H

#include "QtProgressBarDialog.h"

class QtUnknownProgressDialog: public QtProgressBarDialog
{
	Q_OBJECT

public:
	QtUnknownProgressDialog(bool hideable, QWidget* parent = 0);
	QSize sizeHint() const override;

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void onHidePressed();
};

#endif	  // QT_UNKNOWN_PROGRESS_DIALOG_H
