#include "QtGraphicsView.h"

#include <QDir>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

#include <QApplication>
#include <QClipboard>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSvgGenerator>

#include "ApplicationSettings.h"
#include "GraphFocusHandler.h"
#include "MessageActivateLegend.h"
#include "MessageBookmarkCreate.h"
#include "MessageCodeShowDefinition.h"
#include "MessageFocusView.h"
#include "MessageGraphNodeExpand.h"
#include "MessageGraphNodeHide.h"
#include "MessageHistoryRedo.h"
#include "MessageHistoryUndo.h"
#include "MessageTabOpenWith.h"
#include "QtContextMenu.h"
#include "QtFileDialog.h"
#include "QtGraphEdge.h"
#include "QtGraphNode.h"
#include "QtGraphNodeBundle.h"
#include "QtGraphNodeData.h"
#include "QtGraphNodeExpandToggle.h"
#include "QtSelfRefreshIconButton.h"
#include "ResourcePaths.h"
#include "utilityApp.h"
#include "utilityQt.h"

QtGraphicsView::QtGraphicsView(GraphFocusHandler* focusHandler, QWidget* parent)
	: QGraphicsView(parent)
	, m_focusHandler(focusHandler)
	, m_zoomFactor(1.0f)
	, m_appZoomFactor(1.0f)
	, m_zoomInButtonSpeed(20.0f)
	, m_zoomOutButtonSpeed(-20.0f)
{
	QString modifierName = utility::getOsType() == OS_MAC ? QStringLiteral("Cmd")
														  : QStringLiteral("Ctrl");

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	m_timer = std::make_shared<QTimer>(this);
	connect(m_timer.get(), &QTimer::timeout, this, &QtGraphicsView::updateTimer);

	m_timerStopper = std::make_shared<QTimer>(this);
	m_timerStopper->setSingleShot(true);
	connect(m_timerStopper.get(), &QTimer::timeout, this, &QtGraphicsView::stopTimer);

	m_zoomLabelTimer = std::make_shared<QTimer>(this);
	connect(m_zoomLabelTimer.get(), &QTimer::timeout, this, &QtGraphicsView::hideZoomLabel);

	m_openInTabAction = new QAction(QStringLiteral("Open in New Tab (Ctrl + Shift + Left Click)"), this);
#if defined(Q_OS_MAC)
	m_openInTabAction->setText(QStringLiteral("Open in New Tab (Cmd + Shift + Left Click)"));
#endif
	m_openInTabAction->setStatusTip(QStringLiteral("Open this node in a new tab"));
	m_openInTabAction->setToolTip(QStringLiteral("Open this node in a new tab"));
	connect(m_openInTabAction, &QAction::triggered, this, &QtGraphicsView::openInTab);

	m_copyNodeNameAction = new QAction(QStringLiteral("Copy Name"), this);
	m_copyNodeNameAction->setStatusTip(
		QStringLiteral("Copies the name of this node to the clipboard"));
	m_copyNodeNameAction->setToolTip(
		QStringLiteral("Copies the name of this node to the clipboard"));
	connect(m_copyNodeNameAction, &QAction::triggered, this, &QtGraphicsView::copyNodeName);

	m_collapseAction = new QAction(QStringLiteral("Collapse Node (Shift + Left Click)"), this);
	m_collapseAction->setStatusTip(QStringLiteral("Hide the unconnected members of the node"));
	m_collapseAction->setToolTip(QStringLiteral("Hide the unconnected members of the node"));
	connect(m_collapseAction, &QAction::triggered, this, &QtGraphicsView::collapseNode);

	m_expandAction = new QAction(QStringLiteral("Expand Node (Shift + Left Click)"), this);
	m_expandAction->setStatusTip(QStringLiteral("Show unconnected members of the node"));
	m_expandAction->setToolTip(QStringLiteral("Show unconnected members of the node"));
	connect(m_expandAction, &QAction::triggered, this, &QtGraphicsView::expandNode);

	m_showInIDEAction = new QAction(
		QStringLiteral("Show Definition in IDE (Ctrl + Left Click)"), this);
#if defined(Q_OS_MAC)
	m_showInIDEAction->setText("Show Definition in IDE (Cmd + Left Click)");
#endif
	m_showInIDEAction->setStatusTip(
		QStringLiteral("Show definition of this symbol in the IDE (via plug-in)"));
	m_showInIDEAction->setToolTip(
		QStringLiteral("Show definition of this symbol in the IDE (via plug-in)"));
	connect(m_showInIDEAction, &QAction::triggered, this, &QtGraphicsView::showInIDE);

	m_showDefinitionAction = new QAction(
		QStringLiteral("Show Definition (Ctrl + Alt + Left Click)"), this);
#if defined(Q_OS_MAC)
	m_showDefinitionAction->setText("Show Definition (Cmd + Alt + Left Click)");
#endif
	m_showDefinitionAction->setStatusTip(
		QStringLiteral("Show definition of this symbol in the code"));
	m_showDefinitionAction->setToolTip(
		QStringLiteral("Show definition of this symbol in the code"));
	connect(m_showDefinitionAction, &QAction::triggered, this, &QtGraphicsView::showDefinition);

	m_hideNodeAction = new QAction(QStringLiteral("Hide Node (Alt + Left Click)"), this);
	m_hideNodeAction->setStatusTip(QStringLiteral("Hide the node from this graph"));
	m_hideNodeAction->setToolTip(QStringLiteral("Hide the node from this graph"));
	connect(m_hideNodeAction, &QAction::triggered, this, &QtGraphicsView::hideNode);

	m_hideEdgeAction = new QAction(QStringLiteral("Hide Edge (Alt + Left Click)"), this);
	m_hideEdgeAction->setStatusTip(QStringLiteral("Hide the edge from this graph"));
	m_hideEdgeAction->setToolTip(QStringLiteral("Hide the edge from this graph"));
	connect(m_hideEdgeAction, &QAction::triggered, this, &QtGraphicsView::hideEdge);

	m_bookmarkNodeAction = new QAction(QStringLiteral("Bookmark Node"), this);
	m_bookmarkNodeAction->setStatusTip(QStringLiteral("Create a bookmark for this node"));
	m_bookmarkNodeAction->setToolTip(QStringLiteral("Create a bookmark for this node"));
	connect(m_bookmarkNodeAction, &QAction::triggered, this, &QtGraphicsView::bookmarkNode);

	m_exportGraphAction = new QAction(QStringLiteral("Save as Image"), this);
	m_exportGraphAction->setStatusTip(QStringLiteral("Save this graph as image file"));
	m_exportGraphAction->setToolTip(QStringLiteral("Save this graph as image file"));
	connect(m_exportGraphAction, &QAction::triggered, this, &QtGraphicsView::exportGraph);

	m_focusIndicator = new QWidget(this);
	m_focusIndicator->setObjectName(QStringLiteral("focus_indicator"));
	m_focusIndicator->hide();

	m_zoomState = new QPushButton(this);
	m_zoomState->setObjectName(QStringLiteral("zoom_state"));
	m_zoomState->hide();

	m_zoomInButton = new QtSelfRefreshIconButton(
		QLatin1String(""),
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/zoom_in.png"),
		"search/button",
		this);
	m_zoomInButton->setObjectName(QStringLiteral("zoom_in_button"));
	m_zoomInButton->setAutoRepeat(true);
	m_zoomInButton->setToolTip("zoom in (" + modifierName + " + Mousewheel forward)");
	connect(m_zoomInButton, &QPushButton::pressed, this, &QtGraphicsView::zoomInPressed);

	m_zoomOutButton = new QtSelfRefreshIconButton(
		QLatin1String(""),
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/zoom_out.png"),
		"search/button",
		this);
	m_zoomOutButton->setObjectName(QStringLiteral("zoom_out_button"));
	m_zoomOutButton->setAutoRepeat(true);
	m_zoomOutButton->setToolTip("zoom out (" + modifierName + " + Mousewheel back)");
	connect(m_zoomOutButton, &QPushButton::pressed, this, &QtGraphicsView::zoomOutPressed);

	m_legendButton = new QtSelfRefreshIconButton(
		QLatin1String(""),
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/legend.png"),
		"search/button",
		this);
	m_legendButton->setObjectName(QStringLiteral("legend_button"));
	m_legendButton->setToolTip(QStringLiteral("show legend"));
	connect(m_legendButton, &QPushButton::clicked, this, &QtGraphicsView::legendClicked);
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

void QtGraphicsView::setSceneRect(const QRectF& rect)
{
	QGraphicsView::setSceneRect(rect);
	scene()->setSceneRect(rect);
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

QtGraphEdge* QtGraphicsView::getEdgeAtCursorPosition() const
{
	QtGraphEdge* edge = nullptr;

	QPointF point = mapToScene(mapFromGlobal(QCursor::pos()));
	QGraphicsItem* item = scene()->itemAt(point, QTransform());
	if (item)
	{
		edge = dynamic_cast<QtGraphEdge*>(item->parentItem());
	}

	return edge;
}

void QtGraphicsView::ensureVisibleAnimated(const QRectF& rect, int xmargin, int ymargin)
{
	int xval = horizontalScrollBar()->value();
	int yval = verticalScrollBar()->value();

	setInteractive(false);

	ensureVisible(rect, xmargin, ymargin);

	if (ApplicationSettings::getInstance()->getUseAnimations() && isVisible())
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

		connect(move, &QPropertyAnimation::finished, [this]() { setInteractive(true); });

		move->start();
	}
	else
	{
		setInteractive(true);
	}
}

void QtGraphicsView::updateZoom(float delta)
{
	float factor = 1.0f + 0.001f * delta;

	if (factor <= 0.0f)
	{
		factor = 0.000001f;
	}

	double newZoom = m_zoomFactor * factor;
	setZoomFactor(static_cast<float>(qBound(0.1, newZoom, 100.0)));
}

void QtGraphicsView::resizeEvent(QResizeEvent* event)
{
	m_focusIndicator->setGeometry(QRect(0, 0, event->size().width(), 3));
	m_zoomState->setGeometry(QRect(31, event->size().height() - 27, 65, 19));
	m_zoomInButton->setGeometry(QRect(8, event->size().height() - 50, 19, 19));
	m_zoomOutButton->setGeometry(QRect(8, event->size().height() - 27, 18, 19));
	m_legendButton->setGeometry(
		QRect(event->size().width() - 24, event->size().height() - 24, 18, 18));

	m_zoomInButton->setIconSize(QSize(15, 15));
	m_zoomOutButton->setIconSize(QSize(15, 15));
	m_legendButton->setIconSize(QSize(10, 10));

	emit resized();
}

void QtGraphicsView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()))
	{
		m_last = event->pos();
	}

	QGraphicsView::mousePressEvent(event);
}

void QtGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	QtGraphEdge::unfocusBezierEdge();
	QGraphicsView::mouseMoveEvent(event);
}

void QtGraphicsView::mouseReleaseEvent(QMouseEvent* event)
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
	bool moved = moves();
	bool shift = event->modifiers() & Qt::ShiftModifier;
	bool alt = event->modifiers() & Qt::AltModifier;
	bool ctrl = event->modifiers() & Qt::ControlModifier;

	switch (event->key())
	{
	case Qt::Key_Up:
		if (ctrl && !alt)
		{
			m_up = true;
			break;
		}
	case Qt::Key_K:
	case Qt::Key_W:
		if (!ctrl && !alt)
		{
			m_focusHandler->focusNext(GraphFocusHandler::Direction::UP, shift);
		}
		break;

	case Qt::Key_Down:
		if (ctrl && !alt)
		{
			m_down = true;
			break;
		}
	case Qt::Key_J:
	case Qt::Key_S:
		if (!alt && !ctrl)
		{
			m_focusHandler->focusNext(GraphFocusHandler::Direction::DOWN, shift);
		}
		break;

	case Qt::Key_Left:
		if (ctrl && !shift && !alt)
		{
			m_left = true;
			break;
		}
	case Qt::Key_H:
	case Qt::Key_A:
		if (!alt && !ctrl)
		{
			m_focusHandler->focusNext(GraphFocusHandler::Direction::LEFT, shift);
		}
		break;

	case Qt::Key_Right:
		if (ctrl && !shift && !alt)
		{
			m_right = true;
			break;
		}
	case Qt::Key_L:
	case Qt::Key_D:
		if (!alt && !ctrl)
		{
			m_focusHandler->focusNext(GraphFocusHandler::Direction::RIGHT, shift);
		}
		break;

	case Qt::Key_E:
	case Qt::Key_Return:
		if (ctrl && shift)
		{
			m_focusHandler->activateFocus(true);
		}
		else if (shift)
		{
			m_focusHandler->expandFocus();
		}
		else
		{
			m_focusHandler->activateFocus(false);
		}
		break;

	case Qt::Key_Y:
	case Qt::Key_Z:
		if (!alt && !ctrl)
		{
			if (shift)
			{
				MessageHistoryRedo().dispatch();
			}
			else
			{
				MessageHistoryUndo().dispatch();
			}
		}
		break;

	case Qt::Key_0:
		setZoomFactor(1.0f);
		updateTransform();
		break;
	case Qt::Key_Shift:
		m_shift = true;
		break;
	case Qt::Key_Control:
		m_ctrl = true;
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
	case Qt::Key_Up:
	case Qt::Key_W:
		m_up = false;
		break;
	case Qt::Key_Down:
	case Qt::Key_S:
		m_down = false;
		break;
	case Qt::Key_Left:
		m_left = false;
		break;
	case Qt::Key_Right:
		m_right = false;
		break;
	case Qt::Key_Shift:
		m_shift = false;
		break;
	case Qt::Key_Control:
		m_ctrl = false;
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
			updateZoom(static_cast<float>(event->delta()));
		}
	}
	else
	{
		QGraphicsView::wheelEvent(event);
	}
}

void QtGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
	m_openInTabNodeId = 0;
	m_clipboardNodeName = L"";
	m_collapseNodeId = 0;
	m_expandNodeId = 0;
	m_hideNodeId = 0;
	m_hideEdgeId = 0;
	m_bookmarkNodeId = 0;
	FilePath clipboardFilePath;

	QtGraphNode* node = getNodeAtCursorPosition();
	if (node)
	{
		while (node)
		{
			if (!m_hideNodeId)
			{
				m_hideNodeId = node->getTokenId();
			}

			if (!m_clipboardNodeName.size())
			{
				QtGraphNodeData* dataNode = dynamic_cast<QtGraphNodeData*>(node);
				if (dataNode)
				{
					m_clipboardNodeName = dataNode->getName();
					m_openInTabNodeId = dataNode->getTokenId();
					m_bookmarkNodeId = dataNode->getTokenId();
					clipboardFilePath = dataNode->getFilePath();
				}
				else if (dynamic_cast<QtGraphNodeBundle*>(node))
				{
					m_clipboardNodeName = node->getName();
				}
			}

			if (!m_collapseNodeId && !m_expandNodeId)
			{
				for (auto subNode: node->getSubNodes())
				{
					if (subNode->isExpandToggleNode())
					{
						if (dynamic_cast<QtGraphNodeExpandToggle*>(subNode)->isExpanded())
						{
							m_collapseNodeId = node->getTokenId();
						}
						else
						{
							m_expandNodeId = node->getTokenId();
						}
					}
				}
			}

			node = node->getParent();
		}
	}
	else
	{
		QtGraphEdge* edge = getEdgeAtCursorPosition();
		if (edge)
		{
			m_hideEdgeId = edge->getTokenId();
		}
	}

	m_openInTabAction->setEnabled(m_openInTabNodeId);
	m_collapseAction->setEnabled(m_collapseNodeId);
	m_expandAction->setEnabled(m_expandNodeId);
	m_showInIDEAction->setEnabled(m_hideNodeId);
	m_showDefinitionAction->setEnabled(m_hideNodeId);
	m_hideNodeAction->setEnabled(m_hideNodeId);
	m_hideEdgeAction->setEnabled(m_hideEdgeId);
	m_bookmarkNodeAction->setEnabled(m_bookmarkNodeId);

	m_copyNodeNameAction->setEnabled(!m_clipboardNodeName.empty());

	QtContextMenu menu(event, this);

	menu.addAction(m_openInTabAction);
	menu.addUndoActions();

	menu.addSeparator();

	menu.addAction(m_showDefinitionAction);
	menu.addAction(m_showInIDEAction);

	if (m_collapseNodeId)
	{
		menu.addAction(m_collapseAction);
	}
	else
	{
		menu.addAction(m_expandAction);
	}

	menu.addAction(m_hideNodeAction);
	menu.addAction(m_hideEdgeAction);
	menu.addAction(m_bookmarkNodeAction);

	menu.addSeparator();
	menu.addAction(m_exportGraphAction);

	menu.addSeparator();
	menu.addAction(m_copyNodeNameAction);
	menu.addFileActions(clipboardFilePath);

	menu.show();
}

