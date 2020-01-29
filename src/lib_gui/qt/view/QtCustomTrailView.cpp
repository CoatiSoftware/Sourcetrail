#include "QtCustomTrailView.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QRadioButton>
#include <QSlider>

#include "ColorScheme.h"
#include "MessageActivateTrail.h"
#include "NodeTypeSet.h"
#include "QtMainWindow.h"
#include "QtSmartSearchBox.h"
#include "ResourcePaths.h"
#include "TabId.h"
#include "utilityQt.h"

QtCustomTrailView::QtCustomTrailView(ViewLayout* viewLayout)
	: QWidget(utility::getMainWindowforMainView(viewLayout))
	, CustomTrailView(nullptr)
	, m_controllerProxy(this, TabId::app())
{
	setWindowTitle(QStringLiteral("Custom Trail"));
	setWindowFlags(Qt::Window);

	QGridLayout* mainLayout = new QGridLayout();
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);

	QWidget* panelA1 = new QWidget();
	QWidget* panelA2 = new QWidget();
	QWidget* panelB1 = new QWidget();
	QWidget* panelB2 = new QWidget();
	QWidget* panelC1 = new QWidget();
	QWidget* panelC2 = new QWidget();
	QWidget* panelD = new QWidget();

	panelA1->setObjectName(QStringLiteral("panelA"));
	panelA2->setObjectName(QStringLiteral("panelA"));
	panelB1->setObjectName(QStringLiteral("panelB"));
	panelB2->setObjectName(QStringLiteral("panelB"));
	panelC1->setObjectName(QStringLiteral("panelA"));
	panelC2->setObjectName(QStringLiteral("panelA"));
	panelD->setObjectName(QStringLiteral("panelB2"));

	mainLayout->addWidget(panelA1, 0, 0);
	mainLayout->addWidget(panelA2, 0, 1);
	mainLayout->addWidget(panelB1, 1, 0);
	mainLayout->addWidget(panelB2, 1, 1);
	mainLayout->addWidget(panelC1, 2, 0);
	mainLayout->addWidget(panelC2, 2, 1);
	mainLayout->addWidget(panelD, 3, 0, 1, 2);

	// search boxes
	{
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->setContentsMargins(25, 10, 10, 10);
		panelA1->setLayout(hLayout);

		m_searchBoxFrom = new QtSmartSearchBox(QStringLiteral("Start Symbol"), false);
		m_searchBoxTo = new QtSmartSearchBox(QStringLiteral("Target Symbol"), false);

		hLayout->addWidget(new QLabel(QStringLiteral("From:")));
		hLayout->addWidget(createSearchBox(m_searchBoxFrom));
		hLayout->addSpacing(15);

		QVBoxLayout* optionsLayout = new QVBoxLayout();
		optionsLayout->setContentsMargins(10, 10, 25, 10);
		panelA2->setLayout(optionsLayout);

		m_optionTo = new QRadioButton(QString::fromUtf8("\xe2\x86\x92") + " To:");
		m_optionReferenced = new QRadioButton(QString::fromUtf8("\xe2\x86\x92") + " All Referenced");
		m_optionReferencing = new QRadioButton(
			QString::fromUtf8("\xe2\x86\x90") + " All Referencing");

		m_optionTo->setAttribute(Qt::WA_LayoutUsesWidgetRect);			   // fixes layouting on Mac
		m_optionReferenced->setAttribute(Qt::WA_LayoutUsesWidgetRect);	   // fixes layouting on Mac
		m_optionReferencing->setAttribute(Qt::WA_LayoutUsesWidgetRect);	   // fixes layouting on Mac

		m_optionTo->setChecked(true);

		QButtonGroup* options = new QButtonGroup();
		options->addButton(m_optionTo);
		options->addButton(m_optionReferenced);
		options->addButton(m_optionReferencing);

		QWidget* searchBoxToContainer = createSearchBox(m_searchBoxTo);
		QHBoxLayout* toLayout = new QHBoxLayout();
		toLayout->setContentsMargins(0, 0, 0, 0);
		toLayout->addWidget(m_optionTo);
		toLayout->addWidget(searchBoxToContainer);

		optionsLayout->addLayout(toLayout);
		optionsLayout->addWidget(m_optionReferenced);
		optionsLayout->addSpacing(7);
		optionsLayout->addWidget(m_optionReferencing);

		connect(
			options,
			QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
			[this, searchBoxToContainer](QAbstractButton* button) {
				searchBoxToContainer->setEnabled(button == m_optionTo);
			});

		connect(
			m_searchBoxFrom,
			&QtSmartSearchBox::autocomplete,
			[this](const std::wstring& query, NodeTypeSet acceptedNodeTypes) {
				m_controllerProxy.executeAsTaskWithArgs(
					&CustomTrailController::autocomplete, query, true);
			});

		connect(
			m_searchBoxTo,
			&QtSmartSearchBox::autocomplete,
			[this](const std::wstring& query, NodeTypeSet acceptedNodeTypes) {
				m_controllerProxy.executeAsTaskWithArgs(
					&CustomTrailController::autocomplete, query, false);
			});
	}

	// depth slider
	{
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->setContentsMargins(25, 10, 10, 10);
		panelB1->setLayout(hLayout);

		hLayout->addWidget(new QLabel(QStringLiteral("Max Depth:")));

		m_slider = new QSlider(Qt::Horizontal);
		m_slider->setObjectName(QStringLiteral("depth_slider"));
		m_slider->setToolTip(QStringLiteral("adjust graph depth"));
		m_slider->setMinimum(1);
		m_slider->setMaximum(51);
		m_slider->setMinimumWidth(150);
		m_slider->setValue(INITIAL_GRAPH_DEPTH);

		hLayout->addWidget(m_slider);

		QLabel* valueLabel = new QLabel(QString::number(INITIAL_GRAPH_DEPTH));
		valueLabel->setMinimumWidth(20);
		hLayout->addWidget(valueLabel);

		connect(m_slider, &QSlider::valueChanged, [this, valueLabel](int) {
			if (m_slider->value() == m_slider->maximum())
			{
				valueLabel->setText(QStringLiteral("inf"));
			}
			else
			{
				valueLabel->setText(QString::number(m_slider->value()));
			}
		});

		hLayout->addStretch();
	}

	// layout direction
	{
		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->setContentsMargins(10, 10, 25, 10);
		hLayout->setSpacing(15);
		panelB2->setLayout(hLayout);

		m_horizontalButton = new QRadioButton(QStringLiteral("Horizontal"));
		m_verticalButton = new QRadioButton(QStringLiteral("Vertical"));

		m_horizontalButton->setChecked(true);

		hLayout->addWidget(new QLabel(QStringLiteral("Layout Direction:")));
		hLayout->addWidget(m_horizontalButton);
		hLayout->addWidget(m_verticalButton);

		QButtonGroup* layouts = new QButtonGroup();
		layouts->addButton(m_horizontalButton);
		layouts->addButton(m_verticalButton);

		hLayout->addStretch();
	}

	ColorScheme* scheme = ColorScheme::getInstance().get();

	// node filters
	{
		std::vector<QString> nodeFilters;
		std::vector<QColor> nodeColors;

		std::vector<NodeType::Type> nodeTypes = {// NodeType::NODE_SYMBOL,
												 NodeType::NODE_TYPE,
												 NodeType::NODE_BUILTIN_TYPE,
												 // NodeType::NODE_MODULE,
												 // NodeType::NODE_NAMESPACE,
												 // NodeType::NODE_PACKAGE,
												 NodeType::NODE_CLASS,
												 NodeType::NODE_STRUCT,
												 NodeType::NODE_UNION,
												 NodeType::NODE_INTERFACE,
												 NodeType::NODE_TYPEDEF,
												 NodeType::NODE_TYPE_PARAMETER,
												 NodeType::NODE_ENUM,
												 NodeType::NODE_ENUM_CONSTANT,
												 NodeType::NODE_GLOBAL_VARIABLE,
												 NodeType::NODE_FIELD,
												 NodeType::NODE_FUNCTION,
												 NodeType::NODE_METHOD,
												 NodeType::NODE_FILE,
												 NodeType::NODE_MACRO,
												 NodeType::NODE_ANNOTATION};

		for (NodeType::Type t: nodeTypes)
		{
			nodeFilters.push_back(QString::fromStdString(NodeType::getReadableTypeString(t)));
			nodeColors.push_back(
				QColor(scheme->getNodeTypeColor(t, "fill", ColorScheme::FOCUS).c_str()));
		}

		QVBoxLayout* filterLayout = addFilters(QStringLiteral("Nodes:"), nodeFilters, nodeColors, &m_nodeFilters, 11);
		filterLayout->setContentsMargins(25, 10, 25, 10);
		panelC1->setLayout(filterLayout);
	}

	// edge filters
	{
		std::vector<QString> edgeFilters;
		std::vector<QColor> edgeColors;

		std::vector<Edge::EdgeType> edgeTypes = {
			// Edge::EDGE_UNDEFINED,
			Edge::EDGE_TYPE_USAGE,
			Edge::EDGE_INHERITANCE,
			Edge::EDGE_USAGE,
			Edge::EDGE_CALL,
			Edge::EDGE_OVERRIDE,
			// Edge::EDGE_TEMPLATE_ARGUMENT, // merged with type use
			// Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT, // merged with type use
			Edge::EDGE_TEMPLATE_SPECIALIZATION,
			// Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION, // merged with above
			Edge::EDGE_TYPE_ARGUMENT,
			Edge::EDGE_INCLUDE,
			Edge::EDGE_IMPORT,
			// Edge::EDGE_AGGREGATION,
			Edge::EDGE_MACRO_USAGE,
			Edge::EDGE_ANNOTATION_USAGE
			// Edge::EDGE_MEMBER // has separate checkbox
		};

		for (Edge::EdgeType t: edgeTypes)
		{
			edgeFilters.push_back(QString::fromStdWString(Edge::getReadableTypeString(t)));
			edgeColors.push_back(QColor(scheme->getEdgeTypeColor(t, ColorScheme::FOCUS).c_str()));
		}

		QVBoxLayout* filterLayout = addFilters(QStringLiteral("Edges:"), edgeFilters, edgeColors, &m_edgeFilters, 5);
		filterLayout->setContentsMargins(10, 10, 25, 10);
		panelC2->setLayout(filterLayout);
	}

	// controls
	{
		QPushButton* cancelButton = new QPushButton(QStringLiteral("Cancel"));
		QPushButton* searchButton = new QPushButton(QStringLiteral("Search"));

		cancelButton->setObjectName(QStringLiteral("button"));
		searchButton->setObjectName(QStringLiteral("button"));

		cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
		searchButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac

		m_errorLabel = new QLabel();
		m_errorLabel->setObjectName(QStringLiteral("error"));

		QHBoxLayout* hLayout = new QHBoxLayout();
		hLayout->setContentsMargins(25, 15, 25, 15);
		hLayout->addWidget(cancelButton);
		hLayout->addStretch();
		hLayout->addWidget(m_errorLabel);
		hLayout->addSpacing(10);
		hLayout->addWidget(searchButton);
		panelD->setLayout(hLayout);

		connect(cancelButton, &QPushButton::clicked, [this]() { hide(); });

		connect(searchButton, &QPushButton::clicked, [this]() {
			if (m_searchBoxFrom->getMatches().size() == 0 ||
				m_searchBoxFrom->getMatches().front().tokenIds.size() == 0)
			{
				setError(QStringLiteral("No 'Start Symbol' symbol found."));
				return;
			}

			Id startId = m_searchBoxFrom->getMatches().front().tokenIds.front();
			Id endId = 0;

			if (m_optionReferencing->isChecked())
			{
				std::swap(startId, endId);
			}
			else if (m_optionTo->isChecked())
			{
				if (m_searchBoxTo->getMatches().size() &&
					m_searchBoxTo->getMatches().front().tokenIds.size())
				{
					endId = m_searchBoxTo->getMatches().front().tokenIds.front();
				}
				else
				{
					setError(QStringLiteral("No 'Target Symbol' symbol found."));
					return;
				}
			}

			NodeType::TypeMask nodeTypes = getCheckedNodeTypes();
			Edge::TypeMask edgeTypes = getCheckedEdgeTypes();

			if (!nodeTypes)
			{
				setError(QStringLiteral("No 'Nodes' selected."));
				return;
			}

			if (!edgeTypes)
			{
				setError(QStringLiteral("No 'Edges' selected."));
				return;
			}

			MessageActivateTrail message(
				startId,
				endId,
				nodeTypes,
				edgeTypes,
				m_nodeNonIndexed->isChecked(),
				m_slider->value() == m_slider->maximum() ? 0 : m_slider->value(),
				m_horizontalButton->isChecked());

			m_controllerProxy.executeAsTaskWithArgs(&CustomTrailController::activateTrail, message);

			setError(QLatin1String(""));

			hide();
		});
	}
}

