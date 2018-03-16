#ifndef QT_TABLE_H
#define QT_TABLE_H

#include <QTableView>

class QtTable
	: public QTableView
{
	Q_OBJECT

public:
	QtTable(QWidget* parent = nullptr);
	virtual ~QtTable();

	void updateRows();
	int getFilledRowCount();

	void showFirstRow();
	void showLastRow();

	bool hasSelection() const;

protected:
	virtual void resizeEvent(QResizeEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);

private:
	float m_rowsToFill;

	int m_colIndex = -1;
	int m_lastPos = -1;
};

#endif // QT_TABLE_H
