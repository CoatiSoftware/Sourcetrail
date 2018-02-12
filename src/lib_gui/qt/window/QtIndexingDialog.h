#ifndef QT_INDEXING_DIALOG_H
#define QT_INDEXING_DIALOG_H

#include <functional>

#include "project/RefreshInfo.h"
#include "qt/window/QtWindow.h"

class QCheckBox;
class QLabel;
class QRadioButton;
class QtProgressBar;

class QtIndexingDialog
	: public QtWindow
{
	Q_OBJECT

signals:
	void setMode(RefreshMode mode);
	void startIndexing(RefreshMode mode);

public:
	enum DialogType
	{
		DIALOG_MESSAGE,
		DIALOG_UNKNOWN_PROGRESS,
		DIALOG_PROGRESS,
		DIALOG_START_INDEXING,
		DIALOG_INDEXING,
		DIALOG_REPORT
	};

	QtIndexingDialog(QWidget* parent = 0);
	QSize sizeHint() const override;

	DialogType getType() const;

	void setupStart(const std::vector<RefreshMode>& enabledModes);
	void updateRefreshInfo(const RefreshInfo& info);

	void setupIndexing();
	void setupReport(
		size_t indexedFileCount, size_t totalIndexedFileCount, size_t completedFileCount, size_t totalFileCount,
		float time, bool interrupted);

	void setupUnknownProgress();
	void setupProgress();

	void updateMessage(const QString& message);
	std::wstring getMessage() const;
	void updateProgress(size_t progress);
	size_t getProgress() const;
	void updateIndexingProgress(size_t fileCount, size_t totalFileCount, const FilePath& sourcePath);
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
	void addErrorWidget(QBoxLayout* layout);

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
	QWidget* m_errorWidget;

	// start indexing
	QLabel* m_clearLabel;
	QLabel* m_indexLabel;
	std::map<RefreshMode, QRadioButton*> m_refreshModeButtons;

	QSize m_sizeHint;
	QString m_sourcePath;
};

#endif // QT_INDEXING_DIALOG_H
