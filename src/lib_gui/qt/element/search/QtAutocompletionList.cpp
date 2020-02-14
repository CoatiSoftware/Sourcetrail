#include "QtAutocompletionList.h"

#include <QPainter>
#include <QScrollBar>

#include "ApplicationSettings.h"
#include "ColorScheme.h"
#include "GraphViewStyle.h"
#include "QtDeviceScaledPixmap.h"
#include "ResourcePaths.h"
#include "utilityString.h"

QtAutocompletionModel::QtAutocompletionModel(QObject* parent): QAbstractTableModel(parent) {}

QtAutocompletionModel::~QtAutocompletionModel() {}

void QtAutocompletionModel::setMatchList(const std::vector<SearchMatch>& matchList)
{
	size_t rowCount = std::max(m_matchList.size(), matchList.size());
	m_matchList = matchList;
	emit dataChanged(index(0, 0), index(static_cast<int>(rowCount - 1), 5));
}

int QtAutocompletionModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return static_cast<int>(m_matchList.size());
}

int QtAutocompletionModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 6;
}

QVariant QtAutocompletionModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= int(m_matchList.size()) ||
		role != Qt::DisplayRole)
	{
		return QVariant();
	}

	const SearchMatch& match = m_matchList[index.row()];

	switch (index.column())
	{
	case 0:
		return QString::fromStdWString(match.name);
	case 1:
		return QString::fromStdWString(match.text);
	case 2:
		return QString::fromStdWString(match.subtext);
	case 3:
		return QString::fromStdWString(match.typeName);
	case 4:
	{
		QList<QVariant> indices;
		for (const size_t idx: match.indices)
		{
			indices.push_back(quint64(idx));
		}
		return indices;
	}
	case 5:
		return match.nodeType.getKind();
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

QString QtAutocompletionModel::longestText() const
{
	std::wstring str;
	for (const SearchMatch& match: m_matchList)
	{
		if (match.text.size() > str.size())
		{
			str = match.text;
		}
	}
	return QString::fromStdWString(str);
}

QString QtAutocompletionModel::longestSubText() const
{
	std::wstring str;
	for (const SearchMatch& match: m_matchList)
	{
		if (match.subtext.size() > str.size())
		{
			str = match.subtext;
		}
	}
	return QString::fromStdWString(str);
}

QString QtAutocompletionModel::longestType() const
{
	std::wstring str;
	for (const SearchMatch& match: m_matchList)
	{
		if (match.typeName.size() > str.size())
		{
			str = match.typeName;
		}
	}
	return QString::fromStdWString(str);
}


QtAutocompletionDelegate::QtAutocompletionDelegate(QtAutocompletionModel* model, QObject* parent)
	: QStyledItemDelegate(parent), m_model(model)
{
	resetCharSizes();
}

QtAutocompletionDelegate::~QtAutocompletionDelegate() {}

void QtAutocompletionDelegate::paint(
	QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();

	// get data
	QString name = index.data().toString();
	QString text = index.sibling(index.row(), index.column() + 1).data().toString();
	QString subtext = index.sibling(index.row(), index.column() + 2).data().toString();
	QString type = index.sibling(index.row(), index.column() + 3).data().toString();
	QList<QVariant> indices = index.sibling(index.row(), index.column() + 4).data().toList();
	NodeType nodeType(intToNodeKind(index.sibling(index.row(), index.column() + 5).data().toInt()));

	// define highlight colors
	ColorScheme* scheme = ColorScheme::getInstance().get();
	QColor fillColor(0xFF, 0xFF, 0xFF);
	QColor textColor(0, 0, 0);

	if (type.size() && type != QLatin1String("command") && type != QLatin1String("filter"))
	{
		const GraphViewStyle::NodeColor& nodeColor = GraphViewStyle::getNodeColor(
			nodeType.getUnderscoredTypeString(), false);
		fillColor = QColor(nodeColor.fill.c_str());
		textColor = QColor(nodeColor.text.c_str());
	}
	else
	{
		fillColor = QColor(scheme
							   ->getSearchTypeColor(
								   utility::encodeToUtf8(
									   SearchMatch::getSearchTypeName(SearchMatch::SEARCH_COMMAND)),
								   "fill")
							   .c_str());
		textColor = QColor(scheme
							   ->getSearchTypeColor(
								   utility::encodeToUtf8(
									   SearchMatch::getSearchTypeName(SearchMatch::SEARCH_COMMAND)),
								   "text")
							   .c_str());
	}

	int top1 = 6;
	int top2 = static_cast<int>(m_charHeight1 + 3);

	// draw background
	QColor backgroundColor = option.palette.color(
		(option.state & QStyle::State_Selected ? QPalette::Highlight : QPalette::Base));
	painter->fillRect(option.rect, backgroundColor);

	// draw highlights at indices
	QString highlightText(text.size(), QChar(' '));
	if (!indices.empty())
	{
		for (int i = 0; i < indices.size(); i++)
		{
			int idx = indices[i].toInt() - (name.size() - text.size());
			if (idx < 0)
			{
				continue;
			}

			QRect rect(
				static_cast<int>(option.rect.left() + m_charWidth1 * (idx + 1) + 2),
				option.rect.top() + top1 - 1,
				static_cast<int>(m_charWidth1 + 1),
				static_cast<int>(m_charHeight1 - 1));
			painter->fillRect(rect, fillColor);

			highlightText[idx] = text.at(idx);
			text[idx] = QChar(' ');
		}
	}
	else
	{
		QRect rect(
			option.rect.left(),
			option.rect.top() + top1,
			static_cast<int>(m_charWidth1 - 1),
			static_cast<int>(m_charHeight1 - 2));
		painter->fillRect(rect, fillColor);
	}

	// draw text normal
	painter->drawText(
		option.rect.adjusted(static_cast<int>(m_charWidth1 + 2), top1 - 3, 0, 0), Qt::AlignLeft, text);

	// draw text highlighted
	painter->save();
	QPen highlightPen = painter->pen();
	highlightPen.setColor(textColor);
	painter->setPen(highlightPen);
	painter->drawText(
		option.rect.adjusted(static_cast<int>(m_charWidth1 + 2), top1 - 3, 0, 0),
		Qt::AlignLeft,
		highlightText);
	painter->restore();

	// draw subtext
	if (subtext.size())
	{
		// draw arrow icon
		painter->drawPixmap(
			static_cast<int>(option.rect.left() + m_charWidth2 * 2),
			static_cast<int>(option.rect.top() + top2 + 1 + (m_charHeight2 - m_arrow.height()) / 2),
			m_arrow.pixmap());

		painter->setFont(m_font2);

		QString highlightSubtext(subtext.size(), QChar(' '));
		if (indices.size())
		{
			for (int i = 0; i < indices.size(); i++)
			{
				int idx = indices[i].toInt();
				if (idx >= subtext.size())
				{
					continue;
				}

				QRect rect(
					static_cast<int>(option.rect.left() + m_charWidth2 * (idx + 3) + 2),
					option.rect.top() + top2 + 1,
					static_cast<int>(m_charWidth2 + 1),
					static_cast<int>(m_charHeight2));
				painter->fillRect(rect, fillColor);

				highlightSubtext[idx] = subtext.at(idx);
				subtext[idx] = QChar(' ');
			}
		}

		QPen typePen = painter->pen();
		typePen.setColor(scheme->getColor("search/popup/by_text").c_str());
		painter->setPen(typePen);

		// draw subtext normal
		painter->drawText(
			option.rect.adjusted(static_cast<int>((3 * m_charWidth2) + 2), top2, 0, 0),
			Qt::AlignLeft,
			subtext);

		// draw subtext highlighted
		painter->save();
		QPen highlightPen = painter->pen();
		highlightPen.setColor(textColor);
		painter->setPen(highlightPen);
		painter->drawText(
			option.rect.adjusted(static_cast<int>((3 * m_charWidth2) + 2), top2, 0, 0),
			Qt::AlignLeft,
			highlightSubtext);
		painter->restore();
	}

	// draw type
	if (type.size())
	{
		painter->setFont(m_font2);

		QPen typePen = painter->pen();
		typePen.setColor(scheme->getColor("search/popup/by_text").c_str());
		painter->setPen(typePen);

		int width = static_cast<int>(m_charWidth2 * type.size());
		int x = static_cast<int>(painter->viewport().right() - width - m_charWidth2);
		int y = option.rect.top() + top2;

		painter->fillRect(
			QRect(
				static_cast<int>(x - m_charWidth2),
				y,
				static_cast<int>(width + m_charWidth2 * 3),
				static_cast<int>(m_charHeight2 + 2)),
			backgroundColor);
		painter->drawText(
			QRect(x, y, static_cast<int>(width + m_charWidth2), static_cast<int>(m_charHeight2)),
			Qt::AlignRight,
			type);
	}

	// draw bottom line
	QRect rect(0, option.rect.bottom(), option.rect.width(), 1);
	painter->fillRect(rect, scheme->getColor("search/popup/line").c_str());

	painter->restore();
}

QSize QtAutocompletionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const_cast<QtAutocompletionDelegate*>(this)->calculateCharSizes(option.font);

	QString text = m_model->longestText();
	QString subtext = m_model->longestSubText();
	QString type = m_model->longestType();

	return QSize(
		static_cast<int>(std::max(
			(text.size() + 2) * m_charWidth1, (subtext.size() + type.size() + 6) * m_charWidth2)),
		static_cast<int>(m_charHeight1 * 2 + 3));
}

