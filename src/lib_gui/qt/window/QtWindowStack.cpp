#include "qt/window/QtWindowStack.h"

QtWindowStack::QtWindowStack(QObject* parent)
	: QObject(parent)
{
}

QWidget* QtWindowStack::getTopWindow()
{
	if (m_stack.size())
	{
		return m_stack.back();
	}

	return nullptr;
}

void QtWindowStack::pushWindow(QWidget* window)
{
	if (m_stack.size())
	{
		m_stack.back()->hide();
	}

	window->show();

	m_stack.push_back(window);

	emit push();
}

void QtWindowStack::popWindow()
{
	if (m_stack.size())
	{
		m_stack.back()->hide();
		delete m_stack.back();
		m_stack.pop_back();

		emit pop();
	}

	if (m_stack.size())
	{
		m_stack.back()->show();
	}
	else
	{
		emit empty();
	}
}

void QtWindowStack::clearWindows()
{
	for (QWidget* window : m_stack)
	{
		window->hide();
		delete window;
	}

	m_stack.clear();

	emit empty();
}
