#ifndef QT_INDEXING_REPORT_DIALOG_H
#define QT_INDEXING_REPORT_DIALOG_H

#include "QtIndexingDialog.h"

class QtIndexingReportDialog: public QtIndexingDialog
{
	Q_OBJECT

signals:
	void requestReindexing();

public:
	QtIndexingReportDialog(
		size_t indexedFileCount,
		size_t totalIndexedFileCount,
		size_t completedFileCount,
		size_t totalFileCount,
		float time,
		bool interrupted,
		bool shallow,
		QWidget* parent = 0);
	QSize sizeHint() const override;

	void updateErrorCount(size_t errorCount, size_t fatalCount);

protected:
	void closeEvent(QCloseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	void onConfirmPressed();
	void onDiscardPressed();
	void onStartInDepthPressed();

	QWidget* m_errorWidget;
	bool m_interrupted;
};

#endif	  // QT_INDEXING_REPORT_DIALOG_H
