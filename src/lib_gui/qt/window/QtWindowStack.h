#ifndef QT_WINDOW_STACK
#define QT_WINDOW_STACK

#include <vector>

#include <QWidget>

class QtWindowStackElement
	: public QWidget
{
	Q_OBJECT
public:
	QtWindowStackElement(QWidget* parent = nullptr);

	virtual void showWindow() = 0;
	virtual void hideWindow() = 0;
};


class QtWindowStack
	: public QObject
{
	Q_OBJECT

signals:
	void empty();
	void pop();
	void push();

public:
	QtWindowStack(QObject* parent = nullptr);

	QtWindowStackElement* getTopWindow();
	QtWindowStackElement* getBottomWindow();

    size_t getWindowCount();

    void centerSubWindows();

public slots:
	void pushWindow(QtWindowStackElement* window);
	void popWindow();
	void clearWindows();

private:
	std::vector<QtWindowStackElement*> m_stack;
};

#endif // QT_WINDOW_STACK
