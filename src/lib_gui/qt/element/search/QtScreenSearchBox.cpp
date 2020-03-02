#include "QtScreenSearchBox.h"

#include <QApplication>
#include <QCheckBox>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#include "QtSelfRefreshIconButton.h"
#include "ResourcePaths.h"
#include "utilityQt.h"


QtFocusInFilter::QtFocusInFilter(QObject* parent): QObject(parent) {}

bool QtFocusInFilter::eventFilter(QObject* obj, QEvent* event)
{
	QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(obj);
	if (lineEdit && event->type() == QEvent::FocusIn &&
		dynamic_cast<QFocusEvent*>(event)->reason() == Qt::MouseFocusReason)
	{
		emit focusIn();
	}

	return QObject::eventFilter(obj, event);
}


QtScreenSearchBox::QtScreenSearchBox(
	ControllerProxy<ScreenSearchController>* controllerProxy, QWidget* parent)
	: QFrame(parent), m_controllerProxy(controllerProxy)
{
	setObjectName(QStringLiteral("screen_search_box"));

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	setLayout(layout);

	// search field
	{
		m_searchButton = new QtSelfRefreshIconButton(
			QLatin1String(""),
			ResourcePaths::getGuiPath().concatenate(L"search_view/images/search.png"),
			"screen_search/button");
		m_searchButton->setObjectName(QStringLiteral("search_button"));
		m_searchButton->setIconSize(QSize(12, 12));
		layout->addWidget(m_searchButton);

		connect(m_searchButton, &QPushButton::clicked, this, &QtScreenSearchBox::setFocus);

		m_searchBox = new QLineEdit(this);
		m_searchBox->setObjectName(QStringLiteral("search_box"));
		m_searchBox->setAttribute(Qt::WA_MacShowFocusRect, 0);	  // remove blue focus box on Mac
		layout->addWidget(m_searchBox);

		connect(m_searchBox, &QLineEdit::textChanged, this, &QtScreenSearchBox::searchQueryChanged);
		connect(m_searchBox, &QLineEdit::returnPressed, this, &QtScreenSearchBox::returnPressed);

		QtFocusInFilter* filter = new QtFocusInFilter(m_searchBox);
		m_searchBox->installEventFilter(filter);
		connect(filter, &QtFocusInFilter::focusIn, this, &QtScreenSearchBox::findMatches);
	}

	// match label
	{
		m_matchLabel = new QPushButton();
		m_matchLabel->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
		m_matchLabel->setObjectName(QStringLiteral("match_label"));
		layout->addWidget(m_matchLabel);

		connect(m_matchLabel, &QPushButton::clicked, this, &QtScreenSearchBox::setFocus);
	}

	// buttons
	{
		m_prevButton = new QtSelfRefreshIconButton(
			QLatin1String(""),
			ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_left.png"),
			"screen_search/button");
		m_nextButton = new QtSelfRefreshIconButton(
			QLatin1String(""),
			ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_right.png"),
			"screen_search/button");

		m_prevButton->setObjectName(QStringLiteral("prev_button"));
		m_nextButton->setObjectName(QStringLiteral("next_button"));

		m_prevButton->setIconSize(QSize(12, 12));
		m_nextButton->setIconSize(QSize(12, 12));

		layout->addWidget(m_prevButton);
		layout->addWidget(m_nextButton);

		connect(m_prevButton, &QPushButton::clicked, this, &QtScreenSearchBox::previousPressed);
		connect(m_nextButton, &QPushButton::clicked, this, &QtScreenSearchBox::nextPressed);
	}

	// filter
	{
		m_checkboxLayout = new QHBoxLayout();
		layout->addLayout(m_checkboxLayout);
		layout->addStretch();
	}

	// buttons
	{
		m_closeButton = new QtSelfRefreshIconButton(
			QLatin1String(""),
			ResourcePaths::getGuiPath().concatenate(L"screen_search_view/images/close.png"),
			"screen_search/button");
		m_closeButton->setObjectName(QStringLiteral("close_button"));
		m_closeButton->setIconSize(QSize(15, 15));
		layout->addWidget(m_closeButton);

		connect(m_closeButton, &QPushButton::clicked, [this]() { emit closePressed(); });
	}

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	connect(m_timer, &QTimer::timeout, this, &QtScreenSearchBox::findMatches);

	setMatchCount(0);
}

QtScreenSearchBox::~QtScreenSearchBox() {}

void QtScreenSearchBox::setMatchCount(size_t matchCount)
{
	m_matchCount = matchCount;
	m_matchIndex = 0;
	updateMatchLabel();

	m_prevButton->setEnabled(matchCount > 0);
	m_nextButton->setEnabled(matchCount > 0);
}

void QtScreenSearchBox::setMatchIndex(size_t matchIndex)
{
	m_matchIndex = matchIndex;
	updateMatchLabel();
}

void QtScreenSearchBox::addResponder(const std::string& name)
{
	if (m_checkBoxes.find(name) != m_checkBoxes.end())
	{
		return;
	}

	QCheckBox* box = new QCheckBox(name.c_str());
	box->setObjectName(QStringLiteral("filter_checkbox"));
	box->setChecked(true);
	m_checkBoxes.emplace(name, box);
	m_checkboxLayout->addWidget(box);

	connect(box, &QCheckBox::stateChanged, this, &QtScreenSearchBox::findMatches);
}

void QtScreenSearchBox::setFocus()
{
	m_searchBox->setFocus();

	if (m_searchBox->text().size())
	{
		m_searchBox->selectAll();
		searchQueryChanged();
	}
}

void QtScreenSearchBox::searchQueryChanged()
{
	m_timer->stop();
	m_timer->start(200);
}

void QtScreenSearchBox::findMatches()
{
	m_controllerProxy->executeAsTask([this](ScreenSearchController* controller) {
		std::set<std::string> responderNames;
		for (const auto& p: m_checkBoxes)
		{
			if (p.second->isChecked())
			{
				responderNames.insert(p.first);
			}
		}

		controller->search(m_searchBox->text().toLower().toStdWString(), responderNames);
	});
}

void QtScreenSearchBox::returnPressed()
{
	if (Qt::KeyboardModifier::ShiftModifier & QApplication::keyboardModifiers())
	{
		previousPressed();
	}
	else
	{
		nextPressed();
	}
}

void QtScreenSearchBox::previousPressed()
{
	activateMatch(false);
}

void QtScreenSearchBox::nextPressed()
{
	activateMatch(true);
}

void QtScreenSearchBox::activateMatch(bool next)
{
	m_controllerProxy->executeAsTaskWithArgs(&ScreenSearchController::activateMatch, next);
}

void QtScreenSearchBox::updateMatchLabel()
{
	QString text;

	if (m_matchIndex > 0)
	{
		text += QString::number(m_matchIndex) + " of ";
	}

	text += QString::number(m_matchCount) + " match";
	if (m_matchCount != 1)
	{
		text += QLatin1String("es");
	}

	m_matchLabel->setText(text);
}
