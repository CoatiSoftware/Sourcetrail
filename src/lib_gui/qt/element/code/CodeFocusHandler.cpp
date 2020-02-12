#include "CodeFocusHandler.h"

void CodeFocusHandler::clear()
{
	m_focus = Focus();
	m_oldFocus = Focus();
	m_targetColumn = 0;
}

void CodeFocusHandler::focus()
{
	m_hasFocus = true;

	if (!m_oldFocus.isEmpty())
	{
		m_focus = m_oldFocus;
	}
	else
	{
		focusInitialLocation();
	}
	updateFiles();
}

void CodeFocusHandler::defocus()
{
	m_hasFocus = false;

	m_oldFocus = m_focus;
	m_focus = Focus();

	updateFiles();
}

const CodeFocusHandler::Focus& CodeFocusHandler::getCurrentFocus() const
{
	return m_focus;
}

void CodeFocusHandler::setCurrentFocus(const Focus& focus)
{
	if (m_hasFocus)
	{
		m_focus = focus;
		updateFiles();
	}
	else
	{
		m_oldFocus = focus;
	}
}

void CodeFocusHandler::setFocusedLocationId(
	QtCodeArea* area, size_t lineNumber, size_t columnNumber, Id locationId)
{
	if (columnNumber)
	{
		m_targetColumn = columnNumber;
	}

	setCurrentFocus({nullptr, area, nullptr, lineNumber, locationId});
}

void CodeFocusHandler::setFocusedScopeLine(QtCodeArea* area, QPushButton* scopeLine)
{
	setCurrentFocus({nullptr, area, scopeLine, 0, 0});
}

void CodeFocusHandler::setFocusedFile(QtCodeFile* file)
{
	setCurrentFocus({file, nullptr, nullptr, 0, 0});
}

size_t CodeFocusHandler::getTargetColumn() const
{
	return m_targetColumn;
}
