#ifndef QT_ERROR_VIEW_H
#define QT_ERROR_VIEW_H

#include <QWidget>

#include "component/view/ErrorView.h"
#include "data/ErrorFilter.h"
#include "qt/utility/QtThreadedFunctor.h"

class QBoxLayout;
class QCheckBox;
class QLabel;
class QPushButton;
class QStandardItemModel;
class QtHelpButton;
class QtTable;

class QtErrorView
	: public QWidget
	, public ErrorView
{
	Q_OBJECT

public:
	QtErrorView(ViewLayout* viewLayout);
	virtual ~QtErrorView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// ErrorView implementation
	virtual void clear();
	virtual void addErrors(const std::vector<ErrorInfo>& errors, bool scrollTo);
	virtual void setErrorId(Id errorId);

	virtual void setErrorCount(ErrorCountInfo info);
	virtual void resetErrorLimit();

	virtual void showErrorHelpMessage();

private slots:
	void errorFilterChanged(int i = 0);
	void errorFilterChanged(int i, bool showErrors);

private:
	enum COLUMN {
		TYPE = 0,
		MESSAGE = 1,
		FILE = 2,
		LINE = 3,
		COMMANDLINE = 4,
		INDEXED = 5,
		ID = 6
	};

	void setStyleSheet() const;

	void addErrorToTable(const ErrorInfo& error);

	QCheckBox* createFilterCheckbox(const QString& name, bool checked, QBoxLayout* layout);
	bool isShownError(const ErrorInfo& error);

	static QIcon s_errorIcon;

	QtThreadedLambdaFunctor m_onQtThread;

	ErrorFilter m_errorFilter;

	QLabel* m_allLabel;
	QPushButton* m_allButton;

	QCheckBox* m_showErrors;
	QCheckBox* m_showFatals;
	QCheckBox* m_showNonIndexedErrors;
	QCheckBox* m_showNonIndexedFatals;

	QtHelpButton* m_helpButton;

	QStandardItemModel* m_model;
	QtTable* m_table;

	bool m_ignoreRowSelection;
};

#endif // QT_ERROR_VIEW_H
