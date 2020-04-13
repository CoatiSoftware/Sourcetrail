#ifndef QT_BOOKMARK_BUTTONS_VIEW_H
#define QT_BOOKMARK_BUTTONS_VIEW_H

#include "../../../lib/component/view/BookmarkButtonsView.h"

#include "../utility/QtThreadedFunctor.h"

class QFrame;
class QtSearchBarButton;

class QtBookmarkButtonsView
	: public QObject
	, public BookmarkButtonsView
{
	Q_OBJECT

public:
	QtBookmarkButtonsView(ViewLayout* viewLayout);
	~QtBookmarkButtonsView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// BookmarkView implementation
	void setCreateButtonState(const MessageBookmarkButtonState::ButtonState& state) override;

private slots:
	void createBookmarkClicked();
	void showBookmarksClicked();

private:
	QtThreadedLambdaFunctor m_onQtThread;

	QFrame* m_widget;

	QtSearchBarButton* m_createBookmarkButton;
	QtSearchBarButton* m_showBookmarksButton;

	MessageBookmarkButtonState::ButtonState m_createButtonState;
};

#endif	  // QT_BOOKMARK_BUTTONS_VIEW_H
