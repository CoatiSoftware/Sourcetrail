#include "QtSmartSearchBox.h"

#include <deque>
#include <regex>
#include <stdlib.h>

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

#include "ColorScheme.h"
#include "GraphViewStyle.h"
#include "MessageFocusedSearchView.h"
#include "NodeTypeSet.h"
#include "utility.h"
#include "utilityString.h"

QtSearchElement::QtSearchElement(const QString& text, QWidget* parent): QPushButton(text, parent)
{
	show();
	setCheckable(true);
	connect(this, &QtSearchElement::clicked, this, &QtSearchElement::onChecked);
}

void QtSearchElement::onChecked(bool)
{
	emit wasChecked(this);
}

void QtSmartSearchBox::startSearch()
{
	editTextToElement();

	if (m_matches.empty())
	{
		return;
	}

	// Do a fulltext search if no autocompletion match was selected for this match
	if (m_matches.size() == 1)
	{
		SearchMatch& match = m_matches.front();
		if (match.searchType == SearchMatch::SEARCH_NONE && !match.name.empty())
		{
			if (m_oldMatch.name == match.name)
			{
				clearMatches();
				addMatchAndUpdate(m_oldMatch);
			}
			else
			{
				QString text = QString::fromStdWString(match.name);
				if (m_supportsFullTextSearch &&
					!text.startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
				{
					text = QChar(SearchMatch::FULLTEXT_SEARCH_CHARACTER) + text;
				}

				clearMatches();
				updateElements();

				setEditText(text);
				startFullTextSearch();
				return;
			}
		}
	}

	emit search(utility::toVector(m_matches), getMatchAcceptedNodeTypes());
}

QtSmartSearchBox::QtSmartSearchBox(const QString& placeholder, bool supportsFullTextSearch, QWidget* parent)
	: QLineEdit(parent)
	, m_placeholder(placeholder)
	, m_supportsFullTextSearch(supportsFullTextSearch)
	, m_allowTextChange(false)
	, m_cursorIndex(0)
	, m_shiftKeyDown(false)
	, m_mousePressed(false)
	, m_ignoreNextMousePress(false)
{
	setObjectName(QStringLiteral("search_box"));
	setAttribute(Qt::WA_MacShowFocusRect, 0);	 // remove blue focus box on Mac

	m_highlightRect = new QWidget(this);
	m_highlightRect->setGeometry(0, 0, 0, 0);
	m_highlightRect->setObjectName(QStringLiteral("search_box_highlight"));

	connect(this, &QtSmartSearchBox::textEdited, this, &QtSmartSearchBox::onTextEdited);
	connect(this, &QtSmartSearchBox::textChanged, this, &QtSmartSearchBox::onTextChanged);

	m_completer = new QtAutocompletionList(this);
	m_completer->setWidget(this);

	updatePlaceholder();
}

QtSmartSearchBox::~QtSmartSearchBox() {}

QCompleter* QtSmartSearchBox::getCompleter() const
{
	return m_completer;
}

std::vector<SearchMatch> QtSmartSearchBox::getMatches() const
{
	return utility::toVector(m_matches);
}

void QtSmartSearchBox::setAutocompletionList(const std::vector<SearchMatch>& autocompletionList)
{
	// Save the cursor position, because after activating the completer the cursor gets set to the
	// end position.
	int cursor = cursorPosition();

	QtAutocompletionList* completer = m_completer;
	completer->completeAt(QPoint(textMargins().left() + 3, height() + 5), autocompletionList);

	setCursorPosition(cursor);

	connect(
		completer,
		&QtAutocompletionList::matchHighlighted,
		this,
		&QtSmartSearchBox::onAutocompletionHighlighted,
		Qt::DirectConnection);
	connect(
		completer,
		&QtAutocompletionList::matchActivated,
		this,
		&QtSmartSearchBox::onAutocompletionActivated,
		Qt::DirectConnection);

	if (!autocompletionList.empty())
	{
		m_highlightedMatch = *completer->getSearchMatchAt(0);
	}
}

void QtSmartSearchBox::setMatches(const std::vector<SearchMatch>& matches)
{
	if (SearchMatch::searchMatchesToString(matches) ==
		SearchMatch::searchMatchesToString(utility::toVector(m_matches)))
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
	if (hasFocus())
	{
		clearFocus();
	}

	QLineEdit::setFocus(Qt::ShortcutFocusReason);
}

void QtSmartSearchBox::findFulltext()
{
	QLineEdit::setFocus(Qt::ShortcutFocusReason);
	selectAllElementsWith(true);
	deleteSelectedElements();

	setEditText(QChar(SearchMatch::FULLTEXT_SEARCH_CHARACTER));
}

void QtSmartSearchBox::refreshStyle()
{
	updateElements();
}

bool QtSmartSearchBox::event(QEvent* event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Tab)
		{
			if (m_completer->popup()->isVisible())
			{
				if (m_highlightedMatch.isFilterCommand())
				{
					addMatchAndUpdate(m_highlightedMatch);
				}
				else if (m_highlightedMatch.hasChildren && m_highlightedMatch.tokenNames.size())
				{
					setEditText(QString::fromStdWString(
						m_highlightedMatch.getFullName() +
						m_highlightedMatch.tokenNames[0].getDelimiter()));
					requestAutoCompletions();
				}
				else
				{
					setEditText(QString::fromStdWString(m_highlightedMatch.getFullName()));
					requestAutoCompletions();
				}
				return true;
			}
		}
		else if (keyEvent->key() == Qt::Key_Escape)
		{
			clearFocus();
		}
	}

	return QWidget::event(event);
}