void QtCustomTrailView::createWidgetWrapper() {}

void QtCustomTrailView::refreshView()
{
	m_onQtThread([this]() {
		updateStyleSheet();
		m_searchBoxFrom->refreshStyle();
		m_searchBoxTo->refreshStyle();
	});
}

void QtCustomTrailView::clearView()
{
	m_onQtThread([this]() {
		m_searchBoxFrom->setMatches({});
		m_searchBoxTo->setMatches({});
	});
}

void QtCustomTrailView::setAvailableNodeAndEdgeTypes(NodeType::TypeMask nodeTypes, Edge::TypeMask edgeTypes)
{
	m_onQtThread([this, nodeTypes, edgeTypes]() {
		for (QCheckBox* filter: m_nodeFilters)
		{
			if (filter == m_nodeNonIndexed)
			{
				continue;
			}

			bool enabled = nodeTypes &
				NodeType::getTypeForReadableTypeString(filter->text().toStdWString());
			filter->setEnabled(enabled);
			filter->setVisible(enabled);
		}

		for (QCheckBox* filter: m_edgeFilters)
		{
			bool enabled = false;
			if (filter == m_edgeMember)
			{
				enabled = edgeTypes & Edge::EDGE_MEMBER;
			}
			else
			{
				enabled = edgeTypes &
					Edge::getTypeForReadableTypeString(filter->text().toStdWString());
			}

			filter->setEnabled(enabled);
			filter->setVisible(enabled);
		}
	});
}

