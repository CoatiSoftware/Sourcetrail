#ifndef QT_LOG_VIEW_H
#define QT_LOG_VIEW_H

#include <QWidget>

#include "component/view/LogView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/logging/Logger.h"

class QBoxLayout;
class QCheckBox;
class QPalette;
class QStandardItemModel;
class QtTable;

class QtLogView
	: public LogView
	, public QWidget
{
public:
	QtLogView(ViewLayout* viewLayout);
	virtual ~QtLogView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// Log View Implementation
	virtual void clear();
	virtual void addLog(Logger::LogLevel type, const LogMessage& message);
	virtual void addLogs(const std::vector<Log>& logs);

private:
	enum LOGVIEW_COLUMN
	{
		TYPE = 0,
		TIMESTAMP = 1,
		MESSAGE = 2,
	};

	void doClear();
	void doRefreshView();
	void doAddLog(Logger::LogLevel type, const LogMessage& message);
	void doAddLogs(const std::vector<Log>& logs);

	std::vector<Log> m_logs;
	const char* getLogType(Logger::LogLevel type) const;
	void addLogToTable(Log log);
	void setLogFilter();

	bool isCheckedType(const Logger::LogLevel type) const;
	QCheckBox* createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked = false);

	void setStyleSheet() const;

	void setLoggingEnabled(bool enabled);
	void setAstLoggingEnabled(bool enabled);

	void updateMask();
	void updateTable();

	Logger::LogLevelMask m_logLevel;
	QtTable* m_table;
	QStandardItemModel* m_model;

	QCheckBox* m_viewEnabled;
	QCheckBox* m_showAstLogging;

	QCheckBox* m_showErrors;
	QCheckBox* m_showWarnings;
	QCheckBox* m_showInfo;

	QtThreadedFunctor<Logger::LogLevel, const LogMessage&> m_addLogFunctor;
	QtThreadedFunctor<const std::vector<Log>&> m_addLogsFunctor;
	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
};

#endif // QT_LOG_VIEW_H
