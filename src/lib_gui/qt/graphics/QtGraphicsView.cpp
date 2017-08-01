#include "qt/graphics/QtGraphicsView.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSysInfo>
#include <QTimer>

#include <QApplication>
#include <QClipboard>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "qt/view/graphElements/QtGraphNode.h"
#include "qt/view/graphElements/QtGraphNodeData.h"
#include "qt/view/graphElements/QtGraphNodeBundle.h"
#include "qt/utility/QtContextMenu.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "utility/messaging/type/MessageDisplayBookmarkCreator.h"
#include "utility/ResourcePaths.h"

QtGraphicsView::QtGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
	, m_zoomFactor(1.0f)
	, m_appZoomFactor(1.0f)
	, m_up(false)
	, m_down(false)
	, m_left(false)
	, m_right(false)
	, m_shift(false)
	, m_zoomInButtonSpeed(20.0f)
	, m_zoomOutButtonSpeed(-20.0f)
{
	QString modifierName = QSysInfo::macVersion() == QSysInfo::MV_None ? "Ctrl" : "Cmd";

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	m_timer = std::make_shared<QTimer>(this);
	connect(m_timer.get(), &QTimer::timeout, this, &QtGraphicsView::updateTimer);

	m_timerStopper = std::make_shared<QTimer>(this);
	m_timerStopper->setSingleShot(true);
	connect(m_timerStopper.get(), &QTimer::timeout, this, &QtGraphicsView::stopTimer);

	m_zoomLabelTimer = std::make_shared<QTimer>(this);
	connect(m_zoomLabelTimer.get(), &QTimer::timeout, this, &QtGraphicsView::hideZoomLabel);

	m_exportGraphAction = new QAction(tr("Save as Image"), this);
	m_exportGraphAction->setStatusTip(tr("Save this graph as image file"));
	m_exportGraphAction->setToolTip(tr("Save this graph as image file"));
	connect(m_exportGraphAction, &QAction::triggered, this, &QtGraphicsView::exportGraph);

	m_copyNodeNameAction = new QAction(tr("Copy Name"), this);
	m_copyNodeNameAction->setStatusTip(tr("Copies the name of this node to the clipboard"));
	m_copyNodeNameAction->setToolTip(tr("Copies the name of this node to the clipboard"));
	connect(m_copyNodeNameAction, &QAction::triggered, this, &QtGraphicsView::copyNodeName);

	m_bookmarkNodeAction = new QAction(tr("Bookmark Node"), this);
	m_bookmarkNodeAction->setStatusTip(tr("Create a bookmark for this node"));
	m_bookmarkNodeAction->setToolTip(tr("Create a bookmark for this node"));
	connect(m_bookmarkNodeAction, &QAction::triggered, this, &QtGraphicsView::bookmarkNode);

	m_zoomState = new QPushButton(this);
	m_zoomState->setObjectName("zoom_state");
	m_zoomState->hide();

	m_zoomInButton = new QPushButton(this);
	m_zoomInButton->setObjectName("zoom_in_button");
	m_zoomInButton->setAutoRepeat(true);
	m_zoomInButton->setToolTip("Zoom in (" + modifierName + " + Mousewheel forward)");
	connect(m_zoomInButton, &QPushButton::pressed, this, &QtGraphicsView::zoomInPressed);

	m_zoomOutButton = new QPushButton(this);
	m_zoomOutButton->setObjectName("zoom_out_button");
	m_zoomOutButton->setAutoRepeat(true);
	m_zoomOutButton->setToolTip("Zoom out (" + modifierName + " + Mousewheel back)");
	connect(m_zoomOutButton, &QPushButton::pressed, this, &QtGraphicsView::zoomOutPressed);

	refreshStyle();
}

float QtGraphicsView::getZoomFactor() const
{
	return m_appZoomFactor * m_zoomFactor;
}

void QtGraphicsView::setAppZoomFactor(float appZoomFactor)
{
	m_appZoomFactor = appZoomFactor;
	updateTransform();
}

QtGraphNode* QtGraphicsView::getNodeAtCursorPosition() const
{
	QtGraphNode* node = nullptr;

	QPointF point = mapToScene(mapFromGlobal(QCursor::pos()));
	QGraphicsItem* item = scene()->itemAt(point, QTransform());
	if (item)
	{
		node = dynamic_cast<QtGraphNode*>(item->parentItem());
	}

	return node;
}

