#ifndef QT_ERROR_VIEW_H
#define QT_ERROR_VIEW_H

#include <QWidget>

#include "component/controller/ErrorController.h"
#include "component/controller/helper/ControllerProxy.h"
#include "component/view/ErrorView.h"
#include "data/ErrorFilter.h"
#include "qt/utility/QtThreadedFunctor.h"

class QBoxLayout;
class QCheckBox;
class QLabel;
class QPushButton;
class QStandardItemModel;
class QtHelpButton;
class QtSelfRefreshIconButton;
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

	virtual void addErrors(const std::vector<ErrorInfo>& errors, const ErrorCountInfo& errorCount, bool scrollTo);
	virtual void setErrorId(Id errorId);

	virtual void showErrorHelpMessage();

	virtual ErrorFilter getErrorFilter() const;
	virtual void setErrorFilter(const ErrorFilter& filter);

private slots:
	void errorFilterChanged(int i = 0);

private:
	enum Column 
	{
		ID = 0,
		TYPE = 1,
		MESSAGE = 2,
		FILE = 3,
		LINE = 4,
		INDEXED = 5,
		TRANSLATION_UNIT = 6,
		COLUMN_MAX = TRANSLATION_UNIT
	};

	void setStyleSheet() const;

	void addErrorToTable(const ErrorInfo& error);

	QCheckBox* createFilterCheckbox(const QString& name, bool checked, QBoxLayout* layout);
	bool isShownError(const ErrorInfo& error);

	static QIcon s_errorIcon;

	ControllerProxy<ErrorController> m_controllerProxy;
	QtThreadedLambdaFunctor m_onQtThread;

	ErrorFilter m_errorFilter;

	QLabel* m_errorLabel;
	QLabel* m_allLabel;
	QPushButton* m_allButton;

	QCheckBox* m_showErrors;
	QCheckBox* m_showFatals;
	QCheckBox* m_showNonIndexedErrors;
	QCheckBox* m_showNonIndexedFatals;

	QtHelpButton* m_helpButton;
	QtSelfRefreshIconButton* m_editButton;

	QStandardItemModel* m_model;
	QtTable* m_table;

	bool m_ignoreRowSelection;
};

#endif // QT_ERROR_VIEW_H
