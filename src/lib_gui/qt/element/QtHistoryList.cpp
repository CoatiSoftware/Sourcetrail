#include "QtHistoryList.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollBar>

#include "utility/messaging/type/MessageToUndoRedoPosition.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityString.h"

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"

#include "component/view/GraphViewStyle.h"
#include "data/search/SearchMatch.h"
#include "settings/ColorScheme.h"

QtHistoryItem::QtHistoryItem(const SearchMatch& match, size_t index, bool isCurrent)
	: index(index)
{
	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);

	std::string name = utility::elide(match.nodeType.isFile() ? match.text : match.name, utility::ELIDE_RIGHT, 100);

	m_name = new QLabel(name.c_str(), this);
	m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_name->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_name->setObjectName(isCurrent ? "history_item_current" : "history_item");
	m_name->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	layout->addWidget(m_name);

	setLayout(layout);

	QSize size = getSizeHint();

	m_indicator = new QWidget(m_name);
	m_indicator->setGeometry(0, 1, 12, size.height());
	m_indicator->show();

	ColorScheme* scheme = ColorScheme::getInstance().get();
	if (match.searchType == SearchMatch::SEARCH_TOKEN)
	{
		m_indicatorColor = GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), false).fill;
		m_indicatorHoverColor = GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), true).fill;
	}
	else
	{
		m_indicatorColor = scheme->getSearchTypeColor(match.getSearchTypeName(), "fill");
		m_indicatorHoverColor = scheme->getSearchTypeColor(match.getSearchTypeName(), "fill", "hover");
	}

	std::stringstream css;
	css << "QWidget { background-color:" << m_indicatorColor << ";}";
	m_indicator->setStyleSheet(css.str().c_str());

	if (isCurrent)
	{
		QtDeviceScaledPixmap pixmap(QString::fromStdString(ResourcePaths::getGuiPath().str() + "history_list/images/arrow.png"));
		pixmap.scaleToHeight(size.height() / 3);

		QLabel* arrow = new QLabel(this);
		arrow->setPixmap(utility::colorizePixmap(pixmap.pixmap(), QColor(scheme->getColor("search/popup/text").c_str())));
		arrow->setGeometry(15, size.height() / 3, size.height() / 3, size.height() / 3);
		arrow->show();
	}
}

QSize QtHistoryItem::getSizeHint() const
{
	return QSize(m_name->fontMetrics().width(m_name->text()) + 40, m_name->fontMetrics().height() + 8);
}

void QtHistoryItem::enterEvent(QEvent *event)
{
	QWidget::enterEvent(event);

	std::stringstream css;
	css << "QWidget { background-color:" << m_indicatorHoverColor << ";}";
	m_indicator->setStyleSheet(css.str().c_str());

	if (m_name->objectName() != "history_item")
	{
		return;
	}

	QFont f(m_name->font());
	f.setBold(true);
	m_name->setFont(f);
}

void QtHistoryItem::leaveEvent(QEvent *event)
{
	QWidget::leaveEvent(event);

	std::stringstream css;
	css << "QWidget { background-color:" << m_indicatorColor << ";}";
	m_indicator->setStyleSheet(css.str().c_str());

	if (m_name->objectName() != "history_item")
	{
		return;
	}

	QFont f(m_name->font());
	f.setBold(false);
	m_name->setFont(f);
}


QtHistoryList::QtHistoryList(const std::vector<SearchMatch>& history, size_t currentIndex)
	: m_currentIndex(currentIndex)
{
	setWindowFlags(Qt::Popup);
	setObjectName("history");

	m_list = new QListWidget(this);
	m_list->setObjectName("history_list");
	m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (size_t i = 0; i < history.size(); i++)
	{
		QListWidgetItem *item = new QListWidgetItem(m_list);
		QtHistoryItem* line = new QtHistoryItem(history[i], i, i == currentIndex);
		item->setSizeHint(line->getSizeHint());
		m_list->setItemWidget(item, line);
	}

	setStyleSheet(utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(FilePath("history_list/history_list.css"))).c_str());

	connect(m_list, &QListWidget::itemClicked, this, &QtHistoryList::onItemClicked);
}

void QtHistoryList::showPopup(QPoint pos)
{
	QSize size(50, 2);
	for (int i = 0; i < m_list->count(); i++)
	{
		QtHistoryItem* it = dynamic_cast<QtHistoryItem*>(m_list->itemWidget(m_list->item(i)));
		QSize itemSize = it->getSizeHint();
		if (itemSize.width() > size.width())
		{
			size.setWidth(itemSize.width());
		}

		size.setHeight(size.height() + m_list->item(i)->sizeHint().height());
	}

	m_list->setGeometry(0, 0, size.width() + 15, std::min(size.height(), 600));
	setGeometry(pos.x(), pos.y(), m_list->size().width(), m_list->size().height());
	show();
}

void QtHistoryList::closeEvent(QCloseEvent* event)
{
	emit closed();
}

void QtHistoryList::onItemClicked(QListWidgetItem *item)
{
	QtHistoryItem* historyItem = dynamic_cast<QtHistoryItem*>(m_list->itemWidget(item));
	if (historyItem)
	{
		if (historyItem->index != m_currentIndex)
		{
			MessageToUndoRedoPosition(historyItem->index).dispatch();
		}
		close();
	}
}
