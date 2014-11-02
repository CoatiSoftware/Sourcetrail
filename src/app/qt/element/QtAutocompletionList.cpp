#include "qt/element/QtAutocompletionList.h"

#include <QPainter>

QtAutocompletionModel::QtAutocompletionModel(const std::vector<SearchMatch>& matchList, QObject* parent)
	: QAbstractTableModel(parent)
	, m_matchList(matchList)
{
}

QtAutocompletionModel::~QtAutocompletionModel()
{
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
	if (!index.isValid() || index.row() < 0 || size_t(index.row()) >= m_matchList.size() || role != Qt::DisplayRole)
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
	QPen pen = painter->pen();
	QFont font = painter->font();

	if (option.state & QStyle::State_Selected)
	{
		QPen highlightPen = pen;
		highlightPen.setColor(option.palette.color(QPalette::HighlightedText));
		painter->setPen(highlightPen);

		painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
	}
	else
	{
		painter->fillRect(option.rect, option.palette.color(QPalette::Base));
	}

	QFont higlightFont = font;
	higlightFont.setWeight(QFont::Bold);

	QString name = index.data().toString();
	QList<QVariant> indices = index.sibling(index.row(), index.column() + 2).data().toList();
	int idx = 0;

	int x = 0;
	int m = option.fontMetrics.width(QLatin1Char('9'));
	for (int i = 0; i < name.size(); i++)
	{
		if (idx < indices.size() && i == indices[idx])
		{
			painter->setFont(higlightFont);
			idx++;
		}
		else
		{
			painter->setFont(font);
		}

		painter->drawText(option.rect.adjusted(8 + x, 0, 0, 0), Qt::AlignLeft, name.at(i));
		x += m;
	}

	if (font.pixelSize() > 0)
	{
		QFont typeFont = font;
		typeFont.setPixelSize(0.8f * font.pixelSize());
		painter->setFont(typeFont);
	}

	QString type = index.sibling(index.row(), index.column() + 1).data().toString();
	QRect rect = option.rect.adjusted(1, 1, 0, -1);
	rect.setWidth(5);

	if (type.size())
	{
		painter->fillRect(rect, QColor(153, 22, 165));
		painter->drawText(option.rect.adjusted(0, 0, -3, 0), Qt::AlignRight, type);
	}
	else
	{
		painter->fillRect(rect, QColor(172, 150, 0));
	}

	painter->setFont(font);
	painter->setPen(pen);
}


QtAutocompletionList::QtAutocompletionList(const std::vector<SearchMatch>& autocompletionList, QWidget* parent)
	: QCompleter(parent)
{
	m_model = std::make_shared<QtAutocompletionModel>(autocompletionList, this);
	setModel(m_model.get());

	m_delegate = std::make_shared<QtAutocompletionDelegate>(this);

	QListView* list = new QListView(parent);
	list->setItemDelegateForColumn(0, m_delegate.get());
	list->setObjectName("search_box_popup");
	setPopup(list);

	setCaseSensitivity(Qt::CaseInsensitive);

	connect(this, SIGNAL(highlighted(const QModelIndex&)), this, SLOT(onHighlighted(const QModelIndex&)), Qt::DirectConnection);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(onActivated(const QModelIndex&)), Qt::DirectConnection);
}

QtAutocompletionList::~QtAutocompletionList()
{
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
