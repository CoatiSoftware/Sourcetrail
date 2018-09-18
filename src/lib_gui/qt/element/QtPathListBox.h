#ifndef QT_PATH_LIST_BOX_H
#define QT_PATH_LIST_BOX_H

#include "QtListBox.h"

class QtPathListBox
	: public QtListBox
{
public:
	enum SelectionPolicyType
	{
		SELECTION_POLICY_FILES_ONLY,
		SELECTION_POLICY_DIRECTORIES_ONLY,
		SELECTION_POLICY_FILES_AND_DIRECTORIES
	};

	QtPathListBox(QWidget *parent, const QString& listName, SelectionPolicyType selectionPolicy);

	SelectionPolicyType getSelectionPolicy() const;

	const FilePath& getRelativeRootDirectory() const;
	void setRelativeRootDirectory(const FilePath& dir);

	std::vector<FilePath> getPathsAsDisplayed() const;
	std::vector<FilePath> getPathsAsAbsolute() const;
	void setPaths(const std::vector<FilePath>& list, bool readOnly = false);

	void makeAbsolute(FilePath& path) const;
	void makeRelativeIfShorter(FilePath& path) const;

protected:
	void dropEvent(QDropEvent *event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;

private:
	virtual QtListBoxItem* createListBoxItem(QListWidgetItem* item) override;

	const SelectionPolicyType m_selectionPolicy;
	FilePath m_relativeRootDirectory;
};

#endif // QT_PATH_LIST_BOX_H
