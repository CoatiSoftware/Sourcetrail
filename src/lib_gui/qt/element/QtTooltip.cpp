#include "QtTooltip.h"

#include <QApplication>
#include <QCursor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QStyle>
#include <QTimer>

#include "ApplicationSettings.h"
#include "QtCodeField.h"
#include "SourceLocationFile.h"
#include "TextCodec.h"
#include "TooltipInfo.h"

QtTooltip::QtTooltip(QWidget* parent): QFrame(parent), m_parentView(nullptr), m_isHovered(false)
{
	QWidget::setWindowFlags(Qt::ToolTip);
	setObjectName(QStringLiteral("tooltip"));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	setLayout(layout);
}

QtTooltip::~QtTooltip() {}

void QtTooltip::setTooltipInfo(const TooltipInfo& info)
{
	int maxWidth = 600;
	QWidget* parent = m_parentView ? m_parentView : parentWidget();
	if (parent)
	{
		maxWidth = std::max(maxWidth, parent->width() - 50);
	}

	if (!info.title.empty())
	{
		addTitle(QString::fromStdWString(info.title), info.count, info.countText.c_str());
	}

	for (const TooltipSnippet& snippet: info.snippets)
	{
		const TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());

		QtCodeField* field = new QtCodeField(
			1, codec.encode(snippet.code), snippet.locationFile, false);

		QSize size = field->sizeHint() + QSize(15, 5);
		if (size.width() > maxWidth)
		{
			field->setMinimumSize(QSize(
				maxWidth,
				size.height() + QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent)));
		}
		else
		{
			field->setMinimumSize(size);
			field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}

		field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		field->annotateText();

		addWidget(field);
	}

	m_offset = QPoint(info.offset.x(), info.offset.y());
}

void QtTooltip::setParentView(QWidget* parentView)
{
	m_parentView = parentView;
}

bool QtTooltip::isHovered() const
{
	return m_isHovered;
}

void QtTooltip::show()
{
	QWidget* parent = m_parentView ? m_parentView : parentWidget();
	if (!parent)
	{
		return;
	}

	QWidget::show();

	QPoint pos = QCursor::pos() + m_offset;

	if (pos.x() + width() > parent->pos().x() + parent->width())
	{
		pos.setX(pos.x() - width() - m_offset.x() * 2);
	}

	if (pos.x() < parent->pos().x())
	{
		pos.setX(parent->pos().x() + 10);
	}


	if (pos.y() + height() > parent->pos().y() + parent->height())
	{
		pos.setY(pos.y() - height() - m_offset.y() * 2);
	}

	if (pos.y() < parent->pos().y())
	{
		pos.setY(parent->pos().y() + 10);
	}

	move(pos);
}

void QtTooltip::hide(bool force)
{
	if (!m_isHovered || force)
	{
		QWidget::hide();

		clearLayout(layout());
		m_parentView = nullptr;
		m_offset = QPoint();
	}
}

void QtTooltip::leaveEvent(QEvent* event)
{
	m_isHovered = false;

	QTimer::singleShot(500, this, SLOT(hide()));
}

void QtTooltip::enterEvent(QEvent* event)
{
	m_isHovered = true;
}

void QtTooltip::addTitle(const QString& title, int count, const QString& countText)
{
	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setContentsMargins(0, 0, 0, 0);
	titleLayout->setSpacing(0);

	QLabel* titleLabel = new QLabel(title);
	titleLabel->setObjectName(QStringLiteral("tooltip_title"));
	titleLayout->addWidget(titleLabel);

	if (count >= 0)
	{
		QLabel* referenceLabel = new QLabel(
			QString::number(count) + " " + countText + (count != 1 ? "s" : ""));
		referenceLabel->setObjectName(QStringLiteral("tooltip_references"));

		titleLayout->addWidget(referenceLabel, 0, Qt::AlignRight);
	}

	QWidget* titleWidget = new QWidget();
	titleWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	titleWidget->setLayout(titleLayout);

	layout()->addWidget(titleWidget);
}

void QtTooltip::addWidget(QWidget* widget)
{
	widget->setObjectName(QStringLiteral("tooltip_widget"));
	layout()->addWidget(widget);
}

void QtTooltip::clearLayout(QLayout* layout)
{
	while (QLayoutItem* item = layout->takeAt(0))
	{
		if (QWidget* widget = item->widget())
		{
			widget->deleteLater();
		}
		if (QLayout* childLayout = item->layout())
		{
			clearLayout(childLayout);
		}
		delete item;
	}
}
