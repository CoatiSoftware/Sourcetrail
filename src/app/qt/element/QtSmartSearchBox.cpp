#include "qt/element/QtSmartSearchBox.h"

#include <stdlib.h>

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

#include "data/query/QueryTree.h"
#include "qt/element/QtAutocompletionList.h"
#include "settings/ApplicationSettings.h"

QtQueryElement::QtQueryElement(const QString& text, QWidget* parent)
	: QPushButton(text, parent)
{
	show();
	setCheckable(true);
	connect(this, SIGNAL(clicked(bool)), this, SLOT(onChecked(bool)));
}

void QtQueryElement::onChecked(bool)
{
	emit wasChecked(this);
}


void QtSmartSearchBox::search()
{
	editTextToElement();
	LOG_INFO(utility::join(SearchMatch::searchMatchDequeToStringDeque(m_tokens), "")+ text().toStdString());
	MessageSearch(utility::join(SearchMatch::searchMatchDequeToStringDeque(m_tokens), "") + text().toStdString()).dispatch();
}

QtSmartSearchBox::QtSmartSearchBox(QWidget* parent)
	: QLineEdit(parent)
	, m_allowTextChange(false)
	, m_cursorIndex(0)
	, m_shiftKeyDown(false)
	, m_mousePressed(false)
{
	m_highlightRect = new QWidget(this);
	m_highlightRect->setGeometry(0, 0, 0, 0);
	m_highlightRect->setObjectName("search_box_highlight");

	connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));

	QCompleter* completer = new QtAutocompletionList(this);
	setCompleter(completer);

	updatePlaceholder();
}

QtSmartSearchBox::~QtSmartSearchBox()
{
}

void QtSmartSearchBox::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	// Save the cursor position, because after activating the completer the cursor gets set to the end position.
	int cursor = cursorPosition();

	QtAutocompletionList* completer = dynamic_cast<QtAutocompletionList*>(this->completer());
	completer->completeAt(QPoint(textMargins().left() + 3, height() + 3), autocompletionList);

	setCursorPosition(cursor);

	connect(completer, SIGNAL(matchHighlighted(const SearchMatch&)), this, SLOT(onAutocompletionHighlighted(const SearchMatch&)), Qt::DirectConnection);
	connect(completer, SIGNAL(matchActivated(const SearchMatch&)), this, SLOT(onAutocompletionActivated(const SearchMatch&)), Qt::DirectConnection);

	if (autocompletionList.size())
	{
		m_highlightedMatch = *completer->getSearchMatchAt(0);
	}
}

void QtSmartSearchBox::setQuery(const SearchMatch& match)
{
	clearLineEdit();
	m_tokens.clear();
	m_tokens.push_back(match);
	m_cursorIndex = m_tokens.size();
	updateElements();
}

void QtSmartSearchBox::setQuery(const std::string& text)
{
	clearLineEdit();
	m_tokens = SearchMatch::stringDequeToSearchMatchDeque(QueryTree::tokenizeQuery(text));
	m_cursorIndex = m_tokens.size();

	updateElements();
}

void QtSmartSearchBox::setFocus()
{
	QLineEdit::setFocus(Qt::ShortcutFocusReason);
	selectAllElementsWith(true);
	layoutElements();
}

bool QtSmartSearchBox::event(QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Tab)
		{
			if (completer()->popup()->isVisible())
			{
				searchMatchToToken(m_highlightedMatch);
			}
			else
			{
				requestAutoCompletions();
			}
			return true;
		}
	}

	return QWidget::event(event);
}

void QtSmartSearchBox::resizeEvent(QResizeEvent* event)
{
	QLineEdit::resizeEvent(event);
	layoutElements();
}

