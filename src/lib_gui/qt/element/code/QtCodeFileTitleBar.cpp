#include "QtCodeFileTitleBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QVariant>

#include "Application.h"
#include "MessageErrorsForFile.h"
#include "Project.h"
#include "QtIconStateButton.h"
#include "QtSelfRefreshIconButton.h"
#include "ResourcePaths.h"

QtCodeFileTitleBar::QtCodeFileTitleBar(QWidget* parent, bool isHovering, bool isSingle)
	: QtHoverButton(parent)
{
	setObjectName(QStringLiteral("title_bar"));
	setProperty("hovering", isHovering);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac

	if (!isSingle)
	{
		connect(this, &QPushButton::clicked, this, &QtCodeFileTitleBar::clickedTitleBar);
	}

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	setLayout(titleLayout);

	FilePath imageDir = ResourcePaths::getGuiPath().concatenate(L"code_view/images/");

	m_expandButton = new QtSelfRefreshIconButton(
		QLatin1String(""),
		imageDir.getConcatenated(L"snippet_arrow_right.png"),
		"code/file/title",
		this);
	m_collapseButton = new QtSelfRefreshIconButton(
		QLatin1String(""), imageDir.getConcatenated(L"snippet_arrow_down.png"), "code/file/title", this);

	m_expandButton->setToolTip(QStringLiteral("expand"));
	m_collapseButton->setToolTip(QStringLiteral("collapse"));

	for (QtSelfRefreshIconButton* button: {m_expandButton, m_collapseButton})
	{
		button->setIconSize(QSize(9, 9));
		button->setObjectName(QStringLiteral("expand_button"));
		titleLayout->addWidget(button);
	}

	connect(
		m_expandButton,
		&QtSelfRefreshIconButton::clicked,
		this,
		&QtCodeFileTitleBar::clickedExpandButton);
	connect(
		m_collapseButton,
		&QtSelfRefreshIconButton::clicked,
		this,
		&QtCodeFileTitleBar::clickedCollapseButton);

	m_titleButton = new QtCodeFileTitleButton(this);
	QSizePolicy policy = m_titleButton->sizePolicy();
	policy.setRetainSizeWhenHidden(true);
	m_titleButton->setSizePolicy(policy);
	titleLayout->addWidget(m_titleButton);

	setMinimumHeight(m_titleButton->height() + 4);

	m_referenceCount = new QLabel(this);
	m_referenceCount->setObjectName(QStringLiteral("references_label"));
	m_referenceCount->hide();
	titleLayout->addWidget(m_referenceCount);

	titleLayout->addStretch(3);

	m_showErrorsButton = new QPushButton(QStringLiteral("show errors"));
	m_showErrorsButton->setObjectName(QStringLiteral("screen_button"));
	m_showErrorsButton->setToolTip(
		QStringLiteral("Show all errors causing this file to be incomplete"));
	m_showErrorsButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
	m_showErrorsButton->hide();
	titleLayout->addWidget(m_showErrorsButton);

	connect(m_showErrorsButton, &QPushButton::clicked, [this]() {
		MessageErrorsForFile(m_titleButton->getFilePath()).dispatch();
	});

	QColor inactiveColor(0x5E, 0x5D, 0x5D);

	m_snippetButton = new QtIconStateButton(this);
	m_snippetButton->addState(
		QtIconStateButton::STATE_DEFAULT, imageDir.getConcatenated(L"snippet_active.png"));
	m_snippetButton->addState(
		QtIconStateButton::STATE_HOVERED,
		imageDir.getConcatenated(L"snippet_inactive.png"),
		inactiveColor);
	m_snippetButton->addState(
		QtIconStateButton::STATE_DISABLED, imageDir.getConcatenated(L"snippet_inactive.png"));
	m_snippetButton->setToolTip(QStringLiteral("show snippets"));

	m_maximizeButton = new QtIconStateButton(this);
	m_maximizeButton->addState(
		QtIconStateButton::STATE_DEFAULT, imageDir.getConcatenated(L"maximize_active.png"));
	m_maximizeButton->addState(
		QtIconStateButton::STATE_HOVERED,
		imageDir.getConcatenated(L"maximize_inactive.png"),
		inactiveColor);
	m_maximizeButton->addState(
		QtIconStateButton::STATE_DISABLED, imageDir.getConcatenated(L"maximize_inactive.png"));
	m_maximizeButton->setToolTip(QStringLiteral("maximize"));

	for (QtIconStateButton* button: {m_snippetButton, m_maximizeButton})
	{
		button->setIconSize(QSize(16, 16));
		button->setObjectName(QStringLiteral("file_button"));
		button->setEnabled(false);
		titleLayout->addWidget(button);
	}

	connect(
		m_snippetButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedSnippetButton);
	connect(
		m_maximizeButton, &QtIconStateButton::clicked, this, &QtCodeFileTitleBar::clickedMaximizeButton);

	if (isSingle)
	{
		m_expandButton->hide();
		m_collapseButton->hide();
		m_maximizeButton->hide();
	}
	else
	{
		m_snippetButton->hide();
	}

	titleLayout->addSpacing(5);
}

