#ifndef QT_WINDOW_STACK
#define QT_WINDOW_STACK

#include <vector>

#include <QWidget>

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

	QWidget* getTopWindow();

public slots:
	void pushWindow(QWidget* window);
	void popWindow();
	void clearWindows();

private:
	std::vector<QWidget*> m_stack;
};

#endif // QT_WINDOW_STACK