void QtSmartSearchBox::keyPressEvent(QKeyEvent* event)
{
	m_shiftKeyDown = event->modifiers() & Qt::ShiftModifier;

	if (event->key() == Qt::Key_Return)
	{
		if (!completer()->popup()->isVisible())
		{
			search();
		}
	}
	else if (event->key() == Qt::Key_Backspace)
	{
		if (hasSelectedElements())
		{
			deleteSelectedElements();
			return;
		}
		else if (!hasSelectedText() && cursorPosition() == 0 && m_cursorIndex > 0)
		{
			m_elements[m_cursorIndex - 1]->setChecked(true);
			deleteSelectedElements();
			return;
		}
	}
	else if (event->matches(QKeySequence::Delete))
	{
		if (hasSelectedElements())
		{
			deleteSelectedElements();
			return;
		}
		else if (!hasSelectedText() && cursorPosition() == text().size() && m_cursorIndex < m_elements.size())
		{
			m_elements[m_cursorIndex]->setChecked(true);
			deleteSelectedElements();
			return;
		}
	}
	else if (event->matches(QKeySequence::MoveToPreviousChar))
	{
		if (hasSelectedElements())
		{
			for (size_t i = 0; i < m_elements.size(); i++)
			{
				if (m_elements[i]->isChecked())
				{
					m_cursorIndex = i;
					break;
				}
			}

			selectAllElementsWith(false);
			layoutElements();
		}
		else if (cursorPosition() == 0 && m_cursorIndex > 0 && !event->isAutoRepeat())
		{
			editTextToElement();
			moveCursor(-1);
			return;
		}
	}
	else if (event->matches(QKeySequence::MoveToNextChar))
	{
		if (hasSelectedElements())
		{
			for (size_t i = m_elements.size(); i > 0; i--)
			{
				if (m_elements[i - 1]->isChecked())
				{
					m_cursorIndex = i;
					break;
				}
			}

			selectAllElementsWith(false);
			layoutElements();
		}
		else if (cursorPosition() == text().size() && !event->isAutoRepeat())
		{
			if (completer()->popup()->isVisible())
			{
				searchMatchToToken(m_highlightedMatch);
			}
			else if (!editTextToElement())
			{
				moveCursor(1);
			}
			return;
		}
	}
	else if (event->matches(QKeySequence::SelectPreviousChar))
	{
		if (cursorPosition() == 0 && m_cursorIndex > 0)
		{
			editTextToElement();
			m_elements[m_cursorIndex - 1]->setChecked(!m_elements[m_cursorIndex - 1]->isChecked());
			moveCursor(-1);
		}
	}
	else if (event->matches(QKeySequence::SelectNextChar))
	{
		if (cursorPosition() == text().size() && m_cursorIndex < m_elements.size())
		{
			editTextToElement();
			m_elements[m_cursorIndex]->setChecked(!m_elements[m_cursorIndex]->isChecked());
			moveCursor(1);
		}
	}
	else if (event->matches(QKeySequence::MoveToStartOfLine))
	{
		editTextToElement();
		moveCursorTo(0);
		return;
	}
	else if (event->matches(QKeySequence::MoveToEndOfLine))
	{
		if (m_cursorIndex < m_elements.size())
		{
			editTextToElement();
			moveCursorTo(m_elements.size());
			return;
		}
	}
	else if (event->matches(QKeySequence::SelectAll))
	{
		if (m_elements.size())
		{
			editTextToElement();
			selectAllElementsWith(true);
			layoutElements();
			return;
		}
	}
	else if (event->matches(QKeySequence::Cut))
	{
		if (hasSelectedElements())
		{
			std::string str = getSelectedString();
			deleteSelectedElements();
			QApplication::clipboard()->setText(QString::fromStdString(str));
			return;
		}
	}
	else if (event->matches(QKeySequence::Copy))
	{
		if (hasSelectedElements())
		{
			std::string str = getSelectedString();
			QApplication::clipboard()->setText(QString::fromStdString(str));
			return;
		}
	}
	else if (event->matches(QKeySequence::Paste))
	{
		setEditText(text() + QApplication::clipboard()->text());
		onTextEdited(text());
		return;
	}

	QLineEdit::keyPressEvent(event);
}

void QtSmartSearchBox::keyReleaseEvent(QKeyEvent* event)
{
	m_shiftKeyDown = event->modifiers() & Qt::ShiftModifier;
}

