#include "qt/view/QtLogView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QStandardItemModel>

#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/ResourcePaths.h"
#include "qt/utility/utilityQt.h"
#include "utility/logging/LogManager.h"
#include "qt/element/QtTable.h"


QtLogView::QtLogView(ViewLayout* viewLayout)
	: LogView(viewLayout)
	, m_addLogFunctor(std::bind(&QtLogView::doAddLog, this, std::placeholders::_1, std::placeholders::_2))
	, m_clearFunctor(std::bind(&QtLogView::doClear, this))
	, m_refreshFunctor(std::bind(&QtLogView::doRefreshView, this))
{
}

QtLogView::~QtLogView()
{
}

void QtLogView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
}

void QtLogView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 10, 0, 5);
	layout->setSpacing(0);
	widget->setLayout(layout);

	QHBoxLayout* headerLayout = new QHBoxLayout();
	headerLayout->addSpacing(10);

	m_viewEnabled = new QCheckBox("Logging enabled (file, console logging)");
	m_viewEnabled->setChecked(ApplicationSettings::getInstance()->getLoggingEnabled());

	connect(m_viewEnabled, &QCheckBox::stateChanged,
		[=](int){
			setLoggingEnabled(m_viewEnabled->isChecked());
		}
	);
	headerLayout->addWidget(m_viewEnabled);
	headerLayout->addSpacing(25);
	m_showAstLogging = new QCheckBox("AST Logging");
	m_showAstLogging->setEnabled(m_viewEnabled->isChecked());
	m_showAstLogging->setChecked(ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled());
	connect(m_showAstLogging, &QCheckBox::stateChanged,
		[=](int){
			setAstLoggingEnabled(m_showAstLogging->isChecked());
		}
	);
	headerLayout->addWidget(m_showAstLogging);
	headerLayout->addStretch();
	layout->addLayout(headerLayout);

	m_table = new QtTable(this);
	m_model = new QStandardItemModel(this);
	m_table->setModel(m_model);

	m_model->setColumnCount(3);
	m_table->setColumnWidth(LOGVIEW_COLUMN::TYPE, 100);
	m_table->setColumnWidth(LOGVIEW_COLUMN::TIMESTAMP, 150);
	m_table->setColumnWidth(LOGVIEW_COLUMN::TYPE, 100);

	QStringList headers;
	headers << "Type" << "Timestamp" << "Message";
	m_model->setHorizontalHeaderLabels(headers);

	layout->addWidget(m_table);

	QHBoxLayout* filters = new QHBoxLayout();
	filters->addSpacing(15);

	m_showErrors = createFilterCheckbox("error", filters, true);
	m_showWarnings = createFilterCheckbox("warnings", filters, true);
	m_showInfo = createFilterCheckbox("info", filters);

	filters->addStretch();

	QPushButton* clearButton = new QPushButton("clear log");
	connect(clearButton, &QPushButton::clicked,
			[=]()
			{
				doClear();
			});
	filters->addWidget(clearButton);
	filters->addSpacing(30);

	updateMask();

	layout->addLayout(filters);

	doRefreshView();
}

QCheckBox* QtLogView::createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked)
{
	QCheckBox* checkbox = new QCheckBox(name);
	checkbox->setChecked(checked);

	connect(checkbox, &QCheckBox::stateChanged,
		[=](int)
		{
			m_table->selectionModel()->clearSelection();
			updateMask();
			updateTable();
		}
	);

	layout->addWidget(checkbox);
	layout->addSpacing(25);

	return checkbox;
}

void QtLogView::setLoggingEnabled(bool enabled)
{
	m_showAstLogging->setEnabled(enabled);
	LogManager::getInstance()->setLoggingEnabled(true);

	ApplicationSettings::getInstance()->setLoggingEnabled(enabled);
	ApplicationSettings::getInstance()->save();
	MessageRefresh msg;
	msg.reloadSettings = true;
	msg.uiOnly = true;
	msg.dispatchImmediately();
}