void QtSmartSearchBox::resizeEvent(QResizeEvent* event)
{
	QLineEdit::resizeEvent(event);
	layoutElements();
}

void QtSmartSearchBox::focusInEvent(QFocusEvent* event)
{
	QLineEdit::focusInEvent(event);
	MessageFocusedSearchView(true).dispatch();

	if (event->reason() != Qt::MouseFocusReason && event->reason() != Qt::ShortcutFocusReason)
	{
		return;
	}

	if (text().size() == 1 && text().startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
	{
		setEditText(QLatin1String(""));
	}
	else
	{
		editTextToElement();
		selectAllElementsWith(true);
		layoutElements();

		if (m_elements.size() == 1)
		{
			SearchMatch match = editElement(m_elements[0]);
			if (match.searchType != SearchMatch::SEARCH_NONE)
			{
				m_oldMatch = match;
			}
			selectAll();
		}
	}

	if (event->reason() == Qt::MouseFocusReason)
	{
		m_ignoreNextMousePress = true;
	}
}

void QtSmartSearchBox::focusOutEvent(QFocusEvent* event)
{
	QLineEdit::focusOutEvent(event);
	MessageFocusedSearchView(false).dispatch();
}

void QtSmartSearchBox::keyPressEvent(QKeyEvent* event)
{
	m_shiftKeyDown = event->modifiers() & Qt::ShiftModifier;
	bool layout = false;

	if (event->key() == Qt::Key_Return)
	{
		if (text().startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
		{
			startFullTextSearch();
			return;
		}
		else if (!m_completer->popup()->isVisible())
		{
			startSearch();
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
			editElement(m_elements[m_cursorIndex - 1]);
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
		else if (
			!hasSelectedText() && cursorPosition() == text().size() &&
			m_cursorIndex < m_elements.size())
		{
			editElement(m_elements[m_cursorIndex]);
			setCursorPosition(0);
			return;
		}
		else if (event->matches(QKeySequence::DeleteStartOfWord))
		{
			std::string str = text().toStdString();
			std::smatch match;
			std::regex regExp(".\\b");

			std::deque<std::string> parts;
			while (std::regex_search(str, match, regExp))
			{
				parts.push_back(str.substr(0, match.position(0) + match.length(0)));
				str = match.suffix();
			}

			if (str.size())
			{
				parts.push_back(str);
			}

			str.clear();
			while (parts.size() && int(str.size() + parts.front().size()) < cursorPosition())
			{
				str += parts.front();
				parts.pop_front();
			}

			size_t size = 0;
			if (parts.size())
			{
				size = str.size();
				str += parts.front().substr(cursorPosition() - str.size());
				parts.pop_front();
			}

			str += utility::join(parts, "");

			setEditText(QString::fromStdString(str));
			if (size)
			{
				setCursorPosition(static_cast<int>(size));
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
		else if (cursorPosition())
		{
			layout = true;
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
				return;
			}
			else if (m_cursorIndex < m_elements.size())
			{
				editTextToElement();
				moveCursor(1);
			}
		}
		else if (cursorPosition())
		{
			layout = true;
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
			moveCursorTo(static_cast<int>(m_elements.size()));
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
			std::wstring str = getSelectedString();
			deleteSelectedElements();
			QApplication::clipboard()->setText(QString::fromStdWString(str));
			return;
		}
	}
	else if (event->matches(QKeySequence::Copy))
	{
		if (hasSelectedElements())
		{
			std::wstring str = getSelectedString();
			QApplication::clipboard()->setText(QString::fromStdWString(str));
			return;
		}
	}
	else if (event->matches(QKeySequence::Paste))
	{
		deleteSelectedElements();
		if (hasSelectedText())
		{
			del();
		}

		setEditText(text() + QApplication::clipboard()->text());
		onTextEdited(text());
		m_allowTextChange = false;
		return;
	}

	QLineEdit::keyPressEvent(event);

	if (layout)
	{
		layoutElements();
	}
}

void QtSmartSearchBox::keyReleaseEvent(QKeyEvent* event)
{
	m_shiftKeyDown = event->modifiers() & Qt::ShiftModifier;
}

void QtSmartSearchBox::mouseMoveEvent(QMouseEvent* event)
{
	QLineEdit::mouseMoveEvent(event);

	if (!m_mousePressed || m_elements.empty())
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
	if (!m_ignoreNextMousePress)
	{
		QLineEdit::mousePressEvent(event);

		m_mousePressed = true;
		m_mouseX = event->x();
	}

	m_ignoreNextMousePress = false;
}

void QtSmartSearchBox::mouseReleaseEvent(QMouseEvent* event)
{
	QLineEdit::mouseReleaseEvent(event);

	if (!m_mousePressed)
	{
		return;
	}

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
			pos = static_cast<int>(i) + 1;
			minDist = dist;
		}
	}

	bool hasSelected = hasSelectedElements();
	selectAllElementsWith(false);

	if (pos - m_cursorIndex != 0)
	{
		moveCursor(static_cast<int>(pos - m_cursorIndex));
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
	std::deque<SearchMatch> matches = getMatchesForInput(text.toStdWString());

	while (!matches.empty())
	{
		match = matches.front();
		matches.pop_front();

		if (!matches.empty() || match.isValid())
		{
			addMatch(match);
			match = SearchMatch();
			matchesChanged = true;
		}
	}

	if (matchesChanged)
	{
		setEditText(QString::fromStdWString(match.getFullName()));
		updateElements();
	}
	else
	{
		layoutElements();
	}

	if (!match.name.empty())
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

	if (!match.name.empty())
	{
		startSearch();
	}
}

void QtSmartSearchBox::onElementSelected(QtSearchElement* element)
{
	if (m_ignoreNextMousePress)
	{
		m_ignoreNextMousePress = false;
		return;
	}

	if (!hasFocus())
	{
		setFocus();
		return;
	}

	if (!hasSelectedElements() && !m_shiftKeyDown)
	{
		editElement(element);
		return;
	}

	size_t idx = 0;
	bool checked = element->isChecked();
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (m_elements[i] == element)
		{
			idx = i;
			break;
		}
	}

	if (!text().isEmpty())
	{
		if (m_cursorIndex <= idx)
		{
			idx++;
		}

		editTextToElement();

		element = m_elements[idx];
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
	moveCursorTo(static_cast<int>(m_cursorIndex + offset));
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
	if (match.name.empty())
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

	if (lastMatchIsNoFilter())
	{
		clearMatches();
	}

	m_matches.insert(m_matches.begin() + m_cursorIndex, *matchPtr);
	m_cursorIndex++;
}

void QtSmartSearchBox::addMatchAndUpdate(const SearchMatch& match)
{
	if (!match.name.empty())
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
	if (!text().isEmpty())
	{
		addMatch(SearchMatch(text().toStdWString()));
		clearLineEdit();
		updateElements();

		return true;
	}

	return false;
}

SearchMatch QtSmartSearchBox::editElement(QtSearchElement* element)
{
	for (int i = static_cast<int>(m_elements.size() - 1); i >= 0; i--)
	{
		if (m_elements[i] == element)
		{
			m_cursorIndex = i;
			break;
		}
	}

	SearchMatch match = m_matches[m_cursorIndex];
	m_matches.erase(m_matches.begin() + m_cursorIndex);

	setEditText(QString::fromStdWString(match.getFullName()));
	updateElements();

	return match;
}

void QtSmartSearchBox::updateElements()
{
	for (auto e: m_elements)
	{
		e->hide();
		e->deleteLater();
	}
	m_elements.clear();

	ColorScheme* scheme = ColorScheme::getInstance().get();
	std::string searchTextColor = scheme->getColor("search/field/text");

	for (const SearchMatch& match: m_matches)
	{
		std::wstring name = match.getFullName();
		name = utility::replace(name, L"&", L"&&");
		if (match.isFilterCommand())
		{
			name += L':';
		}

		QtSearchElement* element = new QtSearchElement(QString::fromStdWString(name), this);
		m_elements.push_back(element);

		std::string color;
		std::string hoverColor;
		std::string textColor = searchTextColor;
		std::string textHoverColor = searchTextColor;

		if (match.searchType == SearchMatch::SEARCH_TOKEN)
		{
			color = GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), false).fill;
			hoverColor =
				GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), true).fill;
			textColor =
				GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), false).text;
			textHoverColor =
				GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), true).text;
		}
		else
		{
			const std::wstring typeName = match.getSearchTypeName();

			color = scheme->getSearchTypeColor(utility::encodeToUtf8(typeName), "fill");
			hoverColor = scheme->getSearchTypeColor(
				utility::encodeToUtf8(typeName), "fill", "hover");
			textColor = scheme->getSearchTypeColor(utility::encodeToUtf8(typeName), "text");
			textHoverColor = scheme->getSearchTypeColor(
				utility::encodeToUtf8(typeName), "text", "hover");
			;
		}

		std::stringstream css;
		css << "QPushButton { border: none; padding: 0px 4px; background-color:" << color
			<< "; color:" << textColor << ";} ";
		css << "QPushButton:hover { background-color:" << hoverColor << "; color:" << textHoverColor
			<< ";} ";

		element->setStyleSheet(css.str().c_str());

		connect(element, &QtSearchElement::wasChecked, this, &QtSmartSearchBox::onElementSelected);
	}

	updatePlaceholder();
	hideAutoCompletions();

	layoutElements();
}

