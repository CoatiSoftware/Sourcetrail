#include "qt/element/QtCodeFileTitleBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVariant>

#include "utility/ResourcePaths.h"

QtCodeFileTitleBar::QtCodeFileTitleBar(QWidget* parent, bool isHovering)
	: QtHoverButton(parent)
{
	setObjectName("title_widget");
	setProperty("hovering", isHovering);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	setLayout(titleLayout);

	m_titleButton = new QtCodeFileTitleButton(this);
	titleLayout->addWidget(m_titleButton);
	setMinimumHeight(m_titleButton->height() + 4);

	m_referenceCount = new QLabel(this);
	m_referenceCount->setObjectName("references_label");
	m_referenceCount->hide();
	titleLayout->addWidget(m_referenceCount);

	titleLayout->addStretch(3);

	std::string imageDir = ResourcePaths::getGuiPath().str() + "code_view/images/";

	m_minimizeButton = new QtIconStateButton(this);
	m_minimizeButton->addState(QtIconStateButton::STATE_DEFAULT, (imageDir + "minimize_active.png").c_str());
//	m_minimizeButton->addState(QtIconStateButton::STATE_HOVERED, (imageDir + "minimize_inactive.png").c_str(), "#5E5D5D");
	m_minimizeButton->addState(QtIconStateButton::STATE_HOVERED, (imageDir + "minimize_inactive.png").c_str(), QColor(0x5E, 0x5D, 0x5D));
	m_minimizeButton->addState(QtIconStateButton::STATE_DISABLED, (imageDir + "minimize_inactive.png").c_str());
	m_minimizeButton->setIconSize(QSize(16, 16));
	m_minimizeButton->setObjectName("file_button");
	m_minimizeButton->setToolTip("minimize");
	titleLayout->addWidget(m_minimizeButton);

	m_snippetButton = new QtIconStateButton(this);
	m_snippetButton->addState(QtIconStateButton::STATE_DEFAULT, (imageDir + "snippet_active.png").c_str());
//	m_snippetButton->addState(QtIconStateButton::STATE_HOVERED, (imageDir + "snippet_inactive.png").c_str(), "#5E5D5D");
	m_snippetButton->addState(QtIconStateButton::STATE_HOVERED, (imageDir + "snippet_inactive.png").c_str(), QColor(0x5E, 0x5D, 0x5D));
	m_snippetButton->addState(QtIconStateButton::STATE_DISABLED, (imageDir + "snippet_inactive.png").c_str());
	m_snippetButton->setIconSize(QSize(16, 16));
	m_snippetButton->setObjectName("file_button");
	m_snippetButton->setToolTip("show snippets");
	titleLayout->addWidget(m_snippetButton);

	m_maximizeButton = new QtIconStateButton(this);
	m_maximizeButton->addState(QtIconStateButton::STATE_DEFAULT, (imageDir + "maximize_active.png").c_str());
//	m_maximizeButton->addState(QtIconStateButton::STATE_HOVERED, (imageDir + "maximize_inactive.png").c_str(), "#5E5D5D");
	m_maximizeButton->addState(QtIconStateButton::STATE_HOVERED, (imageDir + "maximize_inactive.png").c_str(), QColor(0x5E, 0x5D, 0x5D));
	m_maximizeButton->addState(QtIconStateButton::STATE_DISABLED, (imageDir + "maximize_inactive.png").c_str());
	m_maximizeButton->setIconSize(QSize(16, 16));
	m_maximizeButton->setObjectName("file_button");
	m_maximizeButton->setToolTip("maximize");
	titleLayout->addWidget(m_maximizeButton);

	titleLayout->addSpacing(3);

	connect(this, &QPushButton::clicked, this, &QtCodeFileTitleBar::clickedTitleBar);
	connect(this, &QtHoverButton::hoveredIn, this, &QtCodeFileTitleBar::enteredTitleBar);
	connect(this, &QtHoverButton::hoveredOut, this, &QtCodeFileTitleBar::leftTitleBar);

	connect(m_minimizeButton, &QtIconStateButton::hoveredIn, this, &QtCodeFileTitleBar::leftTitleBar);
	connect(m_minimizeButton, &QtIconStateButton::hoveredOut, this, &QtCodeFileTitleBar::enteredTitleBar);
	connect(m_minimizeButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedMinimizeButton);

	connect(m_snippetButton, &QtIconStateButton::hoveredIn, this, &QtCodeFileTitleBar::leftTitleBar);
	connect(m_snippetButton, &QtIconStateButton::hoveredOut, this, &QtCodeFileTitleBar::enteredTitleBar);
	connect(m_snippetButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedSnippetButton);

	connect(m_maximizeButton, &QtIconStateButton::hoveredIn, this, &QtCodeFileTitleBar::leftTitleBar);
	connect(m_maximizeButton, &QtIconStateButton::hoveredOut, this, &QtCodeFileTitleBar::enteredTitleBar);
	connect(m_maximizeButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedMaximizeButton);

	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(false);
}