void QtLogView::setAstLoggingEnabled(bool enabled)
{
	ApplicationSettings::getInstance()->setVerboseIndexerLoggingEnabled(enabled);
	ApplicationSettings::getInstance()->save();
	MessageRefresh msg;
	msg.reloadSettings = true;
	msg.uiOnly = true;
	msg.dispatchImmediately();
}

void QtLogView::refreshView()
{
	m_refreshFunctor();
}

void QtLogView::clear()
{
	m_clearFunctor();
}

void QtLogView::addLog(Logger::LogLevel type, const LogMessage& message)
{
	m_addLogFunctor(type, message);
}

void QtLogView::doClear()
{
	if (!m_model->index(0, 0).data(Qt::DisplayRole).toString().isEmpty())
	{
		m_model->removeRows(0, m_model->rowCount());
	}

	m_logs.clear();
}

void QtLogView::doRefreshView()
{
	setStyleSheet();
}

void QtLogView::setStyleSheet() const
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, ColorScheme::getInstance()->getColor("error/background"));

	QPalette palette(m_showErrors->palette());
	palette.setColor(QPalette::WindowText, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));
	//palette.setColor(QPalette::Text, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));
	//palette.setColor(QPalette::ButtonText, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));

	//m_showErrors->setAutoFillBackground(true);
	//m_showErrors->setPalette(palette);
	//m_showFatals->setPalette(palette);
	//m_showNonIndexedErrors->setPalette(palette);
	//m_showNonIndexedFatals->setPalette(palette);

	widget->setStyleSheet(
		utility::getStyleSheet(ResourcePaths::getGuiPath() + "error_view/error_view.css").c_str()
	);

	m_table->updateRows();
}

const char* QtLogView::getLogType(Logger::LogLevel type) const
{
	switch (type)
	{
		case Logger::LOG_INFOS:
			return "INFO";
		case Logger::LOG_WARNINGS:
			return "WARNING";
		case Logger::LOG_ERRORS:
			return "ERROR";
		case Logger::LOG_ALL:
			return "UNKNOWN";
	}
}

bool QtLogView::isCheckedType(const Logger::LogLevel type) const
{
	return m_mask & type;
}

void QtLogView::updateTable()
{
	if (!m_model->index(0, 0).data(Qt::DisplayRole).toString().isEmpty())
	{
		m_model->removeRows(0, m_model->rowCount());
	}

	for ( Log log : m_logs )
	{
		if (log.type & m_mask)
		{
			addLogToTable(log);
		}
	}
}


void QtLogView::updateMask()
{
	m_mask =
		(m_showInfo->isChecked() ? Logger::LOG_INFOS : 0) +
		(m_showWarnings->isChecked() ? Logger::LOG_WARNINGS : 0) +
		(m_showErrors->isChecked() ? Logger::LOG_ERRORS : 0);
}

void QtLogView::addLogToTable(Log log)
{
	const int rowNumber = m_table->getFilledRowCount();
	if (rowNumber < m_model->rowCount())
	{
		m_model->insertRow(rowNumber);
	}

	m_model->setItem(rowNumber, LOGVIEW_COLUMN::TYPE, new QStandardItem(getLogType(log.type)));
	m_model->setItem(rowNumber, LOGVIEW_COLUMN::TIMESTAMP, new QStandardItem(log.timestamp.c_str()));
	m_model->setItem(rowNumber, LOGVIEW_COLUMN::MESSAGE, new QStandardItem(log.message.c_str()));
	m_table->updateRows();
}

void QtLogView::doAddLog(Logger::LogLevel type, const LogMessage& message)
{
	if (isCheckedType(type))
	{
		Log log(
			type,
			(message.getFileName().empty() ? "" : message.getFileName() + ": ") + message.message,
			message.getTimeString("%H:%M:%S"));
		m_logs.push_back(log);
		addLogToTable(log);
	}
}
