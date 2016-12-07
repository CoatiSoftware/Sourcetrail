#ifndef QT_STATUS_VIEW_H
#define QT_STATUS_VIEW_H

#include <QWidget>

#include "component/view/StatusView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/logging/Logger.h"

class QBoxLayout;
class QCheckBox;
class QPalette;
class QStandardItemModel;
class QtTable;

class QtStatusView
	: public StatusView
	, public QWidget
{
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

	void doClear();
	void doRefreshView();
	void doAddStatus(const std::vector<Status>& status);

	std::vector<Status> m_status;
	const char* getStatusTypeAsString(STATUSTYPE type) const;
	void addStatusToTable(Status status);

	QCheckBox* createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked = false);

	void setStyleSheet() const;

	void updateMask();
	void updateTable();

	QtTable* m_table;
	QStandardItemModel* m_model;

	QCheckBox* m_showErrors;
	QCheckBox* m_showInfo;

	QtThreadedFunctor<const std::vector<Status>&> m_addStatusFunctor;
	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
};

#endif // QT_STATUS_VIEW_H
