#ifndef QT_ERROR_VIEW_H
#define QT_ERROR_VIEW_H

#include <QWidget>

#include "component/view/ErrorView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QBoxLayout;
class QCheckBox;
class QPalette;
class QStandardItemModel;
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
	virtual void addError(const StorageError& error);

private slots:
	void clickedInEmptySpace();

private:
	enum COLUMN {
		TYPE = 0,
		MESSAGE = 1,
		FILE = 2,
		LINE = 3,
		INDEXED = 4,
		ID = 5
	};

	void doRefreshView();
	void doClear();
	void doAddError(const StorageError& error);

	void setStyleSheet() const;

	void addErrorToTable(const StorageError& error);

	QCheckBox* createFilterCheckbox(const QString& name, bool checked, QBoxLayout* layout);
	bool isShownError(const StorageError& error);

	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
	QtThreadedFunctor<const StorageError&> m_addErrorFunctor;

	QCheckBox* m_showErrors;
	QCheckBox* m_showFatals;
	QCheckBox* m_showNonIndexedErrors;
	QCheckBox* m_showNonIndexedFatals;

	QStandardItemModel* m_model;
	QtTable* m_table;

	std::vector<StorageError> m_errors;
	QPalette* m_palette;
};

#endif // QT_ERROR_VIEW_H
