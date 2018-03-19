#include "qt/element/QtLocationPicker.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>

#include "utility/ResourcePaths.h"

#include "qt/element/QtIconButton.h"
#include "qt/utility/QtFileDialog.h"

QtLocationPicker::QtLocationPicker(QWidget *parent)
	: QWidget(parent)
	, m_pickDirectory(false)
{
	setObjectName("picker");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	m_data = new QtLineEdit(this);
	m_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_data->setObjectName("locationField");
	layout->addWidget(m_data);

	m_button = new QtIconButton(
		ResourcePaths::getGuiPath().concatenate(L"window/dots.png"),
		ResourcePaths::getGuiPath().concatenate(L"window/dots_hover.png")
	);
	m_button->setIconSize(QSize(16, 16));
	m_button->setObjectName("dotsButton");
	m_button->setToolTip("pick file");
	connect(m_button, &QPushButton::clicked, this, &QtLocationPicker::handleButtonPress);
	layout->addWidget(m_button);

	setLayout(layout);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
}

void QtLocationPicker::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);

	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void QtLocationPicker::setPlaceholderText(QString text)
{
	m_data->setPlaceholderText(text);
}

QString QtLocationPicker::getText()
{
	return m_data->text();
}

void QtLocationPicker::setText(QString text)
{
	m_data->setText(text);
}

void QtLocationPicker::clearText()
{
	m_data->clear();
}

bool QtLocationPicker::pickDirectory() const
{
	return m_pickDirectory;
}

void QtLocationPicker::setPickDirectory(bool pickDirectory)
{
	m_pickDirectory = pickDirectory;
}

void QtLocationPicker::setFileFilter(const QString& fileFilter)
{
	m_fileFilter = fileFilter;
}

void QtLocationPicker::setRelativeRootDirectory(const FilePath& dir)
{
	m_relativeRootDirectory = dir;
}

void QtLocationPicker::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::EnabledChange)
	{
		m_button->setVisible(isEnabled());
	}
}

void QtLocationPicker::handleButtonPress()
{
	FilePath path(m_data->text().toStdWString());
	if (!path.empty() && !path.isAbsolute() && !m_relativeRootDirectory.empty())
	{
		path = m_relativeRootDirectory.getConcatenated(path);
	}

	QString fileName;
	if (m_pickDirectory)
	{
		fileName = QtFileDialog::getExistingDirectory(this, tr("Select Directory"), path);
	}
	else
	{
		fileName = QtFileDialog::getOpenFileName(this, tr("Open File"), path, m_fileFilter);
	}

	if (!fileName.isEmpty())
	{
		if (!m_relativeRootDirectory.empty())
		{
			const FilePath path(fileName.toStdWString());
			const FilePath relPath = path.getRelativeTo(m_relativeRootDirectory);
			if (relPath.wstr().size() < path.wstr().size())
			{
				fileName = QString::fromStdWString(relPath.wstr());
			}
		}

		m_data->setText(fileName);
		emit locationPicked();
	}
}