void QtSmartSearchBox::mouseMoveEvent(QMouseEvent* event)
{
	QLineEdit::mouseMoveEvent(event);

	if (!m_mousePressed)
	{
		return;
	}

	int lo = event->x() < m_mouseX ? event->x() : m_mouseX;
	int hi = event->x() > m_mouseX ? event->x() : m_mouseX;

	for (size_t i = 0; i < m_elements.size(); i++)
	{
		int widgetX = m_elements[i]->x() + m_elements[i]->width() / 2;
		m_elements[i]->setChecked(lo < widgetX && widgetX < hi);
	}

	editTextToElement();
	layoutElements();
}

void QtSmartSearchBox::mousePressEvent(QMouseEvent* event)
{
	QLineEdit::mousePressEvent(event);

	m_mousePressed = true;
	m_mouseX = event->x();
}

void QtSmartSearchBox::mouseReleaseEvent(QMouseEvent* event)
{
	QLineEdit::mouseReleaseEvent(event);

	m_mousePressed = false;

	if (abs(event->x() - m_mouseX) > 5)
	{
		return;
	}

	int minDist = event->x();
	int pos = 0;
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		int dist = m_elements[i]->x() + m_elements[i]->width() - event->x();
		if (abs(dist) < abs(minDist))
		{
			pos = i + 1;
			minDist = dist;
		}
	}

	bool hasSelected = hasSelectedElements();
	selectAllElementsWith(false);

	if (pos - m_cursorIndex != 0)
	{
		moveCursor(pos - m_cursorIndex);
	}
	else if (hasSelected)
	{
		layoutElements();
	}
}

void QtSmartSearchBox::onTextEdited(const QString& text)
{
	m_allowTextChange = true;
	deleteSelectedElements();

	bool tokensChanged = false;

	SearchMatch token;
	std::deque<SearchMatch> tokens = SearchMatch::stringDequeToSearchMatchDeque(QueryTree::tokenizeQuery(text.toStdString()));

	while (tokens.size())
	{
		token = tokens.front();
		tokens.pop_front();

		if (tokens.size() || token.queryNodeType != QueryNode::QUERYNODETYPE_NONE)
		{
			matchToToken(token);
			token.decodeFromQuery("");
			tokensChanged = true;
		}
	}

	if (tokensChanged)
	{
		setEditText(QString::fromStdString(token.fullName));
		updateElements();
	}
	else
	{
		layoutElements();
	}

	if (token.fullName.size() || m_elements.size())
	{
		requestAutoCompletions();
	}
}

void QtSmartSearchBox::onTextChanged(const QString& text)
{
	if (!m_allowTextChange)
	{
		setText(m_oldText);
	}
	else
	{
		m_oldText = text;
	}

	m_allowTextChange = false;

	updatePlaceholder();
}

void QtSmartSearchBox::onAutocompletionHighlighted(const SearchMatch& match)
{
	m_highlightedMatch = match;
}

void QtSmartSearchBox::onAutocompletionActivated(const SearchMatch& match)
{
	searchMatchToToken(match);

	if (match.fullName.size())
	{
		search();
	}
}

void QtSmartSearchBox::onElementSelected(QtQueryElement* element)
{
	if (!hasSelectedElements() && !m_shiftKeyDown)
	{
		editElement(element);
		return;
	}

	size_t idx = 0;
	bool checked = element->isChecked();
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (m_elements[i].get() == element)
		{
			idx = i;
			break;
		}
	}

	if (text().size())
	{
		if (m_cursorIndex <= idx)
		{
			idx++;
		}

		editTextToElement();

		element = m_elements[idx].get();
		element->setChecked(checked);
	}

	if (m_shiftKeyDown)
	{
		selectElementsTo(idx, checked);
	}
	else
	{
		selectAllElementsWith(false);
		element->setChecked(true);
		m_cursorIndex = idx + 1;
	}

	layoutElements();
}

void QtSmartSearchBox::moveCursor(int offset)
{
	moveCursorTo(m_cursorIndex + offset);
}

