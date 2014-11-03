#ifndef QT_SMART_SEARCH_BOX_H
#define QT_SMART_SEARCH_BOX_H

#include <deque>
#include <memory>
#include <vector>

#include <QLineEdit>
#include <QPushButton>

#include "data/search/SearchMatch.h"

class QtQueryElement
	: public QPushButton
{
	Q_OBJECT

signals:
	void wasChecked(QtQueryElement*);

public:
	QtQueryElement(const QString& text, QWidget* parent);

private slots:
	void onChecked(bool);
};


class QtSmartSearchBox
	: public QLineEdit
{
	Q_OBJECT

public slots:
	void search();

public:
	QtSmartSearchBox(QWidget* parent);
	virtual ~QtSmartSearchBox();

	void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList);
	void setQuery(const std::string& text);
	void setFocus();

protected:
	virtual bool event(QEvent *event);
	virtual void resizeEvent(QResizeEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);

private slots:
	void onTextEdited(const QString& text);
	void onTextChanged(const QString& text);

	void onAutocompletionHighlighted(const SearchMatch& match);
	void onAutocompletionActivated(const SearchMatch& match);

	void onElementSelected(QtQueryElement* element);

private:
	void moveCursor(int offset);
	void moveCursorTo(int goal);

	void textToToken(std::string text);
	void setEditText(const QString& text);
	bool editTextToElement();
	void editElement(QtQueryElement* element);

	void updateElements();
	void layoutElements();

	bool hasSelectedElements() const;
	std::string getSelectedString() const;

	void selectAllElementsWith(bool selected);
	void selectElementsTo(size_t idx, bool selected);
	void deleteSelectedElements();

	void updatePlaceholder();
	void clearLineEdit();

	void requestAutoCompletions() const;
	void hideAutoCompletions();

	bool m_allowTextChange;
	QString m_oldText;

	std::deque<std::string> m_tokens;
	std::vector<std::shared_ptr<QtQueryElement>> m_elements;

	size_t m_cursorIndex;

	SearchMatch m_highlightedMatch;

	bool m_shiftKeyDown;
	bool m_mousePressed;
	int m_mouseX;
};

#endif // QT_SMART_SEARCH_BOX_H
