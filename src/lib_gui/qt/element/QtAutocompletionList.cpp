#include "qt/element/QtAutocompletionList.h"

#include <QPainter>
#include <QScrollBar>

#include "component/view/GraphViewStyle.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

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
	return 4;
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
		return QString::fromStdString(match.getFullName());
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
	case 3:
		return match.nodeType;
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
	: QStyledItemDelegate(parent)
{
}

QtAutocompletionDelegate::~QtAutocompletionDelegate()
{
}

void QtAutocompletionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    ColorScheme* scheme = ColorScheme::getInstance().get();

	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
	}
	else
	{
		painter->fillRect(option.rect, option.palette.color(QPalette::Base));
	}

	QString name = index.data().toString();


	QString type = index.sibling(index.row(), index.column() + 1).data().toString();
	QColor color("#FFFFFF");
	QColor textColor("#000000");

	Node::NodeType nodeType = static_cast<Node::NodeType>(index.sibling(index.row(), index.column() + 3).data().toInt());
	if (type.size() && type != "command")
	{
		const GraphViewStyle::NodeColor& nodeColor = GraphViewStyle::getNodeColor(Node::getTypeString(nodeType), false);
		color = QColor(nodeColor.fill.c_str());
		textColor = QColor(nodeColor.text.c_str());
	}
	else
	{
		color = QColor(scheme->getSearchTypeColor(SearchMatch::getSearchTypeName(SearchMatch::SEARCH_COMMAND), "fill").c_str());
		textColor = QColor(scheme->getSearchTypeColor(SearchMatch::getSearchTypeName(SearchMatch::SEARCH_COMMAND), "text").c_str());
	}

	float charWidth = option.fontMetrics.width(
		"----------------------------------------------------------------------------------------------------"
	) / 100.0f;
	painter->drawText(option.rect.adjusted(charWidth + 2, -1, 0, 0), Qt::AlignLeft, name);

	QString highlightName(name.size(), ' ');

	QList<QVariant> indices = index.sibling(index.row(), index.column() + 2).data().toList();
	if (indices.size())
	{
		for (int i = 0; i < indices.size(); i++)
		{
			int idx = indices[i].toInt();

			QRect rect = option.rect.adjusted(charWidth * (idx + 1) + 1, 2, 0, -1);
			rect.setWidth(charWidth + 2);
			painter->fillRect(rect, color);

			highlightName[idx] = name.at(idx);
		}
	}
	else
	{
		QRect rect = option.rect.adjusted(0, 2, 0, -1);
		rect.setWidth(charWidth - 1);
		painter->fillRect(rect, color);
	}

	painter->save();
	QPen highlightPen = painter->pen();
	highlightPen.setColor(textColor);
	painter->setPen(highlightPen);
	painter->drawText(option.rect.adjusted(charWidth + 2, -1, 0, 0), Qt::AlignLeft, highlightName);
	painter->restore();

	if (type.size())
	{
		QFont font = painter->font();
		if (font.pointSize() > 0)
		{
			QFont typeFont = font;
			typeFont.setPointSize(ApplicationSettings::getInstance()->getFontSize() - 4);
			painter->setFont(typeFont);
		}

		QPen typePen = painter->pen();
		typePen.setColor(scheme->getColor("search/popup/by_text").c_str());
		painter->setPen(typePen);

		painter->drawText(option.rect.adjusted(0, 3, -charWidth, 0), Qt::AlignRight, type);
	}

	painter->restore();
}

QSize QtAutocompletionDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QString name = index.data().toString();
	QString type = index.sibling(index.row(), index.column() + 1).data().toString();
	return QSize( option.fontMetrics.width(name+type)+5, option.fontMetrics.height());
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

	QSize minSize(400, 253);
	QListView* list = dynamic_cast<QListView*>(popup());

	if (!autocompletionList.size())
	{
		list->hide();
		return;
	}

	setCompletionPrefix("");

	const QModelIndex& index = completionModel()->index(0, 0);
	list->setCurrentIndex(index);

	QRect rect = list->visualRect(index);
	minSize.setHeight(std::min(minSize.height(), m_model->rowCount(index) * rect.height() + 16));

	list->setMinimumSize(minSize);
	list->verticalScrollBar()->setValue(list->verticalScrollBar()->minimum());

	disconnect(); // must be done because of a bug where signals are no longer received by QtSmartSearchBox
	connect(this, SIGNAL(highlighted(const QModelIndex&)), this, SLOT(onHighlighted(const QModelIndex&)), Qt::DirectConnection);
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(onActivated(const QModelIndex&)), Qt::DirectConnection);

	QWidget* textBox = dynamic_cast<QWidget*>(parent());
	complete(QRect(pos.x(), pos.y(), textBox->width(), 1));
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