void QtGraphicsView::ensureVisibleAnimated(const QRectF& rect, int xmargin, int ymargin)
{
	int xval = horizontalScrollBar()->value();
	int yval = verticalScrollBar()->value();

	ensureVisible(rect, xmargin, ymargin);

	if (ApplicationSettings::getInstance()->getUseAnimations())
	{
		int xval2 = horizontalScrollBar()->value();
		int yval2 = verticalScrollBar()->value();

		horizontalScrollBar()->setValue(xval);
		verticalScrollBar()->setValue(yval);

		QParallelAnimationGroup* move = new QParallelAnimationGroup();

		QPropertyAnimation* xanim = new QPropertyAnimation(horizontalScrollBar(), "value");
		xanim->setDuration(150);
		xanim->setStartValue(xval);
		xanim->setEndValue(xval2);
		xanim->setEasingCurve(QEasingCurve::InOutQuad);
		move->addAnimation(xanim);

		QPropertyAnimation* yanim = new QPropertyAnimation(verticalScrollBar(), "value");
		yanim->setDuration(150);
		yanim->setStartValue(yval);
		yanim->setEndValue(yval2);
		yanim->setEasingCurve(QEasingCurve::InOutQuad);
		move->addAnimation(yanim);

		move->start();
	}
}

void QtGraphicsView::updateZoom(float delta)
{
	float factor = 1.0f + 0.001f * delta;

	if (factor <= 0.0f)
	{
		factor = 0.000001;
	}

	double newZoom = m_zoomFactor * factor;
	setZoomFactor(qBound(0.1, newZoom, 100.0));
}

void QtGraphicsView::refreshStyle()
{
	m_zoomInButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "graph_view/images/zoom_in.png",
		"search/button"
	));

	m_zoomOutButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "graph_view/images/zoom_out.png",
		"search/button"
	));
}

void QtGraphicsView::resizeEvent(QResizeEvent* event)
{
	m_zoomState->setGeometry(QRect(31, event->size().height() - 27, 65, 19));
	m_zoomInButton->setGeometry(QRect(8, event->size().height() - 50, 19, 19));
	m_zoomOutButton->setGeometry(QRect(8, event->size().height() - 27, 19, 19));

	m_zoomInButton->setIconSize(QSize(15, 15));
	m_zoomOutButton->setIconSize(QSize(15, 15));

	emit resized();
}

void QtGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()))
	{
		m_last = event->pos();
	}

	QGraphicsView::mousePressEvent(event);
}

void QtGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()) && event->pos() == m_last)
	{
		emit emptySpaceClicked();
	}

	QGraphicsView::mouseReleaseEvent(event);
	viewport()->setCursor(Qt::ArrowCursor);
}

void QtGraphicsView::keyPressEvent(QKeyEvent* event)
{
	if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z && event->text().size())
	{
		QChar c = event->text().at(0).toUpper();
		emit characterKeyPressed(c);
	}

	bool moved = moves();

	switch (event->key())
	{
		case Qt::Key_W:
			m_up = true;
			break;
		case Qt::Key_A:
			m_left = true;
			break;
		case Qt::Key_S:
			m_down = true;
			break;
		case Qt::Key_D:
			m_right = true;
			break;
		case Qt::Key_0:
			setZoomFactor(1.0f);
			updateTransform();
			break;
		case Qt::Key_Shift:
			m_shift = true;
			break;
		default:
			QGraphicsView::keyPressEvent(event);
			return;
	}

	if (!moved && moves())
	{
		m_timer->start(20);
	}

	m_timerStopper->start(1000);
}

void QtGraphicsView::keyReleaseEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_W:
			m_up = false;
			break;
		case Qt::Key_A:
			m_left = false;
			break;
		case Qt::Key_S:
			m_down = false;
			break;
		case Qt::Key_D:
			m_right = false;
			break;
		case Qt::Key_Shift:
			m_shift = false;
			break;
		default:
			return;
	}

	if (!moves())
	{
		m_timer->stop();
	}
}

void QtGraphicsView::wheelEvent(QWheelEvent* event)
{
	bool zoomDefault = ApplicationSettings::getInstance()->getControlsGraphZoomOnMouseWheel();
	bool shiftPressed = event->modifiers() == Qt::ShiftModifier;
	bool ctrlPressed = event->modifiers() == Qt::ControlModifier;

	if (zoomDefault != (shiftPressed | ctrlPressed))
	{
		if (event->delta() != 0.0f)
		{
			updateZoom(event->delta());
		}
	}
	else
	{
		QGraphicsView::wheelEvent(event);
	}
}

void QtGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
	m_clipboardNodeName = "";
	m_bookmarkNodeId = 0;
	FilePath clipboardFilePath;

	QtGraphNode* node = getNodeAtCursorPosition();
	while (node)
	{
		QtGraphNodeData* dataNode = dynamic_cast<QtGraphNodeData*>(node);
		if (dataNode)
		{
			m_clipboardNodeName = dataNode->getName();
			m_bookmarkNodeId = dataNode->getTokenId();
			clipboardFilePath = dataNode->getFilePath();
		}
		else if (dynamic_cast<QtGraphNodeBundle*>(node))
		{
			m_clipboardNodeName = node->getName();
			break;
		}
		node = node->getParent();
	}

	QtContextMenu menu(event, this);
	menu.addSeparator();
	menu.addAction(m_exportGraphAction);

	if (m_bookmarkNodeId)
	{
		menu.addSeparator();
		menu.addAction(m_bookmarkNodeAction);
	}

	if (!m_clipboardNodeName.empty() || !clipboardFilePath.empty())
	{
		menu.addSeparator();
	}

	if (!m_clipboardNodeName.empty())
	{
		menu.addAction(m_copyNodeNameAction);
	}

	if (!clipboardFilePath.empty())
	{
		menu.addFileActions(clipboardFilePath);
	}

	menu.show();
}

void QtGraphicsView::updateTimer()
{
	float ds = 30.0f;
	float dz = 50.0f;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (m_shift)
	{
		if (m_up)
		{
			z += dz;
		}
		else if (m_down)
		{
			z -= dz;
		}
	}
	else
	{
		if (m_up)
		{
			y -= ds;
		}
		else if (m_down)
		{
			y += ds;
		}

		if (m_left)
		{
			x -= ds;
		}
		else if (m_right)
		{
			x += ds;
		}
	}

	if (x != 0)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + x);
	}

	if (y != 0)
	{
		verticalScrollBar()->setValue(verticalScrollBar()->value() + y);
	}

	if (z != 0)
	{
		updateZoom(z);
	}
}

void QtGraphicsView::stopTimer()
{
	m_timer->stop();
}

QString ShowSaveFileDialog(QWidget *parent,
		const QString &title,
		const QString &directory,
		const QString &filter)
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)

	return QFileDialog::getSaveFileName(parent, title, directory, filter);

#else
	QFileDialog dialog(parent, title, directory, filter);

	if (parent)
	{
		dialog.setWindowModality(Qt::WindowModal);
	}

	QRegExp filter_regex(QLatin1String("(?:^\\*\\.(?!.*\\()|\\(\\*\\.)(\\w+)"));
	QStringList filters = filter.split(QLatin1String(";;"));

	if (!filters.isEmpty())
	{
		dialog.setNameFilters(filters);
	}

	dialog.setAcceptMode(QFileDialog::AcceptSave);

	if (dialog.exec() == QDialog::Accepted)
	{
		QString file_name = dialog.selectedFiles().first();
		QFileInfo info(file_name);

		if (info.suffix().isEmpty() && !dialog.selectedNameFilter().isEmpty())
		{
			if (filter_regex.indexIn(dialog.selectedNameFilter()) != -1)
			{
				QString extension = filter_regex.cap(1);
				file_name += QLatin1String(".") + extension;
			}
		}
		return file_name;
	}
	else
	{
		return QString();
	}
#endif  // Q_WS_MAC || Q_WS_WIN
}

void QtGraphicsView::exportGraph()
{
	QString fileName = ShowSaveFileDialog(
		this, "Save image", QDir::homePath(), "PNG (*.png);;JPEG (*.JPEG);;BMP Files (*.bmp)");

	if (!fileName.isNull())
	{
		QImage image(scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		scene()->render(&painter);
		image.save(fileName);

		// different approach
		// QPixmap pixMap = grab();
		// pixMap.save(fileName);
	}
}

void QtGraphicsView::copyNodeName()
{
	QApplication::clipboard()->setText(m_clipboardNodeName.c_str());
}

void QtGraphicsView::bookmarkNode()
{
	MessageDisplayBookmarkCreator(m_bookmarkNodeId).dispatch();
}

void QtGraphicsView::zoomInPressed()
{
	updateZoom(m_zoomInButtonSpeed);
}

void QtGraphicsView::zoomOutPressed()
{
	updateZoom(m_zoomOutButtonSpeed);
}

void QtGraphicsView::hideZoomLabel()
{
	m_zoomState->hide();
}

bool QtGraphicsView::moves() const
{
	return m_up || m_down || m_left || m_right;
}

void QtGraphicsView::setZoomFactor(float zoomFactor)
{
	m_zoomFactor = zoomFactor;

	m_zoomState->setText(QString::number(int(m_zoomFactor * 100)) + "%");
	updateTransform();

	m_zoomState->show();
	m_zoomLabelTimer->stop();
	m_zoomLabelTimer->start(1000);
}

void QtGraphicsView::updateTransform()
{
	float zoomFactor = m_appZoomFactor * m_zoomFactor;
	setTransform(QTransform(zoomFactor, 0, 0, zoomFactor, 0, 0));
}
