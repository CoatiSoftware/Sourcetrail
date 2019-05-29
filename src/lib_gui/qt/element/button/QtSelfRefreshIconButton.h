#ifndef QT_SELF_REFRESH_ICON_BUTTON_H
#define QT_SELF_REFRESH_ICON_BUTTON_H

#include <QPushButton>

#include "FilePath.h"
#include "MessageListener.h"
#include "MessageRefreshUI.h"
#include "QtThreadedFunctor.h"

class QtSelfRefreshIconButton
	: public QPushButton
	, public MessageListener<MessageRefreshUI>
{
public:
	QtSelfRefreshIconButton(
		const QString& text, const FilePath& iconPath, const std::string& buttonKey, QWidget* parent = nullptr);
	~QtSelfRefreshIconButton() = default;

	void setIconPath(const FilePath& iconPath);

protected:
	void handleMessage(MessageRefreshUI* message) override;

	virtual void refresh();

private:
	QtThreadedLambdaFunctor m_onQtThread;

	FilePath m_iconPath;
	const std::string m_buttonKey;
};

#endif // QT_SELF_REFRESH_ICON_BUTTON_H