QtCodeFileTitleButton* QtCodeFileTitleBar::getTitleButton() const
{
	return m_titleButton;
}

void QtCodeFileTitleBar::setIsComplete(bool isComplete)
{
	m_titleButton->setIsComplete(isComplete);
	m_showErrorsButton->setVisible(!isComplete);

	std::shared_ptr<const Project> project = Application::getInstance()->getCurrentProject();
	if (project && project->isIndexing())
	{
		m_showErrorsButton->setVisible(false);
	}
}

void QtCodeFileTitleBar::setIsIndexed(bool isIndexed)
{
	m_titleButton->setIsIndexed(isIndexed);
}

void QtCodeFileTitleBar::updateRefCount(int refCount, bool hasErrors, size_t fatalErrorCount)
{
	if (refCount > 0)
	{
		QString label = hasErrors ? QStringLiteral("error") : QStringLiteral("reference");
		if (refCount > 1)
		{
			label += QChar('s');
		}

		if (fatalErrorCount > 0)
		{
			label += QStringLiteral(" (") + QString::number(fatalErrorCount) +
				QStringLiteral(" fatal)");
		}

		QString text = QString::number(refCount) + QChar(' ') + label;
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

bool QtCodeFileTitleBar::isCollapsed() const
{
	return m_expandButton->isVisible();
}

void QtCodeFileTitleBar::setIsFocused(bool focused)
{
	setProperty("focused", focused);
	style()->polish(this);
}

void QtCodeFileTitleBar::setMinimized()
{
	m_expandButton->setVisible(true);
	m_collapseButton->setVisible(false);
	m_snippetButton->setEnabled(true);
	m_maximizeButton->setEnabled(true);

	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();
}

void QtCodeFileTitleBar::setSnippets()
{
	m_expandButton->setVisible(false);
	m_collapseButton->setVisible(true);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(true);

	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();
}

void QtCodeFileTitleBar::setMaximized()
{
	m_expandButton->setVisible(false);
	m_collapseButton->setVisible(false);
	m_snippetButton->setEnabled(true);
	m_maximizeButton->setEnabled(false);

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

	m_expandButton->setVisible(other->m_expandButton->isVisible());
	m_collapseButton->setVisible(other->m_collapseButton->isVisible());
	m_snippetButton->setEnabled(other->m_snippetButton->isEnabled());
	m_maximizeButton->setEnabled(other->m_maximizeButton->isEnabled());

	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();

	disconnect();

	connect(this, &QPushButton::clicked, this, &QtCodeFileTitleBar::clickedTitleBar);
}

void QtCodeFileTitleBar::clickedTitleBar()
{
	if (m_collapseButton->isVisible())
	{
		emit minimize();
	}
	else if (m_expandButton->isVisible())
	{
		emit snippet();
	}
	else
	{
		emit maximize();
	}
}

void QtCodeFileTitleBar::clickedExpandButton()
{
	emit snippet();
}

void QtCodeFileTitleBar::clickedCollapseButton()
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
