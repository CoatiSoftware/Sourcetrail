#ifndef QT_FILE_DIALOG_H
#define QT_FILE_DIALOG_H

class QString;
class QStringList;
class QWidget;

class QtFileDialog
{
public:
	static QStringList getFileNamesAndDirectories(QWidget* parent, const QString& dir);

	static QString getExistingDirectory(QWidget* parent, const QString& caption, const QString& dir);
	static QString getOpenFileName(QWidget* parent, const QString& caption, const QString& dir, const QString& filter);

	static QString showSaveFileDialog(
		QWidget *parent, const QString& title, const QString& directory, const QString& filter);

private:
	static QString getDir(QString dir);
};

#endif // QT_FILE_DIALOG_H