void QtSmartSearchBox::moveCursorTo(int target)
{
	if (target >= 0 && target <= static_cast<int>(m_elements.size()))
	{
		m_cursorIndex = target;
		layoutElements();
		hideAutoCompletions();
	}
}


void QtSmartSearchBox::matchToToken(const SearchMatch& match)
{
	if(!match.fullName.size())
	{
		return;
	}

	const SearchMatch* matchPtr = &match;

	if(completer()->popup()->isVisible())
	{
		const SearchMatch* m = dynamic_cast<QtAutocompletionList*>(completer())->getSearchMatchAt(0);
		if (m && utility::equalsCaseInsensitive(match.fullName, m->fullName))
		{
			matchPtr = m;
		}
	}
	m_tokens.insert(m_tokens.begin() + m_cursorIndex, *matchPtr);
	m_cursorIndex++;
}

void QtSmartSearchBox::searchMatchToToken(const SearchMatch& match)
{
	if (match.fullName.size())
	{
		m_oldText.clear();
		clearLineEdit();

		matchToToken(match);
		updateElements();
	}
}

void QtSmartSearchBox::setEditText(const QString& text)
{
	m_allowTextChange = true;
	setText(text);
}

bool QtSmartSearchBox::editTextToElement()
{
	if (text().size())
	{
		matchToToken(SearchMatch(text().toStdString()));
		clearLineEdit();
		updateElements();

		return true;
	}

	return false;
}

void QtSmartSearchBox::editElement(QtQueryElement* element)
{
	for (int i = m_elements.size() - 1; i >= 0; i--)
	{
		if (m_elements[i].get() == element)
		{
			m_cursorIndex = i;
			break;
		}
	}

	std::string token = m_tokens[m_cursorIndex].fullName;
	m_tokens.erase(m_tokens.begin() + m_cursorIndex);

	setEditText(QString::fromStdString(token));
	updateElements();

	requestAutoCompletions();
}

void QtSmartSearchBox::updateElements()
{
	m_elements.clear();

	for (const SearchMatch& token : m_tokens)
	{
		std::string name = token.fullName;

		name = utility::replace(name, "&", "&&");

		std::shared_ptr<QtQueryElement> element = std::make_shared<QtQueryElement>(QString::fromStdString(name), this);
		m_elements.push_back(element);
		std::string color = "#000000";
		std::string hoverColor = "#000000";

		if(token.queryNodeType == QueryNode::QUERYNODETYPE_TOKEN)
		{
			element->setObjectName(QString::fromStdString("search_element_" + token.getNodeTypeAsString()));
			color = ApplicationSettings::getInstance()->getNodeTypeColor(token.nodeType);
			hoverColor = ApplicationSettings::getInstance()->getNodeTypeColor(token.nodeType, "hover");
		}
		else
		{
			element->setObjectName(QString::fromStdString("search_element_" + QueryTree::getTokenTypeName(token.queryNodeType)));
			color = ApplicationSettings::getInstance()->getQueryNodeTypeColor(token.queryNodeType);
			hoverColor = ApplicationSettings::getInstance()->getQueryNodeTypeColor(token.queryNodeType, "hover");
		}

		std::string stylesheet = "QPushButton {background-color:";
		stylesheet += color;
		stylesheet += ";}  QPushButton:hover{ background-color:";
		stylesheet += hoverColor;
		stylesheet += ";}";

		element->setStyleSheet(stylesheet.c_str());

		connect(element.get(), SIGNAL(wasChecked(QtQueryElement*)), this, SLOT(onElementSelected(QtQueryElement*)));
	}

	setStyleSheet(TextAccess::createFromFile("data/gui/search_view/search_element.css")->getText().c_str());

	updatePlaceholder();
	hideAutoCompletions();

	layoutElements();
}

