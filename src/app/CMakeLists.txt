add_files(
	APP_FILES

	platform_includes/includesDefault.h
	platform_includes/includesMac.h
	platform_includes/includesWindows.h

	qt/element/QtAutocompletionList.cpp
	qt/element/QtAutocompletionList.h
	qt/element/QtCodeFile.cpp
	qt/element/QtCodeFile.h
	qt/element/QtCodeFileList.cpp
	qt/element/QtCodeFileList.h
	qt/element/QtCodeSnippet.cpp
	qt/element/QtCodeSnippet.h
	qt/element/QtMainWindow.cpp
	qt/element/QtMainWindow.h
	qt/element/QtSearchBar.cpp
	qt/element/QtSearchBar.h
	qt/element/QtSmartSearchBox.cpp
	qt/element/QtSmartSearchBox.h
	qt/element/QtStatusBar.cpp
	qt/element/QtStatusBar.h

	qt/graphics/QtGraphicsRoundedRectItem.cpp
	qt/graphics/QtGraphicsRoundedRectItem.h

	qt/utility/QtDeviceScaledPixmap.h
	qt/utility/QtGraphPostprocessor.cpp
	qt/utility/QtGraphPostprocessor.h
	qt/utility/QtHighlighter.cpp
	qt/utility/QtHighlighter.h
	qt/utility/QtThreadedFunctor.h
	qt/utility/utilityQt.cpp
	qt/utility/utilityQt.h

	qt/view/graphElements/nodeComponents/QtGraphNodeComponent.cpp
	qt/view/graphElements/nodeComponents/QtGraphNodeComponent.h
	qt/view/graphElements/nodeComponents/QtGraphNodeComponentClickable.cpp
	qt/view/graphElements/nodeComponents/QtGraphNodeComponentClickable.h
	qt/view/graphElements/nodeComponents/QtGraphNodeComponentMoveable.cpp
	qt/view/graphElements/nodeComponents/QtGraphNodeComponentMoveable.h

	qt/view/graphElements/QtGraphEdge.cpp
	qt/view/graphElements/QtGraphEdge.h
	qt/view/graphElements/QtGraphNode.cpp
	qt/view/graphElements/QtGraphNode.h
	qt/view/graphElements/QtGraphNodeAccess.cpp
	qt/view/graphElements/QtGraphNodeAccess.h

	qt/view/QtCodeView.cpp
	qt/view/QtCodeView.h
	qt/view/QtGraphView.cpp
	qt/view/QtGraphView.h
	qt/view/QtMainView.cpp
	qt/view/QtMainView.h
	qt/view/QtSearchView.cpp
	qt/view/QtSearchView.h
	qt/view/QtStatusBarView.cpp
	qt/view/QtStatusBarView.h
	qt/view/QtViewFactory.cpp
	qt/view/QtViewFactory.h
	qt/view/QtViewWidgetWrapper.cpp
	qt/view/QtViewWidgetWrapper.h

	main.cpp
)
