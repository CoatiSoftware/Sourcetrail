#ifndef QT_WINDOW_H
#define QT_WINDOW_H

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QSizeGrip>
#include <QWidget>

#include "QtWindowStack.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

class QtWindow
	: public QtWindowStackElement
{
	Q_OBJECT

public:
	QtWindow(bool isSubWindow, QWidget* parent = nullptr);

	QSize sizeHint() const override;

	void setup();

	void setSizeGripStyle(bool isBlack);

	void setCancelAble(bool cancelAble);
	void setScrollAble(bool scrollAble);

	bool isScrollAble() const;
    bool isSubWindow() const;

    void moveToCenter();

	void updateTitle(const QString& title);
	std::wstring getTitle() const;
	void updateSubTitle(QString subTitle);

	void updateNextButton(QString text);
	void updatePreviousButton(QString text);
	void updateCloseButton(QString text);

	void setNextEnabled(bool enabled);
	void setPreviousEnabled(bool enabled);
	void setCloseEnabled(bool enabled);

	void setNextVisible(bool visible);
	void setPreviousVisible(bool visible);
	void setCloseVisible(bool visible);

	void setNextDefault(bool isDefault);
	void setPreviousDefault(bool isDefault);
	void setCloseDefault(bool isDefault);

	// QtWindowStackElement implementation
	virtual void showWindow() override;
	virtual void hideWindow() override;


signals:
	void finished();
	void canceled();

	void next();
	void previous();

protected:
	void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	virtual void populateWindow(QWidget* widget);
	virtual void windowReady();

	virtual void handleNext();
	virtual void handlePrevious();
	virtual void handleClose();

	void setupDone();
	void addLogo();
	QHBoxLayout* createButtons();

	bool m_isSubWindow;

	QWidget* m_window;
	QWidget* m_content;

	QLabel* m_title;
	QLabel* m_subTitle;

	QPushButton* m_nextButton;
	QPushButton* m_previousButton;
	QPushButton* m_closeButton;

public slots:
	void handleNextPress(bool);
	void handlePreviousPress();
	void handleClosePress();

private:
	bool m_cancelAble;
	bool m_scrollAble;

	bool m_hasLogo;

	QPoint m_dragPosition;
	bool m_mousePressedInWindow;

    QSizeGrip* m_sizeGrip;
};

#endif // QT_WINDOW_H
