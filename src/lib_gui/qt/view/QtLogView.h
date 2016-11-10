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

private:
	enum LOGVIEW_COLUMN
	{
		TYPE = 0,
		TIMESTAMP = 1,
		MESSAGE = 2,
	};

	struct Log
	{
		Log(const Logger::LogLevel type, const std::string message, const std::string timestamp)
			: type(type)
			, message(message)
			, timestamp(timestamp){}
		const Logger::LogLevel type;
		const std::string message;
		const std::string timestamp;
	};
	void doClear();
	void doRefreshView();
	void doAddLog(Logger::LogLevel type, const LogMessage& message);

	std::vector<Log> m_logs;
	const char* getLogType(Logger::LogLevel type) const;
	void addLogToTable(Log log);

	bool isCheckedType(const Logger::LogLevel type) const;
	QCheckBox* createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked = false);

	void setStyleSheet() const;

	void setLoggingEnabled(bool enabled);
	void setAstLoggingEnabled(bool enabled);

	void updateMask();
	void updateTable();

	int m_mask;
	QtTable* m_table;
	QStandardItemModel* m_model;

	QCheckBox* m_viewEnabled;
	QCheckBox* m_showAstLogging;

	QCheckBox* m_showErrors;
	QCheckBox* m_showWarnings;
	QCheckBox* m_showInfo;

	QtThreadedFunctor<Logger::LogLevel, const LogMessage&> m_addLogFunctor;
	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
};

#endif // QT_LOG_VIEW_H
