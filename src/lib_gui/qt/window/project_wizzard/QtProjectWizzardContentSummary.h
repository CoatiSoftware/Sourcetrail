#ifndef QT_PROJECT_WIZZARD_CONTENT_SUMMARY_H
#define QT_PROJECT_WIZZARD_CONTENT_SUMMARY_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QCheckBox;

class QtProjectWizzardContentSummary
	: public QtProjectWizzardContent
{
	Q_OBJECT

private:
	struct Element
	{
		QtProjectWizzardContent* content;
		bool advanced;
		bool gapBefore;
	};

public:
	QtProjectWizzardContentSummary(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	void addContent(QtProjectWizzardContent* content, bool advanced, bool gapBefore);
	void setIsForm(bool isForm);

protected:
	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout, int& row) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

	virtual bool isScrollAble() const override;

private slots:
	void advancedToggled(bool checked);

private:
	std::vector<Element> m_elements;

	QGridLayout* m_layout;
	QCheckBox* m_checkBox;
	int m_checkBoxRow;

	bool m_isForm;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SUMMARY_H
