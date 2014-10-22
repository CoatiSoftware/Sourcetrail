#include "qt/element/QtSmartSearchBox.h"

#include <stdlib.h>

#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QCompleter>
#include <QKeyEvent>

#include "data/query/QueryTree.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

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
	MessageSearch(utility::join(m_tokens, "") + text().toStdString()).dispatch();
}

QtSmartSearchBox::QtSmartSearchBox(QWidget* parent)
	: QLineEdit(parent)
	, m_cursorIndex(0)
	, m_shiftKeyDown(false)
	, m_mousePressed(false)
{
	connect(this, SIGNAL(returnPressed()), this, SLOT(search()), Qt::QueuedConnection);
	connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));

	updatePlaceholder();
}

QtSmartSearchBox::~QtSmartSearchBox()
{
}

void QtSmartSearchBox::setAutocompletionList(const std::vector<SearchIndex::SearchMatch>& autocompletionList)
{
	m_matches = autocompletionList;

	QStringList wordList;
	for (const SearchIndex::SearchMatch& match: autocompletionList)
	{
		wordList << match.fullName.c_str();
	}

	QCompleter *completer = new QCompleter(wordList, this);
	completer->popup()->setObjectName("search_box_popup");
	completer->setCaseSensitivity(Qt::CaseInsensitive);

	setCompleter(completer);
	completer->complete();

	connect(completer, SIGNAL(highlighted(const QModelIndex&)), this, SLOT(onSearchCompletionHighlighted(const QModelIndex&)), Qt::DirectConnection);
	connect(completer, SIGNAL(activated(const QModelIndex&)), this, SLOT(onSearchCompletionActivated(const QModelIndex&)), Qt::DirectConnection);
}

void QtSmartSearchBox::setQuery(const std::string& text)
{
	clearLineEdit();
	m_tokens = QueryTree::tokenizeQuery(text);
	m_cursorIndex = m_tokens.size();

	updateElements();
}

void QtSmartSearchBox::setFocus()
{
	QLineEdit::setFocus(Qt::ShortcutFocusReason);
	selectAllElementsWith(true);
}

void QtSmartSearchBox::resizeEvent(QResizeEvent* event)
{
	QLineEdit::resizeEvent(event);
	layoutElements();
}

void QtSmartSearchBox::keyPressEvent(QKeyEvent* event)
{
	m_shiftKeyDown = event->modifiers() & Qt::ShiftModifier;

	if (event->key() == Qt::Key_Backspace)
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
		else if (cursorPosition() == 0 && m_cursorIndex > 0)
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
		else if (cursorPosition() == text().size() && (text().size() || m_cursorIndex < static_cast<int>(m_elements.size())))
		{
			if (!editTextToElement())
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
		if (cursorPosition() == text().size() && m_cursorIndex < static_cast<int>(m_elements.size()))
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
		if (m_cursorIndex < static_cast<int>(m_elements.size()))
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
		setText(text() + QApplication::clipboard()->text());
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
		editTextToElement();
		moveCursor(pos - m_cursorIndex);
	}
	else if (hasSelected)
	{
		layoutElements();
	}
}

void QtSmartSearchBox::onTextEdited(const QString& text)
{
	deleteSelectedElements();

	std::string token;
	std::deque<std::string> tokens = QueryTree::tokenizeQuery(text.toStdString());
	while (tokens.size())
	{
		token = tokens.front();
		tokens.pop_front();

		if (tokens.size() || QueryTree::getTokenTypeName(token) != "none")
		{
			textToToken(token);
			token.clear();
		}
	}

	if (text.toStdString() != token)
	{
		setText(QString::fromStdString(token));
		updateElements();
	}
	else
	{
		layoutElements();
	}

	if (token.size())
	{
		MessageSearchAutocomplete(token).dispatch();
	}
}

void QtSmartSearchBox::onTextChanged(const QString& text)
{
	if (m_oldText.size())
	{
		setText(m_oldText);
	}

	m_oldText.clear();
	updatePlaceholder();
}

