#include "QtCodeFileTitleBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVariant>

#include "ResourcePaths.h"
#include "MessageErrorsForFile.h"

QtCodeFileTitleBar::QtCodeFileTitleBar(QWidget* parent, bool isHovering, bool isSingle)
	: QtHoverButton(parent)
{
	setObjectName("title_bar");
	setProperty("hovering", isHovering);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	if (!isSingle)
	{
		connect(this, &QPushButton::clicked, this, &QtCodeFileTitleBar::clickedTitleBar);
		connect(this, &QtHoverButton::hoveredIn, this, &QtCodeFileTitleBar::enteredTitleBar);
		connect(this, &QtHoverButton::hoveredOut, this, &QtCodeFileTitleBar::leftTitleBar);
	}

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	setLayout(titleLayout);

	m_titleButton = new QtCodeFileTitleButton(this);
	QSizePolicy policy = m_titleButton->sizePolicy();
	policy.setRetainSizeWhenHidden(true);
	m_titleButton->setSizePolicy(policy);
	titleLayout->addWidget(m_titleButton);

	setMinimumHeight(m_titleButton->height() + 4);

	m_referenceCount = new QLabel(this);
	m_referenceCount->setObjectName("references_label");
	m_referenceCount->hide();
	titleLayout->addWidget(m_referenceCount);

	titleLayout->addStretch(3);

	m_showErrorsButton = new QPushButton("show errors");
	m_showErrorsButton->setObjectName("screen_button");
	m_showErrorsButton->setToolTip("Show all errors causing this file to be incomplete");
	m_showErrorsButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_showErrorsButton->hide();
	titleLayout->addWidget(m_showErrorsButton);

	connect(m_showErrorsButton, &QPushButton::clicked,
		[this]()
		{
			MessageErrorsForFile(m_titleButton->getFilePath()).dispatch();
		}
	);

	FilePath imageDir = ResourcePaths::getGuiPath().concatenate(L"code_view/images/");
	QColor inactiveColor(0x5E, 0x5D, 0x5D);

	m_minimizeButton = new QtIconStateButton(this);
	m_minimizeButton->addState(QtIconStateButton::STATE_DEFAULT, imageDir.getConcatenated(L"minimize_active.png"));
	m_minimizeButton->addState(QtIconStateButton::STATE_HOVERED, imageDir.getConcatenated(L"minimize_inactive.png"), inactiveColor);
	m_minimizeButton->addState(QtIconStateButton::STATE_DISABLED, imageDir.getConcatenated(L"minimize_inactive.png"));
	m_minimizeButton->setToolTip("minimize");

	m_snippetButton = new QtIconStateButton(this);
	m_snippetButton->addState(QtIconStateButton::STATE_DEFAULT, imageDir.getConcatenated(L"snippet_active.png"));
	m_snippetButton->addState(QtIconStateButton::STATE_HOVERED, imageDir.getConcatenated(L"snippet_inactive.png"), inactiveColor);
	m_snippetButton->addState(QtIconStateButton::STATE_DISABLED, imageDir.getConcatenated(L"snippet_inactive.png"));
	m_snippetButton->setToolTip("show snippets");

	m_maximizeButton = new QtIconStateButton(this);
	m_maximizeButton->addState(QtIconStateButton::STATE_DEFAULT, imageDir.getConcatenated(L"maximize_active.png"));
	m_maximizeButton->addState(QtIconStateButton::STATE_HOVERED, imageDir.getConcatenated(L"maximize_inactive.png"), inactiveColor);
	m_maximizeButton->addState(QtIconStateButton::STATE_DISABLED, imageDir.getConcatenated(L"maximize_inactive.png"));
	m_maximizeButton->setToolTip("maximize");

	for (QtIconStateButton* button : { m_minimizeButton, m_snippetButton, m_maximizeButton })
	{
		button->setIconSize(QSize(16, 16));
		button->setObjectName("file_button");
		button->setEnabled(false);
		titleLayout->addWidget(button);

		if (!isSingle)
		{
			connect(button, &QtIconStateButton::hoveredIn, this, &QtCodeFileTitleBar::leftTitleBar);
			connect(button, &QtIconStateButton::hoveredOut, this, &QtCodeFileTitleBar::enteredTitleBar);
		}
	}

	connect(m_minimizeButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedMinimizeButton);
	connect(m_snippetButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedSnippetButton);
	connect(m_maximizeButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedMaximizeButton);

	if (isSingle)
	{
		m_minimizeButton->hide();
		m_maximizeButton->hide();
	}

	titleLayout->addSpacing(3);
}

QtCodeFileTitleButton* QtCodeFileTitleBar::getTitleButton() const
{
	return m_titleButton;
}

void QtCodeFileTitleBar::setIsComplete(bool isComplete)
{
	m_titleButton->setIsComplete(isComplete);
	m_showErrorsButton->setVisible(!isComplete);
}

void QtCodeFileTitleBar::setIsIndexed(bool isIndexed)
{
	m_titleButton->setIsIndexed(isIndexed);
}

void QtCodeFileTitleBar::updateRefCount(int refCount, bool hasErrors, size_t fatalErrorCount)
{
	if (refCount > 0)
	{
		QString label = hasErrors ? "error" : "reference";
		if (refCount > 1)
		{
			label += "s";
		}

		if (fatalErrorCount > 0)
		{
			label += " (" + QString::number(fatalErrorCount) + " fatal)";
		}

		QString text = QString::number(refCount) + " " + label;
		if (text != m_referenceCount->text())
		{
			m_referenceCount->setText(text);
		}
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}
}

void QtCodeFileTitleBar::setMinimized()
{
	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(true);
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

void QtCodeFileTitleBar::setMaximized()
{
	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(true);
	m_maximizeButton->setEnabled(false);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();
}

void QtCodeFileTitleBar::updateFromOther(const QtCodeFileTitleBar* other)
{
	m_titleButton->updateFromOther(other->getTitleButton());
	setIsComplete(m_titleButton->isComplete());

	QString refString = other->m_referenceCount->text();
	if (refString.size())
	{
		m_referenceCount->setText(refString);
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}

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
