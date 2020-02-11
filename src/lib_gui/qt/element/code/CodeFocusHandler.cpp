#include "CodeFocusHandler.h"

void CodeFocusHandler::clear()
{
	m_focus = Focus();
	m_oldFocus = Focus();
	m_targetColumn = 0;
}

void CodeFocusHandler::focus()
{
	if (!m_oldFocus.isEmpty())
	{
		m_focus = m_oldFocus;
	}
	else
	{
		// focusInitialLocation();
	}
}

void CodeFocusHandler::defocus()
{
	m_oldFocus = m_focus;
	m_focus = Focus();
}

const CodeFocusHandler::Focus& CodeFocusHandler::getFocus() const
{
	return m_focus;
}

void CodeFocusHandler::setFocusedLocationId(
	QtCodeArea* area, size_t lineNumber, size_t columnNumber, Id locationId)
{
	m_focus = {nullptr, area, nullptr, lineNumber, locationId};

	if (columnNumber)
	{
		m_targetColumn = columnNumber;
	}
}

void CodeFocusHandler::setFocusedScopeLine(QtCodeArea* area, QPushButton* scopeLine)
{
	m_focus = {nullptr, area, scopeLine, 0, 0};
}

void CodeFocusHandler::setFocusedFile(QtCodeFile* file)
{
	m_focus = {file, nullptr, nullptr, 0, 0};
}

size_t CodeFocusHandler::getTargetColumn() const
{
	return m_targetColumn;
}
