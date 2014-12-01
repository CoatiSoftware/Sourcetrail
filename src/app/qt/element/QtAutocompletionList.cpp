#include "qt/element/QtAutocompletionList.h"

#include <QPainter>
#include <QScrollBar>

QtAutocompletionModel::QtAutocompletionModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

QtAutocompletionModel::~QtAutocompletionModel()
{
}

void QtAutocompletionModel::setMatchList(const std::vector<SearchMatch>& matchList)
{
	m_matchList = matchList;
}

int QtAutocompletionModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_matchList.size();
}

int QtAutocompletionModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 3;
}

QVariant QtAutocompletionModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= int(m_matchList.size()) || role != Qt::DisplayRole)
	{
		return QVariant();
	}

	const SearchMatch& match = m_matchList[index.row()];

	switch (index.column())
	{
	case 0:
		return QString::fromStdString(match.fullName);
	case 1:
		return QString::fromStdString(match.typeName);
	case 2:
		{
			QList<QVariant> indices;
			for (const size_t idx : match.indices)
			{
				indices.push_back(quint64(idx));
			}
			return indices;
		}
	default:
		return QVariant();
	}
}

const SearchMatch* QtAutocompletionModel::getSearchMatchAt(int idx) const
{
	if (idx >= 0 && size_t(idx) < m_matchList.size())
	{
		return &m_matchList[idx];
	}
	return nullptr;
}


QtAutocompletionDelegate::QtAutocompletionDelegate(QObject* parent)
	: QItemDelegate(parent)
{
}

QtAutocompletionDelegate::~QtAutocompletionDelegate()
{
}

void QtAutocompletionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
	}
	else
	{
		painter->fillRect(option.rect, option.palette.color(QPalette::Base));
	}

	QString name = index.data().toString();
	float charWidth = option.fontMetrics.width("FontWidth") / 9.0f;

	QString type = index.sibling(index.row(), index.column() + 1).data().toString();

	QColor color("#FFE47A");
	if (type.size())
	{
		color = QColor("#CC8D91");
	}

	QList<QVariant> indices = index.sibling(index.row(), index.column() + 2).data().toList();
	if (indices.size())
	{
		int idx = 0;
		float x = charWidth + 2;
		for (int i = 0; i < name.size(); i++)
		{
			if (idx < indices.size() && i == indices[idx])
			{
				QRect rect = option.rect.adjusted(x, 2, 0, -1);
				rect.setWidth(charWidth + 1);
				painter->fillRect(rect, color);
				idx++;
			}

			x += charWidth;
		}
	}
	else
	{
		QRect rect = option.rect.adjusted(0, 2, 0, -1);
		rect.setWidth(charWidth - 1);
		painter->fillRect(rect, color);
	}

	painter->drawText(option.rect.adjusted(charWidth + 2, -1, 0, 0), Qt::AlignLeft, name);

	if (type.size())
	{
		QFont font = painter->font();
		if (font.pointSize() > 0)
		{
			QFont typeFont = font;
			typeFont.setPointSize(10);
			painter->setFont(typeFont);
		}

		QPen pen = painter->pen();
		QPen typePen = pen;
		typePen.setColor(QColor("#878787"));
		painter->setPen(typePen);

		painter->drawText(option.rect.adjusted(0, 3, -charWidth, 0), Qt::AlignRight, type);

		painter->setFont(font);
		painter->setPen(pen);
	}
}


QtAutocompletionList::QtAutocompletionList(QWidget* parent)
	: QCompleter(parent)
{
	m_model = std::make_shared<QtAutocompletionModel>(this);
	setModel(m_model.get());

	m_delegate = std::make_shared<QtAutocompletionDelegate>(this);

	QListView* list = new QListView(parent);
	list->setItemDelegateForColumn(0, m_delegate.get());
	list->setObjectName("search_box_popup");
	list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	list->setUniformItemSizes(true);
	setPopup(list);

	setCaseSensitivity(Qt::CaseInsensitive);
	// setCompletionMode(QCompleter::UnfilteredPopupCompletion);
}

QtAutocompletionList::~QtAutocompletionList()
{
}

void QtAutocompletionList::completeAt(const QPoint& pos, const std::vector<SearchMatch>& autocompletionList)
{
	m_model->setMatchList(autocompletionList);

	QSize minSize(300, 253);
	QListView* list = dynamic_cast<QListView*>(popup());

	if (!autocompletionList.size())
	{
		list->hide();
		return;
	}

	setCompletionPrefix("");

	const QModelIndex& index = completionModel()->index(0, 0);
	list->setCurrentIndex(index);

	complete(QRect(pos.x(), pos.y(), minSize.width(), 1));

	QRect rect = list->visualRect(index);
	minSize.setHeight(std::min(minSize.height(), m_model->rowCount(index) * rect.height() + 16));

	list->setMinimumSize(minSize);

	list->verticalScrollBar()->setValue(list->verticalScrollBar()->minimum());
	list->setCurrentIndex(index); // must be set again to avoid flickering

	disconnect(); // must be done because of a bug where signals are no longer received by QtSmartSearchBox
	connect(this, SIGNAL(highlighted(const QModelIndex&)), this, SLOT(onHighlighted(const QModelIndex&)), Qt::DirectConnection);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(onActivated(const QModelIndex&)), Qt::DirectConnection);
}

const SearchMatch* QtAutocompletionList::getSearchMatchAt(int idx) const
{
	return m_model->getSearchMatchAt(idx);
}

void QtAutocompletionList::onHighlighted(const QModelIndex& index)
{
	const SearchMatch* match = getSearchMatchAt(index.row());
	if (match)
	{
		emit matchHighlighted(*match);
	}
}

void QtAutocompletionList::onActivated(const QModelIndex& index)
{
	const SearchMatch* match = getSearchMatchAt(index.row());
	if (match)
	{
		emit matchActivated(*match);
	}
}
