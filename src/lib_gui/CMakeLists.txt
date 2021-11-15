add_files(
	LIB_GUI

	platform_includes/includesLinux.h
	platform_includes/includesMac.h
	platform_includes/includesWindows.h

	qt/element/bookmark/QtBookmark.cpp
	qt/element/bookmark/QtBookmark.h
	qt/element/bookmark/QtBookmarkCategory.cpp
	qt/element/bookmark/QtBookmarkCategory.h

	qt/element/button/QtHelpButton.cpp
	qt/element/button/QtHelpButton.h
	qt/element/button/QtHoverButton.cpp
	qt/element/button/QtHoverButton.h
	qt/element/button/QtIconButton.cpp
	qt/element/button/QtIconButton.h
	qt/element/button/QtIconStateButton.cpp
	qt/element/button/QtIconStateButton.h
	qt/element/button/QtSelfRefreshIconButton.cpp
	qt/element/button/QtSelfRefreshIconButton.h

	qt/element/code/CodeFocusHandler.cpp
	qt/element/code/CodeFocusHandler.h
	qt/element/code/QtCodeArea.cpp
	qt/element/code/QtCodeArea.h
	qt/element/code/QtCodeField.cpp
	qt/element/code/QtCodeField.h
	qt/element/code/QtCodeFile.cpp
	qt/element/code/QtCodeFile.h
	qt/element/code/QtCodeFileList.cpp
	qt/element/code/QtCodeFileList.h
	qt/element/code/QtCodeFileSingle.cpp
	qt/element/code/QtCodeFileSingle.h
	qt/element/code/QtCodeFileTitleBar.cpp
	qt/element/code/QtCodeFileTitleBar.h
	qt/element/code/QtCodeFileTitleButton.cpp
	qt/element/code/QtCodeFileTitleButton.h
	qt/element/code/QtCodeNavigable.cpp
	qt/element/code/QtCodeNavigable.h
	qt/element/code/QtCodeNavigator.cpp
	qt/element/code/QtCodeNavigator.h
	qt/element/code/QtCodeSnippet.cpp
	qt/element/code/QtCodeSnippet.h

	qt/element/history/QtHistoryList.cpp
	qt/element/history/QtHistoryList.h
	qt/element/history/QtUndoRedo.cpp
	qt/element/history/QtUndoRedo.h

	qt/element/search/QtAutocompletionList.cpp
	qt/element/search/QtAutocompletionList.h
	qt/element/search/QtScreenSearchBox.cpp
	qt/element/search/QtScreenSearchBox.h
	qt/element/search/QtSearchBar.cpp
	qt/element/search/QtSearchBar.h
	qt/element/search/QtSearchBarButton.cpp
	qt/element/search/QtSearchBarButton.h
	qt/element/search/QtSmartSearchBox.cpp
	qt/element/search/QtSmartSearchBox.h

	qt/element/dialog/QtLineEdit.cpp
	qt/element/dialog/QtLineEdit.h
	qt/element/dialog/QtListBox.cpp
	qt/element/dialog/QtListBox.h
	qt/element/dialog/QtListBoxItem.cpp
	qt/element/dialog/QtListBoxItem.h
	qt/element/dialog/QtLocationPicker.cpp
	qt/element/dialog/QtLocationPicker.h
	qt/element/dialog/QtPathListBox.cpp
	qt/element/dialog/QtPathListBox.h
	qt/element/dialog/QtPathListBoxItem.cpp
	qt/element/dialog/QtPathListBoxItem.h
	qt/element/dialog/QtProgressBar.cpp
	qt/element/dialog/QtProgressBar.h
	qt/element/dialog/QtStringListBox.cpp
	qt/element/dialog/QtStringListBox.h
	qt/element/dialog/QtStringListBoxItem.cpp
	qt/element/dialog/QtStringListBoxItem.h
	qt/element/dialog/QtTextEdit.cpp
	qt/element/dialog/QtTextEdit.h

	qt/element/QtTooltip.cpp
	qt/element/QtTooltip.h
	qt/element/QtStatusBar.cpp
	qt/element/QtStatusBar.h
	qt/element/QtTabBar.cpp
	qt/element/QtTabBar.h
	qt/element/QtTable.cpp
	qt/element/QtTable.h

	qt/graphics/base/QtCountCircleItem.cpp
	qt/graphics/base/QtCountCircleItem.h
	qt/graphics/base/QtLineItemAngled.cpp
	qt/graphics/base/QtLineItemAngled.h
	qt/graphics/base/QtLineItemBase.cpp
	qt/graphics/base/QtLineItemBase.h
	qt/graphics/base/QtLineItemBezier.cpp
	qt/graphics/base/QtLineItemBezier.h
	qt/graphics/base/QtLineItemStraight.cpp
	qt/graphics/base/QtLineItemStraight.h
	qt/graphics/base/QtRoundedRectItem.cpp
	qt/graphics/base/QtRoundedRectItem.h

	qt/graphics/component/QtGraphNodeComponent.cpp
	qt/graphics/component/QtGraphNodeComponent.h
	qt/graphics/component/QtGraphNodeComponentClickable.cpp
	qt/graphics/component/QtGraphNodeComponentClickable.h
	qt/graphics/component/QtGraphNodeComponentMoveable.cpp
	qt/graphics/component/QtGraphNodeComponentMoveable.h

	qt/graphics/graph/QtGraphEdge.cpp
	qt/graphics/graph/QtGraphEdge.h
	qt/graphics/graph/QtGraphNode.cpp
	qt/graphics/graph/QtGraphNode.h
	qt/graphics/graph/QtGraphNodeAccess.cpp
	qt/graphics/graph/QtGraphNodeAccess.h
	qt/graphics/graph/QtGraphNodeBundle.cpp
	qt/graphics/graph/QtGraphNodeBundle.h
	qt/graphics/graph/QtGraphNodeData.cpp
	qt/graphics/graph/QtGraphNodeData.h
	qt/graphics/graph/QtGraphNodeExpandToggle.cpp
	qt/graphics/graph/QtGraphNodeExpandToggle.h
	qt/graphics/graph/QtGraphNodeGroup.cpp
	qt/graphics/graph/QtGraphNodeGroup.h
	qt/graphics/graph/QtGraphNodeQualifier.cpp
	qt/graphics/graph/QtGraphNodeQualifier.h
	qt/graphics/graph/QtGraphNodeText.cpp
	qt/graphics/graph/QtGraphNodeText.h

	qt/graphics/GraphFocusHandler.cpp
	qt/graphics/GraphFocusHandler.h
	qt/graphics/QtGraphicsView.cpp
	qt/graphics/QtGraphicsView.h

	qt/network/QtIDECommunicationController.cpp
	qt/network/QtIDECommunicationController.h
	qt/network/QtNetworkFactory.cpp
	qt/network/QtNetworkFactory.h
	qt/network/QtRequest.cpp
	qt/network/QtRequest.h
	qt/network/QtTcpWrapper.cpp
	qt/network/QtTcpWrapper.h

	qt/project_wizard/content/path/QtProjectWizardContentPath.cpp
	qt/project_wizard/content/path/QtProjectWizardContentPath.h

	qt/project_wizard/content/paths/QtProjectWizardContentPaths.cpp
	qt/project_wizard/content/paths/QtProjectWizardContentPaths.h
	qt/project_wizard/content/paths/QtProjectWizardContentPathsExclude.cpp
	qt/project_wizard/content/paths/QtProjectWizardContentPathsExclude.h
	qt/project_wizard/content/paths/QtProjectWizardContentPathsSource.cpp
	qt/project_wizard/content/paths/QtProjectWizardContentPathsSource.h

	qt/project_wizard/content/QtProjectWizardContent.cpp
	qt/project_wizard/content/QtProjectWizardContent.h
	qt/project_wizard/content/QtProjectWizardContentCustomCommand.cpp
	qt/project_wizard/content/QtProjectWizardContentCustomCommand.h
	qt/project_wizard/content/QtProjectWizardContentExtensions.cpp
	qt/project_wizard/content/QtProjectWizardContentExtensions.h
	qt/project_wizard/content/QtProjectWizardContentGroup.cpp
	qt/project_wizard/content/QtProjectWizardContentGroup.h
	qt/project_wizard/content/QtProjectWizardContentPreferences.cpp
	qt/project_wizard/content/QtProjectWizardContentPreferences.h
	qt/project_wizard/content/QtProjectWizardContentProjectData.cpp
	qt/project_wizard/content/QtProjectWizardContentProjectData.h
	qt/project_wizard/content/QtProjectWizardContentRequiredLabel.h
	qt/project_wizard/content/QtProjectWizardContentSelect.cpp
	qt/project_wizard/content/QtProjectWizardContentSelect.h
	qt/project_wizard/content/QtProjectWizardContentSourceGroupData.cpp
	qt/project_wizard/content/QtProjectWizardContentSourceGroupData.h
	qt/project_wizard/content/QtProjectWizardContentSourceGroupInfoText.cpp
	qt/project_wizard/content/QtProjectWizardContentSourceGroupInfoText.h
	qt/project_wizard/content/QtProjectWizardContentUnloadable.cpp
	qt/project_wizard/content/QtProjectWizardContentUnloadable.h

	qt/project_wizard/QtProjectWizard.cpp
	qt/project_wizard/QtProjectWizard.h
	qt/project_wizard/QtProjectWizardWindow.cpp
	qt/project_wizard/QtProjectWizardWindow.h

	qt/utility/compatibilityQt.cpp
	qt/utility/compatibilityQt.h
	qt/utility/QtContextMenu.cpp
	qt/utility/QtContextMenu.h
	qt/utility/QtDeviceScaledPixmap.cpp
	qt/utility/QtDeviceScaledPixmap.h
	qt/utility/QtFileDialog.cpp
	qt/utility/QtFileDialog.h
	qt/utility/QtFilesAndDirectoriesDialog.cpp
	qt/utility/QtFilesAndDirectoriesDialog.h
	qt/utility/QtFlowLayout.cpp
	qt/utility/QtFlowLayout.h
	qt/utility/QtHelpButtonInfo.cpp
	qt/utility/QtHelpButtonInfo.h
	qt/utility/QtHighlighter.cpp
	qt/utility/QtHighlighter.h
	qt/utility/QtScrollSpeedChangeListener.cpp
	qt/utility/QtScrollSpeedChangeListener.h
	qt/utility/QtThreadedFunctor.h
	qt/utility/QtWindowsTaskbarButton.cpp
	qt/utility/QtWindowsTaskbarButton.h
	qt/utility/utilityQt.cpp
	qt/utility/utilityQt.h

	qt/view/QtBookmarkButtonsView.cpp
	qt/view/QtBookmarkButtonsView.h
	qt/view/QtBookmarkView.cpp
	qt/view/QtBookmarkView.h
	qt/view/QtCodeView.cpp
	qt/view/QtCodeView.h
	qt/view/QtCompositeView.cpp
	qt/view/QtCompositeView.h
	qt/view/QtCustomTrailView.cpp
	qt/view/QtCustomTrailView.h
	qt/view/QtDialogView.cpp
	qt/view/QtDialogView.h
	qt/view/QtErrorView.cpp
	qt/view/QtErrorView.h
	qt/view/QtGraphView.cpp
	qt/view/QtGraphView.h
	qt/view/QtGraphViewStyleImpl.cpp
	qt/view/QtGraphViewStyleImpl.h
	qt/view/QtMainView.cpp
	qt/view/QtMainView.h
	qt/view/QtRefreshView.cpp
	qt/view/QtRefreshView.h
	qt/view/QtScreenSearchView.cpp
	qt/view/QtScreenSearchView.h
	qt/view/QtSearchView.cpp
	qt/view/QtSearchView.h
	qt/view/QtStatusBarView.cpp
	qt/view/QtStatusBarView.h
	qt/view/QtStatusView.cpp
	qt/view/QtStatusView.h
	qt/view/QtTabbedView.cpp
	qt/view/QtTabbedView.h
	qt/view/QtTooltipView.cpp
	qt/view/QtTooltipView.h
	qt/view/QtTabsView.cpp
	qt/view/QtTabsView.h
	qt/view/QtUndoRedoView.cpp
	qt/view/QtUndoRedoView.h
	qt/view/QtViewFactory.cpp
	qt/view/QtViewFactory.h
	qt/view/QtViewWidgetWrapper.cpp
	qt/view/QtViewWidgetWrapper.h

	qt/window/QtAbout.cpp
	qt/window/QtAbout.h
	qt/window/QtBookmarkBrowser.cpp
	qt/window/QtBookmarkBrowser.h
	qt/window/QtBookmarkCreator.cpp
	qt/window/QtBookmarkCreator.h
	qt/window/QtIndexingDialog.cpp
	qt/window/QtIndexingDialog.h
	qt/window/QtIndexingProgressDialog.cpp
	qt/window/QtIndexingProgressDialog.h
	qt/window/QtIndexingReportDialog.cpp
	qt/window/QtIndexingReportDialog.h
	qt/window/QtIndexingStartDialog.cpp
	qt/window/QtIndexingStartDialog.h
	qt/window/QtKeyboardShortcuts.cpp
	qt/window/QtKeyboardShortcuts.h
	qt/window/QtKnownProgressDialog.cpp
	qt/window/QtKnownProgressDialog.h
	qt/window/QtLicenseWindow.cpp
	qt/window/QtLicenseWindow.h
	qt/window/QtMainWindow.cpp
	qt/window/QtMainWindow.h
	qt/window/QtPathListDialog.cpp
	qt/window/QtPathListDialog.h
	qt/window/QtPreferencesWindow.cpp
	qt/window/QtPreferencesWindow.h
	qt/window/QtProgressBarDialog.cpp
	qt/window/QtProgressBarDialog.h
	qt/window/QtSelectPathsDialog.cpp
	qt/window/QtSelectPathsDialog.h
	qt/window/QtStartScreen.cpp
	qt/window/QtStartScreen.h
	qt/window/QtTextEditDialog.cpp
	qt/window/QtTextEditDialog.h
	qt/window/QtUnknownProgressDialog.cpp
	qt/window/QtUnknownProgressDialog.h
	qt/window/QtWindow.cpp
	qt/window/QtWindow.h
	qt/window/QtWindowBase.cpp
	qt/window/QtWindowBase.h
	qt/window/QtWindowStack.cpp
	qt/window/QtWindowStack.h

	qt/QtApplication.cpp
	qt/QtApplication.h
	qt/QtCoreApplication.cpp
	qt/QtCoreApplication.h

	utility/path_detector/CombinedPathDetector.cpp
	utility/path_detector/CombinedPathDetector.h
	utility/path_detector/PathDetector.cpp
	utility/path_detector/PathDetector.h

	utility/utilityApp.cpp
	utility/utilityApp.h
	utility/utilityPathDetection.cpp
	utility/utilityPathDetection.h
)

