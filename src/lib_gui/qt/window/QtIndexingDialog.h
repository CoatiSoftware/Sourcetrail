#ifndef QT_INDEXING_WIZARD_WINDOW_H
#define QT_INDEXING_WIZARD_WINDOW_H

#include <functional>

#include "component/view/DialogView.h"
#include "qt/window/QtWindow.h"

class QCheckBox;
class QLabel;
class QtProgressBar;

class QtIndexingDialog
	: public QtWindow
{
	Q_OBJECT

public:
	enum DialogType
	{
		DIALOG_MESSAGE,
		DIALOG_STATUS,
		DIALOG_PROGRESS,
		DIALOG_INDEXING
	};

	QtIndexingDialog(QWidget* parent = 0);
	QSize sizeHint() const override;

	DialogType getType() const;

	void setupStart(size_t cleanFileCount, size_t indexFileCount, size_t totalFileCount,
		DialogView::IndexingOptions options, std::function<void(DialogView::IndexingOptions)> callback);
	void setupIndexing();
	void setupReport(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount, float time);

	void setupStatus();
	void setupProgress();

	void updateMessage(QString message);
	void updateProgress(int progress);
	void updateIndexingProgress(size_t fileCount, size_t totalFileCount, std::string sourcePath);
	void updateErrorCount(size_t errorCount, size_t fatalCount);

protected:
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

	virtual void handleNext() override;
	virtual void handleClose() override;

private:
	void setType(DialogType type);

	QBoxLayout* createLayout();
	void addTopAndProgressBar(float topRatio);
	void addTitle(QString title, QBoxLayout* layout);

	void addPercentLabel(QBoxLayout* layout);
	void addMessageLabel(QBoxLayout* layout);
	QLabel* createMessageLabel(QBoxLayout* layout);
	void addFilePathLabel(QBoxLayout* layout);
	void addErrorLabel(QBoxLayout* layout);

	void addButtons(QBoxLayout* layout);
	void addFlag();

	void setGeometries();
	void finishSetup();

	DialogType m_type;

	QWidget* m_top;
	float m_topRatio;
	QtProgressBar* m_progressBar;

	QLabel* m_percentLabel;
	QLabel* m_messageLabel;
	QLabel* m_filePathLabel;
	QPushButton* m_errorLabel;

	// start indexing
	QCheckBox* m_fullRefreshCheckBox;
	QCheckBox* m_preprocessorOnlyCheckBox;

	QSize m_sizeHint;

	std::function<void(DialogView::IndexingOptions)> m_callback;
	QString m_sourcePath;
};

#endif // QT_INDEXING_WIZARD_WINDOW_H
