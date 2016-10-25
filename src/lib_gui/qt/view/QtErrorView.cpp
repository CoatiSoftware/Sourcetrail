#include "qt/view/QtErrorView.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QFrame>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QPalette>
#include <QStandardItemModel>
#include <QStandardItem>

#include "qt/utility/utilityQt.h"
#include "qt/element/QtTable.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"
#include "utility/messaging/type/MessageErrorFilterChanged.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/ResourcePaths.h"

#include "qt/view/QtViewWidgetWrapper.h"

QtErrorView::QtErrorView(ViewLayout* viewLayout)
	: ErrorView(viewLayout)
	, m_clearFunctor(std::bind(&QtErrorView::doClear, this))
	, m_refreshFunctor(std::bind(&QtErrorView::doRefreshView, this))
	, m_addErrorsFunctor(std::bind(&QtErrorView::doAddErrors, this, std::placeholders::_1, std::placeholders::_2))
	, m_setErrorIdFunctor(std::bind(&QtErrorView::doSetErrorId, this, std::placeholders::_1))
	, m_ignoreNextSelection(false)
{
}

QtErrorView::~QtErrorView()
{
}

void QtErrorView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
}

void QtErrorView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 5);
	layout->setSpacing(0);
	widget->setLayout(layout);

	m_table = new QtTable(this);
	m_model = new QStandardItemModel(this);
	m_table->setModel(m_model);

	// Setup Table Headers
	m_model->setColumnCount(6);
	m_table->setColumnWidth(COLUMN::TYPE, 70);
	m_table->setColumnWidth(COLUMN::MESSAGE, 450);
	m_table->setColumnWidth(COLUMN::FILE, 300);
	m_table->setColumnWidth(COLUMN::LINE, 50);
	m_table->setColumnHidden(COLUMN::ID, true);

	QStringList headers;
	headers << "Type" << "Message" << "File" << "Line" << "Indexed";
	m_model->setHorizontalHeaderLabels(headers);

	connect(m_table->selectionModel(), &QItemSelectionModel::currentRowChanged,
		[=](const QModelIndex& index, const QModelIndex& previousIndex)
	{
		if (index.isValid() && !m_ignoreNextSelection)
		{
			if (m_model->item(index.row(), COLUMN::FILE) == nullptr)
			{
				return;
			}

			MessageShowErrors(m_model->item(index.row(), COLUMN::ID)->text().toUInt()).dispatch();
		}

		m_ignoreNextSelection = false;
	});

	layout->addWidget(m_table);

	// Setup Checkboxes
	QBoxLayout* checkboxes = new QHBoxLayout();
	checkboxes->addSpacing(15);

	m_showFatals = createFilterCheckbox("fatals", true, checkboxes);
	m_showErrors = createFilterCheckbox("errors", true, checkboxes);
	m_showNonIndexedFatals = createFilterCheckbox("fatals in non-indexed files", true, checkboxes);
	m_showNonIndexedErrors = createFilterCheckbox("errors in non-indexed files", false, checkboxes);

	checkboxes->addStretch();

	layout->addLayout(checkboxes);

	doRefreshView();
}

void QtErrorView::refreshView()
{
	m_refreshFunctor();
}

void QtErrorView::clear()
{
	m_clearFunctor();
}

void QtErrorView::addErrors(const std::vector<ErrorInfo>& errors, bool scrollTo)
{
	m_addErrorsFunctor(errors, scrollTo);
}

void QtErrorView::setErrorId(Id errorId)
{
	m_setErrorIdFunctor(errorId);
}

void QtErrorView::doRefreshView()
{
	setStyleSheet();
}

void QtErrorView::doClear()
{
	if (!m_model->index(0, 0).data(Qt::DisplayRole).toString().isEmpty())
	{
		m_model->removeRows(0, m_model->rowCount());
	}

	m_errors.clear();
}

void QtErrorView::doAddErrors(const std::vector<ErrorInfo>& errors, bool scrollTo)
{
	for (const ErrorInfo& error : errors)
	{
		m_errors.push_back(error);
		addErrorToTable(error);
	}

	if (scrollTo)
	{
		m_table->showLastRow();
	}
}

