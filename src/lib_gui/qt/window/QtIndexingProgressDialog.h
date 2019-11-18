#ifndef QT_INDEXING_PROGRESS_DIALOG_H
#define QT_INDEXING_PROGRESS_DIALOG_H

#include "QtProgressBarDialog.h"

class QLabel;

class QtIndexingProgressDialog: public QtProgressBarDialog
{
	Q_OBJECT

public:
	QtIndexingProgressDialog(bool hideable, QWidget* parent = 0);
	QSize sizeHint() const override;

	void updateIndexingProgress(size_t fileCount, size_t totalFileCount, const FilePath& sourcePath);
	void updateErrorCount(size_t errorCount, size_t fatalCount);

protected:
	void closeEvent(QCloseEvent* event) override;
	void setGeometries() override;

private:
	void onHidePressed();
	void onStopPressed();

	QLabel* m_filePathLabel;
	QWidget* m_errorWidget;
	QString m_sourcePath;
};

#endif	  // QT_INDEXING_PROGRESS_DIALOG_H
