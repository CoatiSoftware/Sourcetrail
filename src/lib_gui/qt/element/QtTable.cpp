#include "qt/element/QtTable.h"

#include <QResizeEvent>

QtTable::QtTable(QWidget* parent)
	: QTableView(parent)
	, m_rowsToFill(0)
{
}

QtTable::~QtTable()
{
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
