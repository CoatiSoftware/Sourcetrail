#include "qt/view/QtStatusView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QStandardItemModel>

#include "qt/element/QtTable.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"
#include "utility/messaging/type/MessageClearStatusView.h"
#include "utility/messaging/type/MessageStatusFilterChanged.h"

QtStatusView::QtStatusView(ViewLayout* viewLayout)
	: StatusView(viewLayout)
	, m_addStatusFunctor(std::bind(&QtStatusView::doAddStatus, this, std::placeholders::_1))
	, m_clearFunctor(std::bind(&QtStatusView::doClear, this))
	, m_refreshFunctor(std::bind(&QtStatusView::doRefreshView, this))
{
}

QtStatusView::~QtStatusView()
{
}

void QtStatusView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
}

void QtStatusView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 5);
	layout->setSpacing(0);
	widget->setLayout(layout);

	m_table = new QtTable(this);
	m_model = new QStandardItemModel(this);
	m_table->setModel(m_model);

	m_model->setColumnCount(2);
	m_table->setColumnWidth(STATUSVIEW_COLUMN::TYPE, 100);
	//m_table->setColumnWidth(STATUSVIEW_COLUMN::STATUS, 150);

	QStringList headers;
	headers << "Type" << "Message";
	m_model->setHorizontalHeaderLabels(headers);

	layout->addWidget(m_table);

	// Setup filters
	QHBoxLayout* filters = new QHBoxLayout();
	filters->addSpacing(15);

	const StatusFilter filter = ApplicationSettings::getInstance()->getStatusFilter();
	m_showInfo = createFilterCheckbox("info", filters, filter & StatusType::STATUS_INFO);
	m_showErrors = createFilterCheckbox("error", filters, filter & StatusType::STATUS_ERROR);

	filters->addStretch();

	QPushButton* clearButton = new QPushButton("clear");
	connect(clearButton, &QPushButton::clicked,
		[=]()
		{
			MessageClearStatusView().dispatch();
		}
	);

	filters->addWidget(clearButton);
	filters->addSpacing(10);

	layout->addLayout(filters);

	doRefreshView();
}

QCheckBox* QtStatusView::createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked)
{
	QCheckBox* checkbox = new QCheckBox(name);
	checkbox->setChecked(checked);

	connect(checkbox, &QCheckBox::stateChanged,
		[=](int)
		{
			m_table->selectionModel()->clearSelection();

			const StatusFilter statusMask =
				(m_showInfo->isChecked() ? StatusType::STATUS_INFO : 0) +
				(m_showErrors->isChecked() ? StatusType::STATUS_ERROR : 0);

			MessageStatusFilterChanged(statusMask).dispatch();
		}
	);

	layout->addWidget(checkbox);
	layout->addSpacing(25);

	return checkbox;
}

void QtStatusView::refreshView()
{
	m_refreshFunctor();
}

void QtStatusView::clear()
{
	m_clearFunctor();
}

void QtStatusView::addStatus(const std::vector<Status>& status)
{
	m_addStatusFunctor(status);
}

void QtStatusView::doClear()
{
	if (!m_model->index(0, 0).data(Qt::DisplayRole).toString().isEmpty())
	{
		m_model->removeRows(0, m_model->rowCount());
	}

	m_table->showFirstRow();

	m_status.clear();
}

void QtStatusView::doRefreshView()
{
	setStyleSheet();
}

void QtStatusView::doAddStatus(const std::vector<Status>& status)
{
	for (Status s : status)
	{
		const int rowNumber = m_table->getFilledRowCount();
		if (rowNumber < m_model->rowCount())
		{
			m_model->insertRow(rowNumber);
		}

		QString statusType = (s.type == StatusType::STATUS_ERROR ? "ERROR" : "INFO");
		m_model->setItem(rowNumber, STATUSVIEW_COLUMN::TYPE, new QStandardItem(statusType));
		m_model->setItem(rowNumber, STATUSVIEW_COLUMN::STATUS, new QStandardItem(s.message.c_str()));
	}

	m_table->updateRows();

	if (!m_table->hasSelection())
	{
		m_table->showLastRow();
	}
}

void QtStatusView::setStyleSheet() const
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, ColorScheme::getInstance()->getColor("error/background"));

	QPalette palette(m_showErrors->palette());
	palette.setColor(QPalette::WindowText, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));

	// widget->setStyleSheet(
	// 	utility::getStyleSheet(ResourcePaths::getGuiPath() + "error_view/error_view.css").c_str()
	// );

	m_table->updateRows();
}
