#include "qt/element/QtSmartSearchBox.h"

#include <stdlib.h>

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchFullText.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "component/view/GraphViewStyle.h"
#include "settings/ColorScheme.h"

QtSearchElement::QtSearchElement(const QString& text, QWidget* parent)
	: QPushButton(text, parent)
{
	show();
	setCheckable(true);
	connect(this, SIGNAL(clicked(bool)), this, SLOT(onChecked(bool)));
}

void QtSearchElement::onChecked(bool)
{
	emit wasChecked(this);
}

void QtSmartSearchBox::search()
{
	editTextToElement();

	// Do a fulltext search if no autocompletion match was selected for this match
	if (m_matches.size() == 1)
	{
		SearchMatch& match = m_matches.front();
		if (match.searchType == SearchMatch::SEARCH_NONE && match.name.size())
		{
			QString text = QString::fromStdString(match.name);
			if (!text.startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
			{
				text = QChar(SearchMatch::FULLTEXT_SEARCH_CHARACTER) + text;
			}

			clearMatches();
			updateElements();

			setEditText(text);
			fullTextSearch();
			return;
		}
	}

	std::vector<SearchMatch> matches = utility::toVector(m_matches);

	MessageSearch(matches).dispatch();
}

void QtSmartSearchBox::fullTextSearch()
{
	std::string term = text().toStdString().substr(1);
	if (!term.size())
	{
		return;
	}

	bool caseSensitive = false;
	if (term.at(0) == SearchMatch::FULLTEXT_SEARCH_CHARACTER)
	{
		term = term.substr(1);
		if (!term.size())
		{
			return;
		}

		caseSensitive = true;
	}

	MessageSearchFullText(term, caseSensitive).dispatch();
}

QtSmartSearchBox::QtSmartSearchBox(QWidget* parent)
	: QLineEdit(parent)
	, m_allowMultipleElements(false)
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

	m_completer = new QtAutocompletionList(this);
	m_completer->setWidget(this);

	updatePlaceholder();
}

QtSmartSearchBox::~QtSmartSearchBox()
{
}

QCompleter* QtSmartSearchBox::getCompleter() const
{
	return m_completer;
}

void QtSmartSearchBox::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	// Save the cursor position, because after activating the completer the cursor gets set to the end position.
	int cursor = cursorPosition();

	QtAutocompletionList* completer = m_completer;
	completer->completeAt(QPoint(textMargins().left() + 3, height() + 5), autocompletionList);

	setCursorPosition(cursor);

	connect(completer, SIGNAL(matchHighlighted(const SearchMatch&)), this, SLOT(onAutocompletionHighlighted(const SearchMatch&)), Qt::DirectConnection);
	connect(completer, SIGNAL(matchActivated(const SearchMatch&)), this, SLOT(onAutocompletionActivated(const SearchMatch&)), Qt::DirectConnection);

	if (autocompletionList.size())
	{
		m_highlightedMatch = *completer->getSearchMatchAt(0);
	}
}

void QtSmartSearchBox::setMatches(const std::vector<SearchMatch>& matches)
{
	if (SearchMatch::searchMatchesToString(matches) == SearchMatch::searchMatchesToString(utility::toVector(m_matches)))
	{
		return;
	}

	clearLineEdit();

	m_matches.clear();
	m_matches.insert(m_matches.begin(), matches.begin(), matches.end());

	m_cursorIndex = m_matches.size();

	updateElements();
}

