#include "CodeFocusHandler.h"

#include "MessageFocusIn.h"
#include "MessageFocusOut.h"
#include "MessageFocusView.h"

void CodeFocusHandler::clear()
{
	setCurrentFocus({});
	m_oldFocus = Focus();
	m_targetColumn = 0;
}

void CodeFocusHandler::focus()
{
	m_hasFocus = true;

	if (!m_oldFocus.isEmpty())
	{
		setCurrentFocus(m_oldFocus);
		m_oldFocus = Focus();
	}
}

void CodeFocusHandler::defocus()
{
	m_oldFocus = m_focus;
	setCurrentFocus({});
	m_hasFocus = false;
}

const CodeFocusHandler::Focus& CodeFocusHandler::getCurrentFocus() const
{
	return m_focus;
}

void CodeFocusHandler::setCurrentFocus(const Focus& focus)
{
	if (m_hasFocus)
	{
		bool same = (m_focus.tokenIds == focus.tokenIds);
		if (m_focus.tokenIds.size() && !same)
		{
			MessageFocusOut(m_focus.tokenIds).dispatch();
		}

		m_focus = focus;
		updateFiles();

		if (m_focus.tokenIds.size() && !same)
		{
			MessageFocusIn(m_focus.tokenIds).dispatch();
		}
	}
	else
	{
		m_oldFocus = focus;
	}
}

bool CodeFocusHandler::hasCurrentFocus() const
{
	if (m_hasFocus)
	{
		return !m_focus.isEmpty();
	}
	else
	{
		return !m_oldFocus.isEmpty();
	}
}

void CodeFocusHandler::setFocusedLocationId(
	QtCodeArea* area, size_t lineNumber, size_t columnNumber, Id locationId, const std::vector<Id>& tokenIds)
{
	if (columnNumber)
	{
		m_targetColumn = columnNumber;
	}

	setCurrentFocus({nullptr, area, nullptr, lineNumber, locationId, tokenIds});
}

void CodeFocusHandler::setFocusedScopeLine(QtCodeArea* area, QPushButton* scopeLine)
{
	setCurrentFocus({nullptr, area, scopeLine, 0, 0, {}});
}

void CodeFocusHandler::setFocusedFile(QtCodeFile* file)
{
	setCurrentFocus({file, nullptr, nullptr, 0, 0, {}});
}

size_t CodeFocusHandler::getTargetColumn() const
{
	return m_targetColumn;
}

void CodeFocusHandler::focusView()
{
	if (!m_hasFocus)
	{
		MessageFocusView(MessageFocusView::ViewType::CODE).dispatch();
	}
}
