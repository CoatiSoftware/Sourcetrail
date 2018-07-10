#include "qt/window/QtWindowStack.h"

#include "qt/window/QtWindow.h"


QtWindowStackElement::QtWindowStackElement(QWidget* parent)
	: QWidget(parent)
{
}

QtWindowStack::QtWindowStack(QObject* parent)
	: QObject(parent)
{
}

QtWindowStackElement* QtWindowStack::getTopWindow() const
{
	if (m_stack.size())
	{
		return m_stack.back();
	}

	return nullptr;
}

QtWindowStackElement* QtWindowStack::getBottomWindow() const
{
	if (m_stack.size())
	{
		return m_stack.front();
	}

	return nullptr;
}

size_t QtWindowStack::getWindowCount() const
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
		m_stack.back()->deleteLater();
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

void QtWindowStack::centerSubWindows()
{
    for (QtWindowStackElement* window : m_stack)
    {
        QtWindow* qtWindow = dynamic_cast<QtWindow*>(window);
        if (qtWindow)
        {
            if (qtWindow->isSubWindow())
            {
                qtWindow->moveToCenter();
            }
        }
    }
}

void QtWindowStack::clearWindows()
{
	for (QtWindowStackElement* window : m_stack)
	{
		window->hideWindow();
		window->deleteLater();
	}

	m_stack.clear();

	emit empty();
}
