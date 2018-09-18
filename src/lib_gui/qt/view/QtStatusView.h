#ifndef QT_STATUS_VIEW_H
#define QT_STATUS_VIEW_H

#include <QWidget>

#include "StatusView.h"
#include "QtThreadedFunctor.h"

class QBoxLayout;
class QCheckBox;
class QStandardItemModel;
class QtTable;

class QtStatusView
	: public QWidget
	, public StatusView
{
	Q_OBJECT

public:
	QtStatusView(ViewLayout* viewLayout);
	virtual ~QtStatusView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// Status View Implementation
	virtual void clear();
	virtual void addStatus(const std::vector<Status>& status);

private:
	enum STATUSVIEW_COLUMN
	{
		TYPE = 0,
		STATUS = 1,
	};

	QCheckBox* createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked = false);

	QtThreadedLambdaFunctor m_onQtThread;

	QtTable* m_table;
	QStandardItemModel* m_model;

	std::vector<Status> m_status;

	QCheckBox* m_showErrors;
	QCheckBox* m_showInfo;
};

#endif // QT_STATUS_VIEW_H