void QtSmartSearchBox::layoutElements()
{
	ensurePolished();

	bool hasSelected = hasSelectedElements();

	int x = 5;
	int editX = x;
	std::vector<int> elementX;

	int highlightBegin = 0;
	int highlightEnd = 0;

	for (size_t i = 0; i <= m_elements.size(); i++)
	{
		if (!hasSelected && i == m_cursorIndex)
		{
			editX = x - 5;
			x += fontMetrics().width(text());
		}

		if (i < m_elements.size())
		{
			QtSearchElement* button = m_elements[i];

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

	int cursorX = fontMetrics().width(text().left(cursorPosition()));
	int offsetX = m_oldLayoutOffset;

	if (x < width())
	{
		offsetX = 0;
	}
	else if (editX + cursorX + 10 > width() - m_oldLayoutOffset)
	{
		offsetX = width() - editX - cursorX - 10;
	}
	else if (m_oldLayoutOffset < 10 - editX - cursorX)
	{
		offsetX = std::min(10 - editX - cursorX, 0);
	}

	QMargins margins = textMargins();
	setTextMargins(offsetX + editX, margins.top(), margins.right(), margins.bottom());
	m_oldLayoutOffset = offsetX;

	for (size_t i = 0; i < elementX.size(); i++)
	{
		QtSearchElement* button = m_elements[i];
		QSize size = button->minimumSizeHint();
		const int y = static_cast<int>((rect().height() - size.height()) / 2.0);
		button->setGeometry(elementX[i] + offsetX, y, size.width(), size.height());
	}

	if (hasSelected)
	{
		m_highlightRect->setGeometry(
			highlightBegin + offsetX, 0, highlightEnd - highlightBegin, height());
	}
	else
	{
		m_highlightRect->setGeometry(0, 0, 0, 0);
	}
}

bool QtSmartSearchBox::hasSelectedElements() const
{
	for (const QtSearchElement* element: m_elements)
	{
		if (element->isChecked())
		{
			return true;
		}
	}
	return false;
}

std::wstring QtSmartSearchBox::getSelectedString() const
{
	std::wstring str;
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
	for (QtSearchElement* element: m_elements)
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
	if (text().isEmpty() && m_elements.empty())
	{
		setPlaceholderText(m_placeholder);
	}
	else
	{
		setPlaceholderText(QLatin1String(""));
	}
}

void QtSmartSearchBox::clearLineEdit()
{
	setEditText(QLatin1String(""));
	hideAutoCompletions();
}

void QtSmartSearchBox::requestAutoCompletions()
{
	if (!text().isEmpty() && !text().startsWith(SearchMatch::FULLTEXT_SEARCH_CHARACTER))
	{
		emit autocomplete(text().toStdWString(), getMatchAcceptedNodeTypes());
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

void QtSmartSearchBox::startFullTextSearch()
{
	if (!m_supportsFullTextSearch)
	{
		return;
	}

	std::wstring term = text().toStdWString().substr(1);
	if (term.empty())
	{
		return;
	}

	bool caseSensitive = false;
	if (term.at(0) == SearchMatch::FULLTEXT_SEARCH_CHARACTER)
	{
		term = term.substr(1);
		if (term.empty())
		{
			return;
		}

		caseSensitive = true;
	}

	emit fullTextSearch(term, caseSensitive);
}

std::deque<SearchMatch> QtSmartSearchBox::getMatchesForInput(const std::wstring& text) const
{
	std::deque<SearchMatch> matches;
	if (!text.empty())
	{
		matches.push_back(SearchMatch(text));
	}
	return matches;
}

NodeTypeSet QtSmartSearchBox::getMatchAcceptedNodeTypes() const
{
	NodeTypeSet acceptedTypes;

	for (const SearchMatch& match: m_matches)
	{
		if (match.isFilterCommand())
		{
			acceptedTypes.add(match.nodeType);
		}
		else
		{
			break;
		}
	}

	if (acceptedTypes.isEmpty())
	{
		acceptedTypes.invert();
	}

	return acceptedTypes;
}

bool QtSmartSearchBox::lastMatchIsNoFilter() const
{
	return m_matches.empty() || !m_matches.back().isFilterCommand();
}
