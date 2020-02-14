#include "QtTable.h"

#include <cmath>

#include <QHeaderView>
#include <QLineEdit>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStyledItemDelegate>

#include "ApplicationSettings.h"


class SelectableCellDelegate: public QStyledItemDelegate
{
public:
	SelectableCellDelegate(QObject* parent = Q_NULLPTR);
	QWidget* createEditor(
		QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

SelectableCellDelegate::SelectableCellDelegate(QObject* parent): QStyledItemDelegate(parent) {}

QWidget* SelectableCellDelegate::createEditor(
	QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
	QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
	if (lineEdit != nullptr)
	{
		lineEdit->setReadOnly(true);
	}
	return editor;
}

QtTable::QtTable(QWidget* parent): QTableView(parent), m_rowsToFill(0)
{
	setAlternatingRowColors(true);
	setShowGrid(false);
	setMouseTracking(true);

	setItemDelegate(new SelectableCellDelegate(this));

	verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
	verticalHeader()->setDefaultAlignment(Qt::AlignRight);

	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);

	connect(horizontalHeader(), &QHeaderView::sectionResized, this, &QtTable::columnResized);
}

QtTable::~QtTable() {}

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

	const int rowCount = model()->rowCount() > m_rowsToFill ? model()->rowCount()
															: static_cast<int>(m_rowsToFill);
	const int width = static_cast<int>(
		ApplicationSettings::getInstance()->getFontSize() * 0.7 * int(1 + std::log10(rowCount)));

	verticalHeader()->setStyleSheet("::section { width: " + QString::number(width) + "px; }");
	verticalHeader()->setDefaultSectionSize(ApplicationSettings::getInstance()->getFontSize() + 6);

	if (this->selectionModel()->hasSelection() &&
		selectionModel()->selection().indexes()[0].row() >= model()->rowCount() - 2)
	{
		clearSelection();
	}

	resizeRowsToContents();
}

int QtTable::getFilledRowCount()
{
	for (int i = model()->rowCount() - 1; i >= 0; i--)
	{
		if (!model()->index(i, 0).data(Qt::DisplayRole).toString().isEmpty())
		{
			return i + 1;
		}
	}

	return 0;
}

void QtTable::showFirstRow()
{
	verticalScrollBar()->setValue(verticalScrollBar()->minimum());
}

void QtTable::showLastRow()
{
	if (m_rowsToFill <= getFilledRowCount())
	{
		verticalScrollBar()->setValue(verticalScrollBar()->maximum());
	}
}

bool QtTable::hasSelection() const
{
	return this->selectionModel()->hasSelection();
}

void QtTable::columnResized(int column, int oldWidth, int newWidth)
{
	resizeRowsToContents();
}

void QtTable::resizeEvent(QResizeEvent* event)
{
	QTableView::resizeEvent(event);
	int tableHeight = event->size().height();

	if (this->model()->rowCount() == 0)
	{
		this->model()->insertRow(0);
	}

	m_rowsToFill = 0;
	for (int row = 0; row < this->model()->rowCount(); row++)
	{
		tableHeight -= this->rowHeight(row);
		m_rowsToFill++;

		if (tableHeight <= 0)
		{
			break;
		}
	}

	m_rowsToFill += (float)tableHeight / this->rowHeight(this->model()->rowCount() - 1);

	updateRows();
}

void QtTable::mousePressEvent(QMouseEvent* event)
{
	m_colIndex = -1;
	m_lastPos = -1;

	if (event->button() == Qt::LeftButton)
	{
		for (auto i = 1; i < this->model()->columnCount(); i++)
		{
			if (std::abs(event->pos().x() - columnViewportPosition(i)) < 5)
			{
				m_colIndex = i - 1;
				m_lastPos = event->pos().x();
				break;
			}
		}
	}

	if (m_colIndex == -1)
	{
		QTableView::mousePressEvent(event);
	}
}

void QtTable::mouseMoveEvent(QMouseEvent* event)
{
	if (m_colIndex >= 0)
	{
		int width = columnWidth(m_colIndex) + event->pos().x() - m_lastPos;
		setColumnWidth(m_colIndex, width < 20 ? 20 : width);

		if (width >= 20)
		{
			m_lastPos = event->pos().x();
		}
	}
	else
	{
		bool inRange = false;
		for (auto i = 1; i < this->model()->columnCount(); i++)
		{
			if (std::abs(event->pos().x() - columnViewportPosition(i)) < 5)
			{
				setCursor(Qt::SplitHCursor);
				inRange = true;
				break;
			}
		}

		if (!inRange)
		{
			unsetCursor();
		}

		QTableView::mouseMoveEvent(event);
	}
}

void QtTable::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_colIndex == -1)
	{
		QTableView::mouseReleaseEvent(event);
	}

	m_colIndex = -1;
	m_lastPos = -1;
}