void QtSmartSearchBox::setFocus()
{
	QLineEdit::setFocus(Qt::ShortcutFocusReason);

	if (text().size() == 1 && text().startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
	{
		setEditText("");
	}
	else
	{
		editTextToElement();
		selectAllElementsWith(true);
		layoutElements();
	}
}

void QtSmartSearchBox::findFulltext()
{
	QLineEdit::setFocus(Qt::ShortcutFocusReason);
	selectAllElementsWith(true);
	deleteSelectedElements();

	setEditText(QChar(SearchMatch::FULLTEXT_SEARCH_CHARACTER));
}

bool QtSmartSearchBox::event(QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Tab)
		{
			if (m_completer->popup()->isVisible())
			{
				if (m_highlightedMatch.hasChildren)
				{
					setEditText((m_highlightedMatch.getFullName() + NameHierarchy::getDelimiter()).c_str());
				}
				else
				{
					setEditText(m_highlightedMatch.getFullName().c_str());
				}

				requestAutoCompletions();
			}
			else if (m_allowMultipleElements)
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
		if (text().startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
		{
			fullTextSearch();
			return;
		}
		else if (!m_completer->popup()->isVisible())
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
	else if (event->matches(QKeySequence::Delete) || event->matches(QKeySequence::DeleteStartOfWord))
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
		else
		{
			std::vector<std::string> names = utility::splitToVector(text().toStdString(), NameHierarchy::getDelimiter());
			if (names.back() == "")
			{
				names.pop_back();
			}

			if (names.size() < 2)
			{
				setEditText("");
			}
			else
			{
				names.back() = "";
				setEditText(utility::join(names, NameHierarchy::getDelimiter()).c_str());
			}

			requestAutoCompletions();
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
			if (m_completer->popup()->isVisible())
			{
				addMatchAndUpdate(m_highlightedMatch);
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
		m_allowTextChange = false;
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

	if (!m_mousePressed || !m_elements.size())
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

	bool matchesChanged = false;

	SearchMatch match;
	std::deque<SearchMatch> matches = getMatchesForInput(text.toStdString());

	while (matches.size())
	{
		match = matches.front();
		matches.pop_front();

		if (matches.size() || match.isValid())
		{
			addMatch(match);
			match = SearchMatch();
			matchesChanged = true;
		}
	}

	if (match.name.size() && !m_allowMultipleElements)
	{
		if (m_matches.size())
		{
			matchesChanged = true;
		}
		clearMatches();
	}

	if (matchesChanged)
	{
		setEditText(QString::fromStdString(match.getFullName()));
		updateElements();
	}
	else
	{
		layoutElements();
	}

	if (match.name.size() || m_elements.size())
	{
		requestAutoCompletions();
	}
	else
	{
		hideAutoCompletions();
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
	addMatchAndUpdate(match);

	if (match.name.size())
	{
		search();
	}
}

void QtSmartSearchBox::onElementSelected(QtSearchElement* element)
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


void QtSmartSearchBox::addMatch(const SearchMatch& match)
{
	if (!match.name.size())
	{
		return;
	}

	const SearchMatch* matchPtr = &match;

	if (m_completer->popup()->isVisible())
	{
		const SearchMatch* mPtr = m_completer->getSearchMatchAt(0);
		if (mPtr && utility::equalsCaseInsensitive(match.getFullName(), mPtr->getFullName()))
		{
			matchPtr = mPtr;
		}
	}

	if (!m_allowMultipleElements)
	{
		clearMatches();
	}

	m_matches.insert(m_matches.begin() + m_cursorIndex, *matchPtr);
	m_cursorIndex++;
}

void QtSmartSearchBox::addMatchAndUpdate(const SearchMatch& match)
{
	if (match.name.size())
	{
		m_oldText.clear();
		clearLineEdit();

		addMatch(match);
		updateElements();
	}
}

void QtSmartSearchBox::clearMatches()
{
	m_matches.clear();
	m_cursorIndex = 0;
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
		addMatch(SearchMatch(text().toStdString()));
		clearLineEdit();
		updateElements();

		return true;
	}

	return false;
}

void QtSmartSearchBox::editElement(QtSearchElement* element)
{
	for (int i = m_elements.size() - 1; i >= 0; i--)
	{
		if (m_elements[i].get() == element)
		{
			m_cursorIndex = i;
			break;
		}
	}

	std::string name = m_matches[m_cursorIndex].getFullName();
	m_matches.erase(m_matches.begin() + m_cursorIndex);

	setEditText(QString::fromStdString(name));
	updateElements();

	requestAutoCompletions();
}

void QtSmartSearchBox::updateElements()
{
	m_elements.clear();

	ColorScheme* scheme = ColorScheme::getInstance().get();
	std::string searchTextColor = scheme->getColor("search/field/text");

	for (const SearchMatch& match : m_matches)
	{
		std::string name = match.getFullName();
		name = utility::replace(name, "&", "&&");

		std::shared_ptr<QtSearchElement> element = std::make_shared<QtSearchElement>(QString::fromStdString(name), this);
		m_elements.push_back(element);

		std::string color;
		std::string hoverColor;
		std::string textColor = searchTextColor;
		std::string textHoverColor = searchTextColor;

		if (match.searchType == SearchMatch::SEARCH_TOKEN)
		{
			element->setObjectName(QString::fromStdString("search_element_" + match.getNodeTypeAsString()));
			color = GraphViewStyle::getNodeColor(Node::getTypeString(match.nodeType), false).fill;
			hoverColor = GraphViewStyle::getNodeColor(Node::getTypeString(match.nodeType), true).fill;
			textColor = GraphViewStyle::getNodeColor(Node::getTypeString(match.nodeType), false).text;
			textHoverColor = GraphViewStyle::getNodeColor(Node::getTypeString(match.nodeType), true).text;
		}
		else
		{
			std::string typeName = match.getSearchTypeName();

			element->setObjectName(QString::fromStdString("search_element_" + typeName));
			color = scheme->getSearchTypeColor(typeName, "fill");
			hoverColor = scheme->getSearchTypeColor(typeName, "fill", "hover");
			textColor = scheme->getSearchTypeColor(typeName, "text");
			textHoverColor = scheme->getSearchTypeColor(typeName, "text", "hover");;
		}

		std::stringstream css;
		css << "QPushButton { border: none; padding: 0px 4px; background-color:" << color << "; color:" << textColor << ";} ";
		css << "QPushButton:hover { background-color:" << hoverColor << "; color:" << textHoverColor << ";} ";

		element->setStyleSheet(css.str().c_str());

		connect(element.get(), SIGNAL(wasChecked(QtSearchElement*)), this, SLOT(onElementSelected(QtSearchElement*)));
	}

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
			QtSearchElement* button = m_elements[i].get();

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
		QtSearchElement* button = m_elements[i].get();
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
	for (const std::shared_ptr<QtSearchElement> element : m_elements)
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
			str += m_matches[i].getFullName();
		}
	}
	return str;
}

void QtSmartSearchBox::selectAllElementsWith(bool selected)
{
	for (const std::shared_ptr<QtSearchElement> element : m_elements)
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
			m_matches.erase(m_matches.begin() + (i - 1));

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

void QtSmartSearchBox::requestAutoCompletions()
{
	if (text().size() && !text().startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
	{
		MessageSearchAutocomplete(text().toStdString()).dispatch();
	}
	else
	{
		hideAutoCompletions();
	}
}

void QtSmartSearchBox::hideAutoCompletions()
{
	m_completer->popup()->hide();
}

std::deque<SearchMatch> QtSmartSearchBox::getMatchesForInput(const std::string& text) const
{
	std::deque<SearchMatch> matches;
	if (text.size())
	{
		matches.push_back(SearchMatch(text));
	}
	return matches;
}