void QtSmartSearchBox::onSearchCompletionHighlighted(const QModelIndex& index)
{
	m_oldText = text();
}

void QtSmartSearchBox::onSearchCompletionActivated(const QModelIndex& index)
{
	if (index.row() >= 0 && index.row() < int(m_matches.size()))
	{
		m_oldText.clear();
		clearLineEdit();

		std::string match = m_matches[index.row()].encodeForQuery();
		textToToken(match);
		updateElements();
	}

	completer()->blockSignals(true);
}

void QtSmartSearchBox::onElementSelected(QtQueryElement* element)
{
	if (!hasSelectedElements() && !m_shiftKeyDown)
	{
		editElement(element);
		return;
	}

	int idx = 0;
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
	}
}

void QtSmartSearchBox::textToToken(std::string text)
{
	if (!text.size())
	{
		return;
	}

	if (m_matches.size() && utility::equalsCaseInsensitive(text, m_matches.front().fullName))
	{
		text = m_matches.front().encodeForQuery();
	}

	m_tokens.insert(m_tokens.begin() + m_cursorIndex, text);
	m_cursorIndex++;
}

bool QtSmartSearchBox::editTextToElement()
{
	if (text().size())
	{
		textToToken(text().toStdString());
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

	std::string token = QueryTree::getTokenName(m_tokens[m_cursorIndex]);
	m_tokens.erase(m_tokens.begin() + m_cursorIndex);

	setText(QString::fromStdString(token));
	updateElements();

	MessageSearchAutocomplete(token).dispatch();
}

void QtSmartSearchBox::updateElements()
{
	m_elements.clear();

	for (const std::string& token : m_tokens)
	{
		std::string name = QueryTree::getTokenName(token);

		std::shared_ptr<QtQueryElement> element = std::make_shared<QtQueryElement>(QString::fromStdString(name), this);
		m_elements.push_back(element);

		element->setObjectName(QString::fromStdString("search_element_" + QueryTree::getTokenTypeName(token)));

		connect(element.get(), SIGNAL(wasChecked(QtQueryElement*)), this, SLOT(onElementSelected(QtQueryElement*)));
	}

	setStyleSheet(TextAccess::createFromFile("data/gui/search_view/search_element.css")->getText().c_str());

	updatePlaceholder();

	layoutElements();
}

void QtSmartSearchBox::layoutElements()
{
	ensurePolished();

	int x = 7;
	bool hasSelected = hasSelectedElements();

	for (size_t i = 0; i <= m_elements.size(); i++)
	{
		if (!hasSelected && static_cast<int>(i) == m_cursorIndex)
		{
			int left, top, right, bottom;
			getTextMargins(&left, &top, &right, &bottom);
			setTextMargins(x - 9, top, right, bottom);
			x += fontMetrics().width(text());
		}

		if (i < m_elements.size())
		{
			QtQueryElement* button = m_elements[i].get();
			QSize size = button->minimumSizeHint();
			int y = (rect().height() - size.height()) / 2.0;
			button->setGeometry(x, y, size.width(), size.height());
			x += size.width() + 5;
		}
	}

	if (hasSelected)
	{
		setTextMargins(width() + 10, 0, 0, 0);
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
			str += m_tokens[i];
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

void QtSmartSearchBox::selectElementsTo(int idx, bool selected)
{
	int low = idx < m_cursorIndex ? idx : m_cursorIndex;
	int hi = idx > m_cursorIndex ? idx + 1 : m_cursorIndex;

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

	for (int i = m_elements.size() - 1; i >= 0; i--)
	{
		if (m_elements[i]->isChecked())
		{
			m_tokens.erase(m_tokens.begin() + i);

			if (i < m_cursorIndex)
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
		setPlaceholderText("Please enter your search string.");
	}
	else
	{
		setPlaceholderText("");
	}
}

void QtSmartSearchBox::clearLineEdit()
{
	setText("");

	if (completer())
	{
		completer()->popup()->hide();
	}
}
