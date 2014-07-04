#ifndef QT_BUTTON_H
#define QT_BUTTON_H

#include <functional>

#include <QPushButton>

class QtButton: public QPushButton
{
	Q_OBJECT

public:
	QtButton(QWidget *parent);
	~QtButton();

	void setCallbackOnClick(std::function<void(void)> callback);

private slots:
	void slotOnClick();

private:
	std::function<void(void)> m_onClick;
};

#endif // QT_BUTTON_H
