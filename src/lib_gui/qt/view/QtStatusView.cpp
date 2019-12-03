#include "QtStatusView.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QFrame>
#include <QPushButton>
#include <QStandardItemModel>

#include "ApplicationSettings.h"
#include "ColorScheme.h"
#include "MessageClearStatusView.h"
#include "MessageStatusFilterChanged.h"
#include "QtTable.h"
#include "QtViewWidgetWrapper.h"
#include "utilityQt.h"

QtStatusView::QtStatusView(ViewLayout* viewLayout): StatusView(viewLayout)
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));

	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	m_table = new QtTable(this);
	m_model = new QStandardItemModel(this);
	m_table->setModel(m_model);

	m_model->setColumnCount(2);
	m_table->setColumnWidth(STATUSVIEW_COLUMN::TYPE, 100);
	// m_table->setColumnWidth(STATUSVIEW_COLUMN::STATUS, 150);

	QStringList headers;
	headers << "Type"
			<< "Message";
	m_model->setHorizontalHeaderLabels(headers);

	layout->addWidget(m_table);

	// Setup filters
	QHBoxLayout* filters = new QHBoxLayout();
	filters->setContentsMargins(10, 3, 0, 3);
	filters->setSpacing(25);

	const StatusFilter filter = ApplicationSettings::getInstance()->getStatusFilter();
	m_showInfo = createFilterCheckbox("Info", filters, filter & StatusType::STATUS_INFO);
	m_showErrors = createFilterCheckbox("Error", filters, filter & StatusType::STATUS_ERROR);

	filters->addStretch();

	QPushButton* clearButton = new QPushButton("Clear Table");
	clearButton->setObjectName("screen_button");
	connect(clearButton, &QPushButton::clicked, [=]() { MessageClearStatusView().dispatch(); });

	filters->addWidget(clearButton);
	filters->addSpacing(10);

	layout->addLayout(filters);
}

void QtStatusView::createWidgetWrapper() {}

void QtStatusView::refreshView()
{
	m_onQtThread([this]() {
		QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
		utility::setWidgetBackgroundColor(
			widget, ColorScheme::getInstance()->getColor("window/background"));

		QPalette palette(m_showErrors->palette());
		palette.setColor(
			QPalette::WindowText,
			QColor(ColorScheme::getInstance()->getColor("table/text/normal").c_str()));

		m_showErrors->setPalette(palette);
		m_showInfo->setPalette(palette);

		m_table->updateRows();
	});
}

void QtStatusView::clear()
{
	m_onQtThread([this]() {
		if (!m_model->index(0, 0).data(Qt::DisplayRole).toString().isEmpty())
		{
			m_model->removeRows(0, m_model->rowCount());
		}

		m_table->showFirstRow();

		m_status.clear();
	});
}

void QtStatusView::addStatus(const std::vector<Status>& status)
{
	m_onQtThread([=]() {
		for (const Status& s: status)
		{
			const int rowNumber = m_table->getFilledRowCount();
			if (rowNumber < m_model->rowCount())
			{
				m_model->insertRow(rowNumber);
			}

			QString statusType = (s.type == StatusType::STATUS_ERROR ? "ERROR" : "INFO");
			m_model->setItem(rowNumber, STATUSVIEW_COLUMN::TYPE, new QStandardItem(statusType));
			m_model->setItem(
				rowNumber,
				STATUSVIEW_COLUMN::STATUS,
				new QStandardItem(QString::fromStdWString(s.message)));

			if (s.type == StatusType::STATUS_ERROR)
			{
				m_model->item(rowNumber, STATUSVIEW_COLUMN::TYPE)->setForeground(QBrush(Qt::red));
			}
		}

		m_table->updateRows();

		if (!m_table->hasSelection())
		{
			m_table->showLastRow();
		}
	});
}

QCheckBox* QtStatusView::createFilterCheckbox(const QString& name, QBoxLayout* layout, bool checked)
{
	QCheckBox* checkbox = new QCheckBox(name);
	checkbox->setChecked(checked);

	connect(checkbox, &QCheckBox::stateChanged, [=](int) {
		m_table->selectionModel()->clearSelection();

		const StatusFilter statusMask = (m_showInfo->isChecked() ? StatusType::STATUS_INFO : 0) +
			(m_showErrors->isChecked() ? StatusType::STATUS_ERROR : 0);

		MessageStatusFilterChanged(statusMask).dispatch();
	});

	layout->addWidget(checkbox);

	return checkbox;
}
