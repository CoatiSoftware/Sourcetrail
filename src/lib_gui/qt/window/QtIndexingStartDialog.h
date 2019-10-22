#ifndef QT_INDEXING_START_DIALOG_H
#define QT_INDEXING_START_DIALOG_H

#include "RefreshInfo.h"
#include "QtIndexingDialog.h"

class QLabel;
class QRadioButton;

class QtIndexingStartDialog
	: public QtIndexingDialog
{
	Q_OBJECT

signals:
	void setMode(RefreshMode mode);
	void startIndexing(RefreshMode mode);

public:
	QtIndexingStartDialog(const std::vector<RefreshMode>& enabledModes, const RefreshMode initialMode, QWidget* parent = 0);
	QSize sizeHint() const override;

	void updateRefreshInfo(const RefreshInfo& info);

protected:
	void closeEvent(QCloseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	void onStartPressed();
	void onCancelPressed();

	QLabel* m_clearLabel;
	QLabel* m_indexLabel;
	std::map<RefreshMode, QRadioButton*> m_refreshModeButtons;
};

#endif // QT_INDEXING_START_DIALOG_H