void QtAutocompletionDelegate::calculateCharSizes(QFont font)
{
	if (m_charWidth1 > 0)
	{
		return;
	}

	m_font1 = font;

	QFontMetrics metrics1(font);
	m_charWidth1 = metrics1.width(QStringLiteral(
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------")) /
		500.0f;
	m_charHeight1 = static_cast<float>(metrics1.height());

	font.setPixelSize(ApplicationSettings::getInstance()->getFontSize() - 3);
	m_font2 = font;

	QFontMetrics metrics2(font);
	m_charWidth2 = metrics2.width(QStringLiteral(
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------"
					   "---------------------------------------------------------------------------"
					   "-------------------------")) /
		500.0f;
	m_charHeight2 = static_cast<float>(metrics2.height());

	m_arrow = QtDeviceScaledPixmap(
		QString::fromStdString(ResourcePaths::getGuiPath().str() + "search_view/images/arrow.png"));
	m_arrow.scaleToWidth(static_cast<int>(m_charWidth2));
	m_arrow.colorize(ColorScheme::getInstance()->getColor("search/popup/by_text").c_str());
}

void QtAutocompletionDelegate::resetCharSizes()
{
	m_charWidth1 = m_charHeight1 = m_charWidth2 = m_charHeight2 = 0.0f;
}


QtAutocompletionList::QtAutocompletionList(QWidget* parent): QCompleter(parent)
{
	m_model = std::make_shared<QtAutocompletionModel>(this);
	setModel(m_model.get());

	m_delegate = std::make_shared<QtAutocompletionDelegate>(m_model.get(), this);

	QListView* list = new QListView(parent);
	list->setItemDelegateForColumn(0, m_delegate.get());
	list->setObjectName(QStringLiteral("search_box_popup"));
	list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	list->setUniformItemSizes(true);
	setPopup(list);

	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	setModelSorting(QCompleter::UnsortedModel);
	setCompletionPrefix(QLatin1String(""));
	setMaxVisibleItems(8);
}

QtAutocompletionList::~QtAutocompletionList() {}

void QtAutocompletionList::completeAt(QPoint pos, const std::vector<SearchMatch>& autocompletionList)
{
	m_model->setMatchList(autocompletionList);

	QListView* list = dynamic_cast<QListView*>(popup());
	if (!autocompletionList.size())
	{
		list->hide();
		return;
	}

	if (!list->isVisible())
	{
		// this block fixes an issue where the autocompletion list becomes invisible after some time
		// on Windows
		// TODO: try to find out which line actually fixes the issue
		m_delegate = std::make_shared<QtAutocompletionDelegate>(m_model.get(), this);
		list->setItemDelegateForColumn(0, m_delegate.get());
		setPopup(list);
		list->show();
	}

	disconnect();	 // must be done because of a bug where signals are no longer received by
					 // QtSmartSearchBox
	connect(
		this,
		static_cast<void (QtAutocompletionList::*)(const QModelIndex&)>(
			&QtAutocompletionList::highlighted),
		this,
		&QtAutocompletionList::onHighlighted,
		Qt::DirectConnection);
	connect(
		this,
		static_cast<void (QtAutocompletionList::*)(const QModelIndex&)>(
			&QtAutocompletionList::activated),
		this,
		&QtAutocompletionList::onActivated,
		Qt::DirectConnection);

	complete(QRect(pos.x(), pos.y(), std::max(dynamic_cast<QWidget*>(parent())->width(), 400), 1));

	list->setCurrentIndex(completionModel()->index(0, 0));
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
