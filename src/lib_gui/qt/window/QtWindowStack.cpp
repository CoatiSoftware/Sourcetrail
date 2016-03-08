#include "qt/window/QtWindowStack.h"


QtWindowStackElement::QtWindowStackElement(QWidget* parent)
	: QWidget(parent)
{
}

QtWindowStack::QtWindowStack(QObject* parent)
	: QObject(parent)
{
}

QtWindowStackElement* QtWindowStack::getTopWindow()
{
	if (m_stack.size())
	{
		return m_stack.back();
	}

	return nullptr;
}

size_t QtWindowStack::getWindowCount()
{
	return m_stack.size();
}

void QtWindowStack::pushWindow(QtWindowStackElement* window)
{
	if (m_stack.size())
	{
		m_stack.back()->hideWindow();
	}

	window->showWindow();

	m_stack.push_back(window);

	emit push();
}

void QtWindowStack::popWindow()
{
	if (m_stack.size())
	{
		m_stack.back()->hideWindow();
		delete m_stack.back();
		m_stack.pop_back();

		emit pop();
	}

	if (m_stack.size())
	{
		m_stack.back()->showWindow();
	}
	else
	{
		emit empty();
	}
}

void QtWindowStack::clearWindows()
{
	for (QtWindowStackElement* window : m_stack)
	{
		window->hideWindow();
		delete window;
	}

	m_stack.clear();

	emit empty();
}