void QtSmartSearchBox::layoutElements()
{
	ensurePolished();

	bool hasSelected = hasSelectedElements();

	QString cursorText = text();
	cursorText.resize(cursorPosition());

	int x = 5;
	int editX = x;
	int cursorX = fontMetrics().width(cursorText) + x;
	std::vector<int> elementX;

	int highlightBegin = 0;
	int highlightEnd = 0;

	for (size_t i = 0; i <= m_elements.size(); i++)
	{
		if (!hasSelected && i == m_cursorIndex)
		{
			editX = x - 5;
			cursorX += editX;
			if (i != m_elements.size())
			{
				cursorX += 15;
			}
			x += fontMetrics().width(text());
		}

		if (i < m_elements.size())
		{
			QtQueryElement* button = m_elements[i].get();

			if (button->isChecked() && !highlightBegin)
			{
				highlightBegin = x - 2;
			}

			elementX.push_back(x);
			x += button->minimumSizeHint().width() + 5;

			if (button->isChecked())
			{
				highlightEnd = x - 3;
			}
		}
	}

	int offsetX = 0;
	if (cursorX > width())
	{
		offsetX = width() - cursorX;
	}

	for (size_t i = 0; i < elementX.size(); i++)
	{
		QtQueryElement* button = m_elements[i].get();
		QSize size = button->minimumSizeHint();
		int y = (rect().height() - size.height()) / 2.0;
		button->setGeometry(elementX[i] + offsetX, y, size.width(), size.height());
	}

	if (hasSelected)
	{
		setTextMargins(width() + 10, 0, 0, 0);
		m_highlightRect->setGeometry(highlightBegin + offsetX, 0, highlightEnd - highlightBegin, height());
	}
	else
	{
		QMargins margins = textMargins();
		setTextMargins(editX + offsetX, margins.top(), margins.right(), margins.bottom());
		m_highlightRect->setGeometry(0, 0, 0, 0);
	}
}

bool QtSmartSearchBox::hasSelectedElements() const
{
	for (const std::shared_ptr<QtQueryElement> element : m_elements)
	{
		if (element->isChecked())
		{
			return true;
		}
	}
	return false;
}

std::string QtSmartSearchBox::getSelectedString() const
{
	std::string str;
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (m_elements[i]->isChecked())
		{
			str += m_tokens[i].encodeForQuery();
		}
	}
	return str;
}

void QtSmartSearchBox::selectAllElementsWith(bool selected)
{
	for (const std::shared_ptr<QtQueryElement> element : m_elements)
	{
		element->setChecked(selected);
	}
}

void QtSmartSearchBox::selectElementsTo(size_t idx, bool selected)
{
	size_t low = idx < m_cursorIndex ? idx : m_cursorIndex;
	size_t hi = idx > m_cursorIndex ? idx + 1 : m_cursorIndex;

	while (low < hi)
	{
		m_elements[low]->setChecked(selected);
		low++;
	}

	if (!selected)
	{
		m_elements[idx]->setChecked(true);
	}

	if (idx < m_cursorIndex)
	{
		m_cursorIndex = idx;
	}
	else
	{
		m_cursorIndex = idx + 1;
	}
}

void QtSmartSearchBox::deleteSelectedElements()
{
	if (!hasSelectedElements())
	{
		return;
	}

	for (size_t i = m_elements.size(); i > 0; i--)
	{
		if (m_elements[i - 1]->isChecked())
		{
			m_tokens.erase(m_tokens.begin() + (i - 1));

			if ((i - 1) < m_cursorIndex)
			{
				m_cursorIndex--;
			}
		}
	}

	updateElements();
}

void QtSmartSearchBox::updatePlaceholder()
{
	if (!text().size() && !m_elements.size())
	{
		setPlaceholderText("Search");
	}
	else
	{
		setPlaceholderText("");
	}
}

void QtSmartSearchBox::clearLineEdit()
{
	setEditText("");
	hideAutoCompletions();
}

void QtSmartSearchBox::requestAutoCompletions() const
{
	std::string query;

	for (size_t i = 0; i < m_tokens.size() && i < m_cursorIndex; i++)
	{
		query += m_tokens[i].encodeForQuery();
	}

	MessageSearchAutocomplete(query, text().toStdString()).dispatch();
}

void QtSmartSearchBox::hideAutoCompletions()
{
	completer()->popup()->hide();
}
