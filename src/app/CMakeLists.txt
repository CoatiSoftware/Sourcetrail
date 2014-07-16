add_files(
	APP_FILES

	platform_includes/includesDefault.h
	platform_includes/includesMac.h
	platform_includes/includesWindows.h

	qt/element/QtCodeFile.cpp
	qt/element/QtCodeFile.h
	qt/element/QtButton.cpp
	qt/element/QtButton.h
	qt/element/QtCodeSnippet.cpp
	qt/element/QtCodeSnippet.h
	qt/element/QtEditBox.cpp
	qt/element/QtEditBox.h
	qt/element/QtMainWindow.cpp
	qt/element/QtMainWindow.h

	qt/utility/QtHighLighter.cpp
	qt/utility/QtHighLighter.h
	qt/utility/QtThreadedFunctor.h
	qt/utility/utilityQt.cpp
	qt/utility/utilityQt.h

	qt/view/graphElements/QtGraphEdge.cpp
	qt/view/graphElements/QtGraphEdge.h
	qt/view/graphElements/QtGraphNode.cpp
	qt/view/graphElements/QtGraphNode.h
	qt/view/graphElements/QtGraphNodeMouseMovable.cpp
	qt/view/graphElements/QtGraphNodeMouseMovable.h

	qt/view/QtCodeView.cpp
	qt/view/QtCodeView.h
	qt/view/QtGraphView.cpp
	qt/view/QtGraphView.h
	qt/view/QtMainView.cpp
	qt/view/QtMainView.h
	qt/view/QtSearchView.cpp
	qt/view/QtSearchView.h

	qt/QtGuiFactory.cpp
	qt/QtGuiFactory.h
	qt/QtWidgetWrapper.cpp
	qt/QtWidgetWrapper.h

	main.cpp
)
