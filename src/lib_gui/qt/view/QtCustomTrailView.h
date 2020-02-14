#ifndef QT_CUSTOM_TRAIL_VIEW_H
#define QT_CUSTOM_TRAIL_VIEW_H

#include <QWidget>

#include "ControllerProxy.h"
#include "CustomTrailController.h"
#include "CustomTrailView.h"
#include "QtThreadedFunctor.h"

class QCheckBox;
class QHBoxLayout;
class QLabel;
class QSlider;
class QRadioButton;
class QtSmartSearchBox;
class QVBoxLayout;

class QtCustomTrailView
	: public QWidget
	, public CustomTrailView
{
public:
	QtCustomTrailView(ViewLayout*);

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// TrailView implementation
	void clearView() override;
	void setAvailableNodeAndEdgeTypes(NodeKindMask nodeTypes, Edge::TypeMask edgeTypes) override;

	void showView() override;
	void hideView() override;
	void showAutocompletions(const std::vector<SearchMatch>& autocompletions, bool from) override;

protected:
	void keyPressEvent(QKeyEvent* event) override;

private:
	void updateStyleSheet();

	QWidget* createSearchBox(QtSmartSearchBox* searchBox) const;
	QVBoxLayout* addFilters(
		const QString& name,
		const std::vector<QString>& filters,
		const std::vector<QColor>& colors,
		std::vector<QCheckBox*>* checkBoxes,
		size_t filtersInFirstColumn);
	QHBoxLayout* addCheckButtons(const std::vector<QCheckBox*>& checkBoxes) const;

	NodeKindMask getCheckedNodeTypes() const;
	Edge::TypeMask getCheckedEdgeTypes() const;

	void setError(const QString& error);

	static const size_t INITIAL_GRAPH_DEPTH = 5;
	static const size_t FILTERS_PER_COLUMN = 10;

	QtThreadedLambdaFunctor m_onQtThread;

	ControllerProxy<CustomTrailController> m_controllerProxy;

	QtSmartSearchBox* m_searchBoxFrom;
	QtSmartSearchBox* m_searchBoxTo;

	QRadioButton* m_optionReferenced;
	QRadioButton* m_optionReferencing;
	QRadioButton* m_optionTo;

	QSlider* m_slider;

	QRadioButton* m_horizontalButton;
	QRadioButton* m_verticalButton;

	std::vector<QCheckBox*> m_nodeFilters;
	std::vector<QCheckBox*> m_edgeFilters;

	QCheckBox* m_nodeNonIndexed;
	QCheckBox* m_edgeMember;

	QLabel* m_errorLabel;
};

#endif	  // QT_CUSTOM_TRAIL_VIEW_H
