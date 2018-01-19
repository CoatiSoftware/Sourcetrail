#ifndef QT_SCREEN_SEARCH_BOX_H
#define QT_SCREEN_SEARCH_BOX_H

#include <QFrame>

#include "component/controller/helper/ControllerProxy.h"
#include "component/controller/ScreenSearchController.h"

class QCheckBox;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QTimer;


class QtFocusInFilter
	: public QObject
{
	Q_OBJECT

public:
	QtFocusInFilter(QObject* parent = Q_NULLPTR);

signals:
	void focusIn();

protected:
	bool eventFilter(QObject* obj, QEvent* event);
};


class QtScreenSearchBox
	: public QFrame
{
	Q_OBJECT

public:
	QtScreenSearchBox(ControllerProxy<ScreenSearchController>* controllerProxy, QWidget* parent = nullptr);
	virtual ~QtScreenSearchBox();

	void refreshStyle();

	void setMatchCount(size_t matchCount);
	void setMatchIndex(size_t matchIndex);

	void addResponder(const std::string& name);

signals:
	void closePressed();

public slots:
	void setFocus();

private slots:
	void searchQueryChanged();
	void findMatches();
	void returnPressed();

	void previousPressed();
	void nextPressed();

private:
	void activateMatch(bool next);

	void updateMatchLabel();

	ControllerProxy<ScreenSearchController>* m_controllerProxy;

	QLineEdit* m_searchBox;
	QPushButton* m_matchLabel;

	QPushButton* m_searchButton;
	QPushButton* m_prevButton;
	QPushButton* m_nextButton;
	QPushButton* m_closeButton;

	QHBoxLayout* m_checkboxLayout;
	std::vector<QCheckBox*> m_checkBoxes;

	size_t m_matchCount = 0;
	size_t m_matchIndex = 0;

	QTimer* m_timer;
};

#endif // QT_SCREEN_SEARCH_BOX_H