if (BUILD_CXX_LANGUAGE_PACKAGE)
	add_files(
		LIB_GUI

		qt/project_wizard/content/path/QtProjectWizardContentPathCDB.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathCDB.h
		qt/project_wizard/content/path/QtProjectWizardContentPathCodeblocksProject.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathCodeblocksProject.h
		qt/project_wizard/content/path/QtProjectWizardContentPathCxxPch.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathCxxPch.h

		qt/project_wizard/content/paths/QtProjectWizardContentPathsFrameworkSearch.cpp
		qt/project_wizard/content/paths/QtProjectWizardContentPathsFrameworkSearch.h
		qt/project_wizard/content/paths/QtProjectWizardContentPathsFrameworkSearchGlobal.cpp
		qt/project_wizard/content/paths/QtProjectWizardContentPathsFrameworkSearchGlobal.h
		qt/project_wizard/content/paths/QtProjectWizardContentPathsHeaderSearch.cpp
		qt/project_wizard/content/paths/QtProjectWizardContentPathsHeaderSearch.h
		qt/project_wizard/content/paths/QtProjectWizardContentPathsHeaderSearchGlobal.cpp
		qt/project_wizard/content/paths/QtProjectWizardContentPathsHeaderSearchGlobal.h
		qt/project_wizard/content/paths/QtProjectWizardContentPathsIndexedHeaders.cpp
		qt/project_wizard/content/paths/QtProjectWizardContentPathsIndexedHeaders.h

		qt/project_wizard/content/QtProjectWizardContentCppStandard.cpp
		qt/project_wizard/content/QtProjectWizardContentCppStandard.h
		qt/project_wizard/content/QtProjectWizardContentCrossCompilationOptions.cpp
		qt/project_wizard/content/QtProjectWizardContentCrossCompilationOptions.h
		qt/project_wizard/content/QtProjectWizardContentCStandard.cpp
		qt/project_wizard/content/QtProjectWizardContentCStandard.h
		qt/project_wizard/content/QtProjectWizardContentCxxPchFlags.cpp
		qt/project_wizard/content/QtProjectWizardContentCxxPchFlags.h
		qt/project_wizard/content/QtProjectWizardContentFlags.cpp
		qt/project_wizard/content/QtProjectWizardContentFlags.h
		qt/project_wizard/content/QtProjectWizardContentVS.cpp
		qt/project_wizard/content/QtProjectWizardContentVS.h

		utility/path_detector/cxx_header/CxxFrameworkPathDetector.cpp
		utility/path_detector/cxx_header/CxxFrameworkPathDetector.h
		utility/path_detector/cxx_header/CxxHeaderPathDetector.cpp
		utility/path_detector/cxx_header/CxxHeaderPathDetector.h
		utility/path_detector/cxx_header/CxxVs10To14HeaderPathDetector.cpp
		utility/path_detector/cxx_header/CxxVs10To14HeaderPathDetector.h
		utility/path_detector/cxx_header/CxxVs15HeaderPathDetector.cpp
		utility/path_detector/cxx_header/CxxVs15HeaderPathDetector.h
		utility/path_detector/cxx_header/utilityCxxHeaderDetection.cpp
		utility/path_detector/cxx_header/utilityCxxHeaderDetection.h
	)
