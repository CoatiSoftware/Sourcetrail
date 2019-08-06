#ifndef QT_SELF_REFRESH_ICON_BUTTON_H
#define QT_SELF_REFRESH_ICON_BUTTON_H

#include <QPushButton>
#include <QTimer>

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

	void setText(const QString& text);
	void setIconPath(const FilePath& iconPath);

	void setAutoElide(bool autoElide);

protected:
	void handleMessage(MessageRefreshUI* message) override;

	virtual void refresh();

	void resizeEvent(QResizeEvent *event) override;

private:
	void updateText(int width);

	QtThreadedLambdaFunctor m_onQtThread;

	QString m_text;
	FilePath m_iconPath;
	const std::string m_buttonKey;

	bool m_autoElide = false;
	bool m_blockUpdate = false;
	QTimer m_updateTimer;
	QTimer m_blockTimer;
};

#endif // QT_SELF_REFRESH_ICON_BUTTON_H
