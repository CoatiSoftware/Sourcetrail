#include "qt/element/QtTable.h"

#include <cmath>

#include <QHeaderView>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QLineEdit>

#include "settings/ApplicationSettings.h"


class SelectableCellDelegate : public QStyledItemDelegate
{
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


QWidget* SelectableCellDelegate::createEditor(
	QWidget* parent,
	const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
	QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
	if (lineEdit != nullptr)
	{
		lineEdit->setReadOnly(true);
	}
	return editor;
}

QtTable::QtTable(QWidget* parent)
	: QTableView(parent)
	, m_rowsToFill(0)
{
	setAlternatingRowColors(true);
	setShowGrid(false);

	this->setItemDelegate(new SelectableCellDelegate());

	verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
	verticalHeader()->setDefaultAlignment(Qt::AlignRight);

	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

QtTable::~QtTable()
{
}

void QtTable::updateRows()
{
	while (model()->rowCount() <= m_rowsToFill)
	{
		model()->insertRow(model()->rowCount());
	}

	while (model()->rowCount() > m_rowsToFill + 1)
	{
		int row = model()->rowCount() - 1;
		if (model()->index(row, 0).data(Qt::DisplayRole).toString().isEmpty())
		{
			model()->removeRow(row);
		}
		else
		{
			break;
		}
	}

	int rowCount = model()->rowCount() > m_rowsToFill ? model()->rowCount() : m_rowsToFill;
	int width = ApplicationSettings::getInstance()->getFontSize() * 0.7 * int(1 + std::log10(rowCount));

	verticalHeader()->setStyleSheet("::section { width: " + QString::number(width) + "px; }");
	verticalHeader()->setDefaultSectionSize(ApplicationSettings::getInstance()->getFontSize() + 6);
}

int QtTable::getFilledRowCount()
{
	for (int i = 0; i < model()->rowCount(); i++)
	{
		if (model()->index(i, 0).data(Qt::DisplayRole).toString().isEmpty())
		{
			return i;
		}
	}

	return model()->rowCount();
}

void QtTable::showLastRow()
{
	if (m_rowsToFill <= getFilledRowCount())
	{
		verticalScrollBar()->setValue(verticalScrollBar()->maximum());
	}
}

void QtTable::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);
	int tableHeight = event->size().height();

	if (this->model()->rowCount() == 0)
	{
		this->model()->insertRow(0);
	}

	m_rowsToFill = (float)tableHeight / this->rowHeight(0);

	updateRows();
}
