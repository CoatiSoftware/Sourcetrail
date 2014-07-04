#ifndef QT_EDIT_BOX_H
#define QT_EDIT_BOX_H

#include <functional>

#include <QLineEdit>

class QtEditBox: public QLineEdit
{
	Q_OBJECT

public:
	QtEditBox(QWidget *parent);
	~QtEditBox();

	void setCallbackOnReturnPressed(std::function<void(void)> callback);

private slots:
	void slotOnReturnPressed();

private:
	std::function<void(void)> m_onReturnPressed;
};

#endif // QT_EDIT_BOX_H