void QtCustomTrailView::showView()
{
	m_onQtThread([this]() {
		show();
		raise();
	});
}

void QtCustomTrailView::hideView()
{
	m_onQtThread([this]() { hide(); });
}

void QtCustomTrailView::showAutocompletions(const std::vector<SearchMatch>& autocompletions, bool from)
{
	m_onQtThread([this, autocompletions, from]() {
		if (from)
		{
			m_searchBoxFrom->setAutocompletionList(autocompletions);
		}
		else
		{
			m_searchBoxTo->setAutocompletionList(autocompletions);
		}
	});
}

void QtCustomTrailView::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		hide();
		return;
	}

	QWidget::keyPressEvent(event);
}

void QtCustomTrailView::updateStyleSheet()
{
	std::string css = utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(L"search_view/search_view.css"));
	css += utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(L"custom_trail_view/custom_trail_view.css"));

	setStyleSheet(css.c_str());

	QAbstractItemView* popup = m_searchBoxFrom->getCompleter()->popup();
	if (popup)
	{
		popup->setStyleSheet(css.c_str());
	}

	popup = m_searchBoxTo->getCompleter()->popup();
	if (popup)
	{
		popup->setStyleSheet(css.c_str());
	}
}

QWidget* QtCustomTrailView::createSearchBox(QtSmartSearchBox* searchBox) const
{
	QWidget* searchBoxContainer = new QWidget();
	searchBoxContainer->setObjectName(QStringLiteral("search_box_container"));
	searchBoxContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	searchBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

	QBoxLayout* innerLayout = new QVBoxLayout();
	innerLayout->setContentsMargins(12, 3, 5, 2);
	innerLayout->addWidget(searchBox);
	searchBoxContainer->setLayout(innerLayout);

	return searchBoxContainer;
}