QtCodeFileTitleButton* QtCodeFileTitleBar::getTitleButton() const
{
	return m_titleButton;
}

void QtCodeFileTitleBar::setRefString(const QString& refString)
{
	if (refString.size())
	{
		m_referenceCount->setText(refString);
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}
}

void QtCodeFileTitleBar::setMinimized(bool hasSnippets)
{
	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(hasSnippets);
	m_maximizeButton->setEnabled(true);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();
}

void QtCodeFileTitleBar::setSnippets()
{
	m_minimizeButton->setEnabled(true);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(true);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();
}

void QtCodeFileTitleBar::setMaximized(bool hasSnippets)
{
	m_minimizeButton->setEnabled(true);
	m_snippetButton->setEnabled(hasSnippets);
	m_maximizeButton->setEnabled(false);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();
}

void QtCodeFileTitleBar::updateFromOther(const QtCodeFileTitleBar* other)
{
	m_titleButton->updateFromOther(other->getTitleButton());

	setRefString(other->m_referenceCount->text());

	m_minimizeButton->setEnabled(other->m_minimizeButton->isEnabled());
	m_snippetButton->setEnabled(other->m_snippetButton->isEnabled());
	m_maximizeButton->setEnabled(other->m_maximizeButton->isEnabled());

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();

	disconnect();

	connect(this, &QPushButton::clicked, this, &QtCodeFileTitleBar::clickedTitleBar);
	connect(this, &QtHoverButton::hoveredIn, this, &QtCodeFileTitleBar::enteredTitleBar);
	connect(this, &QtHoverButton::hoveredOut, this, &QtCodeFileTitleBar::leftTitleBar);
}

void QtCodeFileTitleBar::clickedTitleBar()
{
	if (m_minimizeButton->isEnabled())
	{
		emit minimize();
	}
	else if (m_snippetButton->isEnabled())
	{
		emit snippet();
	}
	else
	{
		emit maximize();
	}
}

void QtCodeFileTitleBar::enteredTitleBar(QPushButton* button)
{
	if (m_minimizeButton->isEnabled())
	{
		m_minimizeButton->hoverIn();
	}
	else if (m_snippetButton->isEnabled())
	{
		m_snippetButton->hoverIn();
	}
	else if (m_maximizeButton->isEnabled())
	{
		m_maximizeButton->hoverIn();
	}
}

void QtCodeFileTitleBar::leftTitleBar(QPushButton* button)
{
	if (m_minimizeButton->isEnabled())
	{
		if (m_minimizeButton != button)
		{
			m_minimizeButton->hoverOut();
		}
	}
	else if (m_snippetButton->isEnabled())
	{
		if (m_snippetButton != button)
		{
			m_snippetButton->hoverOut();
		}
	}
	else if (m_maximizeButton->isEnabled())
	{
		if (m_maximizeButton != button)
		{
			m_maximizeButton->hoverOut();
		}
	}
}

void QtCodeFileTitleBar::clickedMinimizeButton()
{
	emit minimize();
}

void QtCodeFileTitleBar::clickedSnippetButton()
{
	emit snippet();
}

void QtCodeFileTitleBar::clickedMaximizeButton()
{
	emit maximize();
}