void QtGraphicsView::focusInEvent(QFocusEvent* event)
{
	m_focusIndicator->show();
	emit focusIn();

	MessageFocusView(MessageFocusView::ViewType::GRAPH).dispatch();
}

void QtGraphicsView::focusOutEvent(QFocusEvent* event)
{
	m_focusIndicator->hide();
	emit focusOut();
}

void QtGraphicsView::updateTimer()
{
	const int ds = 30;
	const float dz = 50.0f;

	int x = 0;
	int y = 0;

	if (m_shift && m_ctrl)
	{
		if (m_up)
		{
			updateZoom(dz);
		}
		else if (m_down)
		{
			updateZoom(-dz);
		}
	}
	else if (m_ctrl)
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
}

void QtGraphicsView::stopTimer()
{
	m_timer->stop();
}

void QtGraphicsView::openInTab()
{
	MessageTabOpenWith(m_openInTabNodeId).dispatch();
}

void QtGraphicsView::exportGraph()
{
	const QString exportNotice = QStringLiteral("Exported from Sourcetrail");
	const int margin = 10;

	FilePath filePath(
		QtFileDialog::showSaveFileDialog(
			nullptr,
			QStringLiteral("Save image"),
			FilePath(),
			QStringLiteral("PNG (*.png);;JPEG (*.JPEG);;BMP Files (*.bmp);;SVG (*.svg)"))
			.toStdWString());


	if (filePath.extension() == L".svg")
	{
		QSvgGenerator svgGen;
		svgGen.setFileName(QString::fromStdWString(filePath.wstr()));
		svgGen.setSize(scene()->sceneRect().size().toSize());
		svgGen.setViewBox(QRect(QPoint(0, 0), scene()->sceneRect().size().toSize()));
		svgGen.setTitle(QString::fromStdWString(filePath.withoutExtension().fileName()));
		svgGen.setDescription(QStringLiteral("Graph exported from Sourcetrail") + QChar(0x00AE));

		QPainter painter(&svgGen);
		scene()->render(&painter);

		{
			QFont font(QStringLiteral("Fira Sans, sans-serif"));
			font.setPixelSize(8);
			painter.setFont(font);
		}
		{
			QRect boundingRect;
			painter.drawText(
				QRect(
					margin,
					margin,
					svgGen.size().width() - 2 * margin,
					svgGen.size().height() - 2 * margin),
				Qt::AlignBottom | Qt::AlignHCenter,
				exportNotice + ' ' + QChar(0x00AE),
				&boundingRect);
		}
	}
	else if (!filePath.empty())
	{
		QImage image(scene()->sceneRect().size().toSize() * 2, QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		scene()->render(&painter);

		{
			QFont font = painter.font();
			font.setPixelSize(14);
			painter.setFont(font);
		}
		{
			QRect boundingRect;
			painter.drawText(
				QRect(margin, margin, image.size().width() - 2 * margin, image.size().height() - 2 * margin),
				Qt::AlignBottom | Qt::AlignHCenter,
				exportNotice,
				&boundingRect);

			{
				QFont font = painter.font();
				font.setPixelSize(8);
				painter.setFont(font);
			}

			painter.drawText(
				boundingRect.right() + boundingRect.height() / 5,
				boundingRect.top() + boundingRect.height() / 2,
				QChar(0x00AE));
		}

		image.save(QString::fromStdWString(filePath.wstr()));
	}
}

void QtGraphicsView::copyNodeName()
{
	QApplication::clipboard()->setText(QString::fromStdWString(m_clipboardNodeName));
}

void QtGraphicsView::collapseNode()
{
	MessageGraphNodeExpand(m_collapseNodeId, false).dispatch();
}

void QtGraphicsView::expandNode()
{
	MessageGraphNodeExpand(m_expandNodeId, true).dispatch();
}

void QtGraphicsView::showInIDE()
{
	MessageCodeShowDefinition(m_hideNodeId, true).dispatch();
}

void QtGraphicsView::showDefinition()
{
	MessageCodeShowDefinition(m_hideNodeId).dispatch();
}

void QtGraphicsView::hideNode()
{
	MessageGraphNodeHide(m_hideNodeId).dispatch();
}

void QtGraphicsView::hideEdge()
{
	MessageGraphNodeHide(m_hideEdgeId).dispatch();
}

void QtGraphicsView::bookmarkNode()
{
	MessageBookmarkCreate(m_bookmarkNodeId).dispatch();
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

void QtGraphicsView::legendClicked()
{
	MessageActivateLegend().dispatch();
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