QVBoxLayout* QtCustomTrailView::addFilters(
	QString name,
	const std::vector<QString>& filters,
	const std::vector<QColor>& colors,
	std::vector<QCheckBox*>* checkBoxes,
	size_t filtersInFirstColumn)
{
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(new QLabel(name));

	QHBoxLayout* mainLayout = new QHBoxLayout();
	vLayout->addLayout(mainLayout);

	QVBoxLayout* filterALayout = new QVBoxLayout();
	QVBoxLayout* filterBLayout = new QVBoxLayout();

	mainLayout->addLayout(filterALayout);
	mainLayout->addLayout(filterBLayout);

	QPixmap pixmap(QString::fromStdString(
		ResourcePaths::getGuiPath().concatenate(L"custom_trail_view/images/circle.png").str()));
	for (size_t i = 0; i < filters.size(); i++)
	{
		QCheckBox* checkBox = new QCheckBox(filters[i]);
		checkBox->setChecked(true);
		checkBox->setIcon(QIcon(utility::colorizePixmap(pixmap, colors[i])));
		checkBoxes->push_back(checkBox);

		if (i < filtersInFirstColumn)
		{
			filterALayout->addWidget(checkBox);
		}
		else
		{
			filterBLayout->addWidget(checkBox);
		}
	}

	if (checkBoxes == &m_nodeFilters)
	{
		m_nodeNonIndexed = new QCheckBox(QStringLiteral("non-indexed"));
		m_nodeNonIndexed->setChecked(true);
		checkBoxes->push_back(m_nodeNonIndexed);

		filterBLayout->addSpacing(7);
		filterBLayout->addWidget(m_nodeNonIndexed);
	}
	else if (checkBoxes == &m_edgeFilters)
	{
		m_edgeMember = new QCheckBox(QStringLiteral("member"));
		m_edgeMember->setChecked(true);
		checkBoxes->push_back(m_edgeMember);

		filterBLayout->addSpacing(5);
		filterBLayout->addWidget(m_edgeMember);
	}

	filterALayout->addStretch();
	filterBLayout->addStretch();

	vLayout->addLayout(addCheckButtons(*checkBoxes));
	vLayout->addStretch();

	return vLayout;
}

