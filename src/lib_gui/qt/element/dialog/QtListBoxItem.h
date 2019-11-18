#ifndef QT_LIST_BOX_ITEM_H
#define QT_LIST_BOX_ITEM_H

#include <QWidget>

class QtLineEdit;
class QListWidget;
class QPushButton;
class QListWidgetItem;

class QtListBox;

class QtListBoxItem: public QWidget
{
	Q_OBJECT

public:
	QtListBoxItem(QListWidgetItem* item, QWidget* parent = nullptr);
	virtual ~QtListBoxItem() = default;

	virtual QString getText() const;
	void setText(const QString& text);

	bool getReadOnly() const;
	void setReadOnly(bool readOnly);

public slots:
	void setFocus();

protected:
	virtual QtListBox* getListBox() = 0;
	void selectItem();

private slots:
	void handleFocus();

private:
	virtual void onReadOnlyChanged();

	QListWidgetItem* m_item;
	QtLineEdit* m_data;
	bool m_readOnly;
};

#endif	  // QT_LIST_BOX_ITEM_H
