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
	void setCallbackOnTextEdited(std::function<void(const std::string&)> callback);

private slots:
	void slotOnReturnPressed();
	void slotOnTextEdited(const QString& text);

private:
	std::function<void(void)> m_onReturnPressed;
	std::function<void(const std::string&)> m_onTextEdited;
};

#endif // QT_EDIT_BOX_H