QHBoxLayout* QtCustomTrailView::addCheckButtons(const std::vector<QCheckBox*>& checkBoxes) const
{
	QHBoxLayout* buttonLayout = new QHBoxLayout();

	QPushButton* checkButton = new QPushButton(QStringLiteral("Check All"));
	QPushButton* uncheckButton = new QPushButton(QStringLiteral("Uncheck All"));

	checkButton->setObjectName(QStringLiteral("button_small"));
	uncheckButton->setObjectName(QStringLiteral("button_small"));

	buttonLayout->addWidget(checkButton);
	buttonLayout->addWidget(uncheckButton);
	buttonLayout->addStretch();

	connect(checkButton, &QPushButton::clicked, [&checkBoxes]() {
		for (QCheckBox* box: checkBoxes)
		{
			if (box->isEnabled())
			{
				box->setChecked(true);
			}
		}
	});

	connect(uncheckButton, &QPushButton::clicked, [&checkBoxes]() {
		for (QCheckBox* box: checkBoxes)
		{
			box->setChecked(false);
		}
	});

	return buttonLayout;
}

NodeType::TypeMask QtCustomTrailView::getCheckedNodeTypes() const
{
	NodeType::TypeMask nodeTypes = 0;
	for (const QCheckBox* filter: m_nodeFilters)
	{
		if (filter->isEnabled() && filter->isChecked() && filter != m_nodeNonIndexed)
		{
			nodeTypes |= NodeType::getTypeForReadableTypeString(filter->text().toStdWString());
		}
	}
	return nodeTypes;
}

Edge::TypeMask QtCustomTrailView::getCheckedEdgeTypes() const
{
	Edge::TypeMask edgeTypes = 0;
	for (const QCheckBox* filter: m_edgeFilters)
	{
		if (filter->isEnabled() && filter->isChecked() && filter != m_edgeMember)
		{
			Edge::EdgeType type = Edge::getTypeForReadableTypeString(filter->text().toStdWString());
			edgeTypes |= type;
		}
	}

	if (m_edgeMember->isChecked())
	{
		edgeTypes |= Edge::EDGE_MEMBER;
	}

	return edgeTypes;
}

void QtCustomTrailView::setError(const QString& error)
{
	m_errorLabel->setText(error);
}
