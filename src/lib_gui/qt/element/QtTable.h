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

protected:
	virtual void resizeEvent(QResizeEvent* event);

private:
	float m_rowsToFill;
};

#endif // QT_TABLE_H