endif()

if (BUILD_JAVA_LANGUAGE_PACKAGE)
	add_files(
		LIB_GUI

		qt/project_wizard/content/path/QtProjectWizardContentPathSettingsMaven.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathSettingsMaven.h
		qt/project_wizard/content/path/QtProjectWizardContentPathSourceGradle.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathSourceGradle.h
		qt/project_wizard/content/path/QtProjectWizardContentPathSourceMaven.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathSourceMaven.h

		qt/project_wizard/content/paths/QtProjectWizardContentPathsClassJava.cpp
		qt/project_wizard/content/paths/QtProjectWizardContentPathsClassJava.h

		qt/project_wizard/content/QtProjectWizardContentJavaStandard.cpp
		qt/project_wizard/content/QtProjectWizardContentJavaStandard.h

		utility/path_detector/java_runtime/JavaPathDetector.cpp
		utility/path_detector/java_runtime/JavaPathDetector.h
		utility/path_detector/java_runtime/JavaPathDetectorLinux.cpp
		utility/path_detector/java_runtime/JavaPathDetectorLinux.h
		utility/path_detector/java_runtime/JavaPathDetectorMac.cpp
		utility/path_detector/java_runtime/JavaPathDetectorMac.h
		utility/path_detector/java_runtime/JavaPathDetectorWindows.cpp
		utility/path_detector/java_runtime/JavaPathDetectorWindows.h

		utility/path_detector/jre_system_library/JreSystemLibraryPathDetector.cpp
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetector.h
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorLinux.cpp
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorLinux.h
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorMac.cpp
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorMac.h
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorWindows.cpp
		utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorWindows.h

		utility/path_detector/maven_executable/MavenPathDetectorUnix.cpp
		utility/path_detector/maven_executable/MavenPathDetectorUnix.h
		utility/path_detector/maven_executable/MavenPathDetectorWindows.cpp
		utility/path_detector/maven_executable/MavenPathDetectorWindows.h
	)
endif()

if (BUILD_PYTHON_LANGUAGE_PACKAGE)
	add_files(
		LIB_GUI

		qt/project_wizard/content/path/QtProjectWizardContentPathPythonEnvironment.cpp
		qt/project_wizard/content/path/QtProjectWizardContentPathPythonEnvironment.h
	)
endif()