void QtErrorView::doSetErrorId(Id errorId)
{
	QList<QStandardItem*> items = m_model->findItems(QString::number(errorId), Qt::MatchExactly, COLUMN::ID);

	if (items.size() == 1)
	{
		m_ignoreNextSelection = true;
		m_table->selectRow(items.at(0)->row());
	}
}

void QtErrorView::setStyleSheet() const
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);
	utility::setWidgetBackgroundColor(widget, ColorScheme::getInstance()->getColor("error/background"));


	QPalette palette(m_showErrors->palette());
	palette.setColor(QPalette::WindowText, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));
	//palette.setColor(QPalette::Text, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));
	//palette.setColor(QPalette::ButtonText, QColor(ColorScheme::getInstance()->getColor("error/text/normal").c_str()));

	//m_showErrors->setAutoFillBackground(true);
	m_showErrors->setPalette(palette);
	m_showFatals->setPalette(palette);
	m_showNonIndexedErrors->setPalette(palette);
	m_showNonIndexedFatals->setPalette(palette);

	widget->setStyleSheet(
		utility::getStyleSheet(ResourcePaths::getGuiPath() + "error_view/error_view.css").c_str()
	);

	m_table->updateRows();
}

void QtErrorView::addErrorToTable(const ErrorInfo& error)
{
	if (!isShownError(error))
	{
		return;
	}

	int rowNumber = m_table->getFilledRowCount();
	if (rowNumber < m_model->rowCount())
	{
		m_model->insertRow(rowNumber);
	}

	m_model->setItem(rowNumber, COLUMN::TYPE, new QStandardItem(error.fatal ? "FATAL" : "ERROR"));
	m_model->item(rowNumber, COLUMN::TYPE)->setForeground(QBrush(Qt::red));
	m_model->item(rowNumber, COLUMN::TYPE)->setTextAlignment(Qt::AlignCenter);
	m_model->setItem(rowNumber, COLUMN::MESSAGE, new QStandardItem(error.message.c_str()));
	std::string errorPngPath = ResourcePaths::getGuiPath() + "/indexing_dialog/error.png";
	m_model->item(rowNumber, COLUMN::MESSAGE)->setIcon(QIcon(QString(errorPngPath.c_str())));
	m_model->setItem(rowNumber, COLUMN::FILE, new QStandardItem(error.filePath.str().c_str()));
	m_model->item(rowNumber, COLUMN::FILE)->setToolTip(error.filePath.str().c_str());
	m_model->setItem(rowNumber, COLUMN::LINE, new QStandardItem(QString::number(error.lineNumber)));
	m_model->setItem(rowNumber, COLUMN::INDEXED, new QStandardItem(error.indexed ? "yes" : "no"));
	m_model->setItem(rowNumber, COLUMN::ID, new QStandardItem(QString::number(error.id)));
	m_table->updateRows();
}

QCheckBox* QtErrorView::createFilterCheckbox(const QString& name, bool checked, QBoxLayout* layout)
{
	QCheckBox* checkbox = new QCheckBox(name);
	checkbox->setChecked(checked);

	connect(checkbox, &QCheckBox::stateChanged,
		[=](int)
		{
			m_table->selectionModel()->clearSelection();

			ErrorFilter filter;
			filter.error = m_showErrors->checkState() == Qt::Checked;
			filter.fatal = m_showFatals->checkState() == Qt::Checked;
			filter.unindexedError = m_showNonIndexedErrors->checkState() == Qt::Checked;
			filter.unindexedFatal = m_showNonIndexedFatals->checkState() == Qt::Checked;

			MessageErrorFilterChanged(filter).dispatch();
		}
	);

	layout->addWidget(checkbox);
	layout->addSpacing(25);

	return checkbox;
}

bool QtErrorView::isShownError(const ErrorInfo& error)
{
	if (!error.fatal && error.indexed && m_showErrors->checkState() == Qt::Checked)
	{
		return true;
	}
	if (error.fatal && error.indexed && m_showFatals->checkState() == Qt::Checked)
	{
		return true;
	}
	if (!error.fatal && !error.indexed && m_showNonIndexedErrors->checkState() == Qt::Checked)
	{
		return true;
	}
	if (error.fatal && !error.indexed && m_showNonIndexedFatals->checkState() == Qt::Checked)
	{
		return true;
	}
	return false;
}
