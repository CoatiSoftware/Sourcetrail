
add_files(
	LIB

	app/paths/AppPath.cpp
	app/paths/AppPath.h
	app/paths/ResourcePaths.cpp
	app/paths/ResourcePaths.h
	app/paths/UserPaths.cpp
	app/paths/UserPaths.h

	app/Application.cpp
	app/Application.h
	app/LanguagePackage.h
	app/LanguagePackageManager.cpp
	app/LanguagePackageManager.h

	component/controller/helper/ActivationListener.cpp
	component/controller/helper/ActivationListener.h
	component/controller/helper/BucketLayouter.cpp
	component/controller/helper/BucketLayouter.h
	component/controller/helper/DummyEdge.h
	component/controller/helper/DummyNode.h
	component/controller/helper/ListLayouter.cpp
	component/controller/helper/ListLayouter.h
	component/controller/helper/NetworkProtocolHelper.cpp
	component/controller/helper/NetworkProtocolHelper.h
	component/controller/helper/ScreenSearchInterfaces.h
	component/controller/helper/SnippetMerger.cpp
	component/controller/helper/SnippetMerger.h
	component/controller/helper/TrailLayouter.cpp
	component/controller/helper/TrailLayouter.h

	component/controller/ActivationController.cpp
	component/controller/ActivationController.h
	component/controller/BookmarkController.cpp
	component/controller/BookmarkController.h
	component/controller/CodeController.cpp
	component/controller/CodeController.h
	component/controller/Controller.cpp
	component/controller/Controller.h
	component/controller/CustomTrailController.cpp
	component/controller/CustomTrailController.h
	component/controller/ErrorController.cpp
	component/controller/ErrorController.h
	component/controller/GraphController.cpp
	component/controller/GraphController.h
	component/controller/IDECommunicationController.cpp
	component/controller/IDECommunicationController.h
	component/controller/RefreshController.cpp
	component/controller/RefreshController.h
	component/controller/ScreenSearchController.cpp
	component/controller/ScreenSearchController.h
	component/controller/SearchController.cpp
	component/controller/SearchController.h
	component/controller/StatusBarController.cpp
	component/controller/StatusBarController.h
	component/controller/StatusController.cpp
	component/controller/StatusController.h
	component/controller/TabsController.cpp
	component/controller/TabsController.h
	component/controller/TooltipController.cpp
	component/controller/TooltipController.h
	component/controller/UndoRedoController.cpp
	component/controller/UndoRedoController.h

	component/view/helper/CodeScrollParams.h
	component/view/helper/CodeSnippetParams.cpp
	component/view/helper/CodeSnippetParams.h
	component/view/BookmarkButtonsView.cpp
	component/view/BookmarkButtonsView.h
	component/view/BookmarkView.cpp
	component/view/BookmarkView.h
	component/view/CodeView.cpp
	component/view/CodeView.h
	component/view/CompositeView.cpp
	component/view/CompositeView.h
	component/view/CustomTrailView.h
	component/view/DialogView.cpp
	component/view/DialogView.h
	component/view/ErrorView.cpp
	component/view/ErrorView.h
	component/view/GraphView.cpp
	component/view/GraphView.h
	component/view/GraphViewStyle.cpp
	component/view/GraphViewStyle.h
	component/view/GraphViewStyleImpl.h
	component/view/MainView.cpp
	component/view/MainView.h
	component/view/RefreshView.cpp
	component/view/RefreshView.h
	component/view/ScreenSearchView.cpp
	component/view/ScreenSearchView.h
	component/view/SearchView.cpp
	component/view/SearchView.h
	component/view/StatusBarView.cpp
	component/view/StatusBarView.h
	component/view/StatusView.cpp
	component/view/StatusView.h
	component/view/TabbedView.cpp
	component/view/TabbedView.h
	component/view/TabsView.cpp
	component/view/TabsView.h
	component/view/TooltipView.cpp
	component/view/TooltipView.h
	component/view/UndoRedoView.cpp
	component/view/UndoRedoView.h
	component/view/View.cpp
	component/view/View.h
	component/view/ViewFactory.h
	component/view/ViewLayout.cpp
	component/view/ViewLayout.h
	component/view/ViewWidgetWrapper.cpp
	component/view/ViewWidgetWrapper.h

	component/Component.cpp
	component/Component.h
	component/ComponentFactory.cpp
	component/ComponentFactory.h
	component/ComponentManager.cpp
	component/ComponentManager.h
	component/NetworkFactory.cpp
	component/NetworkFactory.h
	component/Tab.cpp
	component/Tab.h
	component/TabId.cpp
	component/TabId.h

	data/bookmark/Bookmark.cpp
	data/bookmark/Bookmark.h
	data/bookmark/BookmarkCategory.cpp
	data/bookmark/BookmarkCategory.h
	data/bookmark/EdgeBookmark.cpp
	data/bookmark/EdgeBookmark.h
	data/bookmark/NodeBookmark.cpp
	data/bookmark/NodeBookmark.h

	data/fulltextsearch/FullTextSearchIndex.cpp
	data/fulltextsearch/FullTextSearchIndex.h
	data/fulltextsearch/SuffixArray.cpp
	data/fulltextsearch/SuffixArray.h

	data/graph/token_component/TokenComponent.cpp
	data/graph/token_component/TokenComponent.h
	data/graph/token_component/TokenComponentAbstraction.cpp
	data/graph/token_component/TokenComponentAbstraction.h
	data/graph/token_component/TokenComponentAccess.cpp
	data/graph/token_component/TokenComponentAccess.h
	data/graph/token_component/TokenComponentBundledEdges.cpp
	data/graph/token_component/TokenComponentBundledEdges.h
	data/graph/token_component/TokenComponentConst.cpp
	data/graph/token_component/TokenComponentConst.h
	data/graph/token_component/TokenComponentFilePath.cpp
	data/graph/token_component/TokenComponentFilePath.h
	data/graph/token_component/TokenComponentInheritanceChain.h
	data/graph/token_component/TokenComponentIsAmbiguous.h
	data/graph/token_component/TokenComponentStatic.cpp
	data/graph/token_component/TokenComponentStatic.h

	data/graph/Edge.cpp
	data/graph/Edge.h
	data/graph/ElementComponentKind.cpp
	data/graph/ElementComponentKind.h
	data/graph/Graph.cpp
	data/graph/Graph.h
	data/graph/Node.cpp
	data/graph/Node.h
	data/graph/Token.cpp
	data/graph/Token.h

	data/indexer/interprocess/shared_types/SharedIndexerCommand.cpp
	data/indexer/interprocess/shared_types/SharedIndexerCommand.h
	data/indexer/interprocess/shared_types/SharedIntermediateStorage.cpp
	data/indexer/interprocess/shared_types/SharedIntermediateStorage.h
	data/indexer/interprocess/shared_types/SharedStorageTypes.h

	data/indexer/interprocess/BaseInterprocessDataManager.cpp
	data/indexer/interprocess/BaseInterprocessDataManager.h
	data/indexer/interprocess/InterprocessIndexer.cpp
	data/indexer/interprocess/InterprocessIndexer.h
	data/indexer/interprocess/InterprocessIndexerCommandManager.cpp
	data/indexer/interprocess/InterprocessIndexerCommandManager.h
	data/indexer/interprocess/InterprocessIndexingStatusManager.cpp
	data/indexer/interprocess/InterprocessIndexingStatusManager.h
	data/indexer/interprocess/InterprocessIntermediateStorageManager.cpp
	data/indexer/interprocess/InterprocessIntermediateStorageManager.h

	data/indexer/CombinedIndexerCommandProvider.cpp
	data/indexer/CombinedIndexerCommandProvider.h
	data/indexer/Indexer.h
	data/indexer/IndexerBase.cpp
	data/indexer/IndexerBase.h
	data/indexer/IndexerCommand.cpp
	data/indexer/IndexerCommand.h
	data/indexer/IndexerCommandCustom.cpp
	data/indexer/IndexerCommandCustom.h
	data/indexer/IndexerCommandProvider.cpp
	data/indexer/IndexerCommandProvider.h
	data/indexer/IndexerCommandType.cpp
	data/indexer/IndexerCommandType.h
	data/indexer/IndexerComposite.cpp
	data/indexer/IndexerComposite.h
	data/indexer/IndexerStateInfo.h
	data/indexer/MemoryIndexerCommandProvider.cpp
	data/indexer/MemoryIndexerCommandProvider.h
	data/indexer/TaskBuildIndex.cpp
	data/indexer/TaskBuildIndex.h
	data/indexer/TaskExecuteCustomCommands.cpp
	data/indexer/TaskExecuteCustomCommands.h
	data/indexer/TaskFillIndexerCommandQueue.cpp
	data/indexer/TaskFillIndexerCommandQueue.h

	data/location/LocationType.cpp
	data/location/LocationType.h
	data/location/SourceLocation.cpp
	data/location/SourceLocation.h
	data/location/SourceLocationCollection.cpp
	data/location/SourceLocationCollection.h
	data/location/SourceLocationFile.cpp
	data/location/SourceLocationFile.h

	data/name/NameDelimiterType.cpp
	data/name/NameDelimiterType.h
	data/name/NameElement.cpp
	data/name/NameElement.h
	data/name/NameHierarchy.cpp
	data/name/NameHierarchy.h

	data/parser/AccessKind.cpp
	data/parser/AccessKind.h
	data/parser/ParseLocation.cpp
	data/parser/ParseLocation.h
	data/parser/Parser.cpp
	data/parser/Parser.h
	data/parser/ParserClient.h
	data/parser/ParserClientImpl.cpp
	data/parser/ParserClientImpl.h
	data/parser/ReferenceKind.cpp
	data/parser/ReferenceKind.h
	data/parser/SymbolKind.cpp
	data/parser/SymbolKind.h
	data/parser/TaskParseWrapper.cpp
	data/parser/TaskParseWrapper.h

	data/search/SearchIndex.cpp
	data/search/SearchIndex.h
	data/search/SearchMatch.cpp
	data/search/SearchMatch.h

	data/storage/migration/SqliteStorageMigration.cpp
	data/storage/migration/SqliteStorageMigration.h
	data/storage/migration/SqliteStorageMigrationLambda.cpp
	data/storage/migration/SqliteStorageMigrationLambda.h
	data/storage/migration/SqliteStorageMigrator.h

	data/storage/sqlite/SqliteBookmarkStorage.cpp
	data/storage/sqlite/SqliteBookmarkStorage.h
	data/storage/sqlite/SqliteDatabaseIndex.cpp
	data/storage/sqlite/SqliteDatabaseIndex.h
	data/storage/sqlite/SqliteIndexStorage.cpp
	data/storage/sqlite/SqliteIndexStorage.h
	data/storage/sqlite/SqliteStorage.cpp
	data/storage/sqlite/SqliteStorage.h

	data/storage/type/StorageBookmarkCategory.h
	data/storage/type/StorageBookmark.h
	data/storage/type/StorageBookmarkedEdge.h
	data/storage/type/StorageBookmarkedNode.h
	data/storage/type/StorageComponentAccess.h
	data/storage/type/StorageEdge.h
	data/storage/type/StorageElementComponent.h
	data/storage/type/StorageError.h
	data/storage/type/StorageFile.h
	data/storage/type/StorageLocalSymbol.h
	data/storage/type/StorageNode.h
	data/storage/type/StorageOccurrence.h
	data/storage/type/StorageSourceLocation.h
	data/storage/type/StorageSymbol.h

	data/storage/IntermediateStorage.cpp
	data/storage/IntermediateStorage.h
	data/storage/PersistentStorage.cpp
	data/storage/PersistentStorage.h
	data/storage/Storage.cpp
	data/storage/Storage.h
	data/storage/StorageAccess.h
	data/storage/StorageAccessProxy.cpp
	data/storage/StorageAccessProxy.h
	data/storage/StorageCache.cpp
	data/storage/StorageCache.h
	data/storage/StorageProvider.cpp
	data/storage/StorageProvider.h
	data/storage/StorageStats.h

	data/tooltip/TooltipInfo.h
	data/tooltip/TooltipOrigin.h

	data/DefinitionKind.cpp
	data/DefinitionKind.h
	data/ErrorCountInfo.h
	data/ErrorFilter.h
	data/ErrorInfo.h
	data/GroupType.cpp
	data/GroupType.h
	data/HierarchyCache.cpp
	data/HierarchyCache.h
	data/NodeKind.cpp
	data/NodeKind.h
	data/NodeType.cpp
	data/NodeType.h
	data/NodeTypeSet.cpp
	data/NodeTypeSet.h
	data/TaskCleanStorage.cpp
	data/TaskCleanStorage.h
	data/TaskFinishParsing.cpp
	data/TaskFinishParsing.h
	data/TaskInjectStorage.cpp
	data/TaskInjectStorage.h
	data/TaskMergeStorages.cpp
	data/TaskMergeStorages.h

	project/Project.cpp
	project/Project.h
	project/RefreshInfo.h
	project/RefreshInfoGenerator.h
	project/RefreshInfoGenerator.cpp
	project/SourceGroup.cpp
	project/SourceGroup.h
	project/SourceGroupCustomCommand.cpp
	project/SourceGroupCustomCommand.h
	project/SourceGroupFactory.cpp
	project/SourceGroupFactory.h
	project/SourceGroupFactoryModule.cpp
	project/SourceGroupFactoryModule.h
	project/SourceGroupFactoryModuleCustom.cpp
	project/SourceGroupFactoryModuleCustom.h

	settings/migration/SettingsMigration.cpp
	settings/migration/SettingsMigration.h
	settings/migration/SettingsMigrationDeleteKey.cpp
	settings/migration/SettingsMigrationDeleteKey.h
	settings/migration/SettingsMigrationLambda.cpp
	settings/migration/SettingsMigrationLambda.h
	settings/migration/SettingsMigrationMoveKey.cpp
	settings/migration/SettingsMigrationMoveKey.h
	settings/migration/SettingsMigrator.h

	settings/source_group/component/SourceGroupSettingsWithCustomCommand.cpp
	settings/source_group/component/SourceGroupSettingsWithCustomCommand.h
	settings/source_group/component/SourceGroupSettingsWithExcludeFilters.cpp
	settings/source_group/component/SourceGroupSettingsWithExcludeFilters.h
	settings/source_group/component/SourceGroupSettingsWithSourceExtensions.cpp
	settings/source_group/component/SourceGroupSettingsWithSourceExtensions.h
	settings/source_group/component/SourceGroupSettingsWithSourceExtensionsEmpty.h
	settings/source_group/component/SourceGroupSettingsWithSourcePaths.cpp
	settings/source_group/component/SourceGroupSettingsWithSourcePaths.h

	settings/source_group/type/SourceGroupSettingsUnloadable.cpp
	settings/source_group/type/SourceGroupSettingsUnloadable.h

	settings/source_group/SourceGroupSettings.cpp
	settings/source_group/SourceGroupSettings.h
	settings/source_group/SourceGroupSettingsBase.h
	settings/source_group/SourceGroupSettingsWithComponents.h
	settings/source_group/SourceGroupStatusType.cpp
	settings/source_group/SourceGroupStatusType.h
	settings/source_group/SourceGroupType.cpp
	settings/source_group/SourceGroupType.h

	settings/ApplicationSettings.cpp
	settings/ApplicationSettings.h
	settings/ApplicationSettingsPrefiller.cpp
	settings/ApplicationSettingsPrefiller.h
	settings/ColorScheme.cpp
	settings/ColorScheme.h
	settings/LanguageType.cpp
	settings/LanguageType.h
	settings/ProjectSettings.cpp
	settings/ProjectSettings.h
	settings/Settings.cpp
	settings/Settings.h

	utility/commandline/CommandlineHelper.cpp
	utility/commandline/CommandlineHelper.h
	utility/commandline/CommandLineParser.cpp
	utility/commandline/CommandLineParser.h
	utility/commandline/commands/CommandlineCommand.cpp
	utility/commandline/commands/CommandlineCommand.h
	utility/commandline/commands/CommandlineCommandConfig.cpp
	utility/commandline/commands/CommandlineCommandConfig.h
	utility/commandline/commands/CommandlineCommandIndex.cpp
	utility/commandline/commands/CommandlineCommandIndex.h

	utility/file/FileInfo.cpp
	utility/file/FileInfo.h
	utility/file/FileManager.cpp
	utility/file/FileManager.h
	utility/file/FilePath.cpp
	utility/file/FilePath.h
	utility/file/FilePathFilter.cpp
	utility/file/FilePathFilter.h
	utility/file/FileRegister.cpp
	utility/file/FileRegister.h
	utility/file/FileSystem.cpp
	utility/file/FileSystem.h
	utility/file/FileTree.cpp
	utility/file/FileTree.h
	utility/file/utilityFile.cpp
	utility/file/utilityFile.h

	utility/interprocess/SharedMemory.cpp
	utility/interprocess/SharedMemory.h
	utility/interprocess/SharedMemoryGarbageCollector.cpp
	utility/interprocess/SharedMemoryGarbageCollector.h

	utility/logging/ConsoleLogger.cpp
	utility/logging/ConsoleLogger.h
	utility/logging/FileLogger.cpp
	utility/logging/FileLogger.h
	utility/logging/Logger.cpp
	utility/logging/Logger.h
	utility/logging/logging.h
	utility/logging/LogManager.cpp
	utility/logging/LogManager.h
	utility/logging/LogManagerImplementation.cpp
	utility/logging/LogManagerImplementation.h
	utility/logging/LogMessage.h

	utility/math/Color.h
	utility/math/MatrixBase.h
	utility/math/MatrixDynamicBase.h
	utility/math/Vector2.h
	utility/math/Vector4.h
	utility/math/VectorBase.h

	utility/messaging/filter_types/MessageFilterErrorCountUpdate.h
	utility/messaging/filter_types/MessageFilterFocusInOut.h
	utility/messaging/filter_types/MessageFilterSearchAutocomplete.h

	utility/messaging/type/activation/MessageActivateBase.h
	utility/messaging/type/activation/MessageActivateErrors.h
	utility/messaging/type/activation/MessageActivateFullTextSearch.h
	utility/messaging/type/activation/MessageActivateLegend.h
	utility/messaging/type/activation/MessageActivateOverview.h
	utility/messaging/type/activation/MessageActivateTokens.h
	utility/messaging/type/activation/MessageActivateTrail.h

	utility/messaging/type/bookmark/MessageBookmarkActivate.h
	utility/messaging/type/bookmark/MessageBookmarkBrowse.h
	utility/messaging/type/bookmark/MessageBookmarkButtonState.h
	utility/messaging/type/bookmark/MessageBookmarkCreate.h
	utility/messaging/type/bookmark/MessageBookmarkDelete.h
	utility/messaging/type/bookmark/MessageBookmarkEdit.h

	utility/messaging/type/code/MessageActivateFile.h
	utility/messaging/type/code/MessageActivateLocalSymbols.h
	utility/messaging/type/code/MessageActivateSourceLocations.h
	utility/messaging/type/code/MessageActivateTokenIds.h
	utility/messaging/type/code/MessageChangeFileView.h
	utility/messaging/type/code/MessageCodeReference.h
	utility/messaging/type/code/MessageCodeShowDefinition.h
	utility/messaging/type/code/MessageScrollCode.h
	utility/messaging/type/code/MessageScrollToLine.h
	utility/messaging/type/code/MessageShowReference.h
	utility/messaging/type/code/MessageShowScope.h

	utility/messaging/type/custom_trail/MessageCustomTrailShow.h

	utility/messaging/type/error/MessageErrorCountClear.h
	utility/messaging/type/error/MessageErrorCountUpdate.h
	utility/messaging/type/error/MessageErrorsAll.h
	utility/messaging/type/error/MessageErrorsForFile.h
	utility/messaging/type/error/MessageErrorsHelpMessage.h
	utility/messaging/type/error/MessageShowError.h

	utility/messaging/type/focus/MessageFocusChanged.h
	utility/messaging/type/focus/MessageFocusedSearchView.h
	utility/messaging/type/focus/MessageFocusIn.h
	utility/messaging/type/focus/MessageFocusOut.h
	utility/messaging/type/focus/MessageFocusView.h

	utility/messaging/type/graph/MessageActivateEdge.h
	utility/messaging/type/graph/MessageActivateNodes.h
	utility/messaging/type/graph/MessageActivateTrailEdge.h
	utility/messaging/type/graph/MessageDeactivateEdge.h
	utility/messaging/type/graph/MessageGraphNodeBundleSplit.h
	utility/messaging/type/graph/MessageGraphNodeExpand.h
	utility/messaging/type/graph/MessageGraphNodeHide.h
	utility/messaging/type/graph/MessageGraphNodeMove.h
	utility/messaging/type/graph/MessageScrollGraph.h
	utility/messaging/type/graph/MessageSaveAsImage.h

	utility/messaging/type/history/MessageHistoryToPosition.h
	utility/messaging/type/history/MessageHistoryRedo.h
	utility/messaging/type/history/MessageHistoryUndo.h

	utility/messaging/type/indexing/MessageIndexingFinished.h
	utility/messaging/type/indexing/MessageIndexingInterrupted.h
	utility/messaging/type/indexing/MessageIndexingShowDialog.h
	utility/messaging/type/indexing/MessageIndexingStarted.h
	utility/messaging/type/indexing/MessageIndexingStatus.h

	utility/messaging/type/plugin/MessageIDECreateCDB.h
	utility/messaging/type/plugin/MessageMoveIDECursor.h
	utility/messaging/type/plugin/MessagePingReceived.h
	utility/messaging/type/plugin/MessagePluginPortChange.h

	utility/messaging/type/search/MessageFind.h
	utility/messaging/type/search/MessageSearch.h
	utility/messaging/type/search/MessageSearchAutocomplete.h

	utility/messaging/type/tab/MessageTabClose.h
	utility/messaging/type/tab/MessageTabOpen.h
	utility/messaging/type/tab/MessageTabOpenWith.h
	utility/messaging/type/tab/MessageTabSelect.h
	utility/messaging/type/tab/MessageTabState.h

	utility/messaging/type/MessageActivateWindow.h
	utility/messaging/type/MessageClearStatusView.h
	utility/messaging/type/MessageCloseProject.h
	utility/messaging/type/MessageFlushUpdates.h
	utility/messaging/type/MessageLoadProject.h
	utility/messaging/type/MessageLogFilterChanged.h
	utility/messaging/type/MessageProjectEdit.h
	utility/messaging/type/MessageProjectNew.h
	utility/messaging/type/MessageQuitApplication.h
	utility/messaging/type/MessageRefresh.h
	utility/messaging/type/MessageRefreshUI.h
	utility/messaging/type/MessageRefreshUIState.h
	utility/messaging/type/MessageResetZoom.h
	utility/messaging/type/MessageShowStatus.h
	utility/messaging/type/MessageStatus.cpp
	utility/messaging/type/MessageStatus.h
	utility/messaging/type/MessageStatusFilterChanged.h
	utility/messaging/type/MessageSwitchColorScheme.h
	utility/messaging/type/MessageTooltipHide.h
	utility/messaging/type/MessageTooltipShow.h
	utility/messaging/type/MessageWindowChanged.h
	utility/messaging/type/MessageWindowClosed.h
	utility/messaging/type/MessageWindowFocus.h
	utility/messaging/type/MessageZoom.h

	utility/messaging/Message.h
	utility/messaging/MessageBase.cpp
	utility/messaging/MessageBase.h
	utility/messaging/MessageFilter.h
	utility/messaging/MessageListener.h
	utility/messaging/MessageListenerBase.cpp
	utility/messaging/MessageListenerBase.h
	utility/messaging/MessageQueue.cpp
	utility/messaging/MessageQueue.h

	utility/migration/Migration.h
	utility/migration/Migrator.h

	utility/scheduling/Blackboard.cpp
	utility/scheduling/Blackboard.h
	utility/scheduling/Task.cpp
	utility/scheduling/Task.h
	utility/scheduling/TaskDecorator.cpp
	utility/scheduling/TaskDecorator.h
	utility/scheduling/TaskDecoratorRepeat.cpp
	utility/scheduling/TaskDecoratorRepeat.h
	utility/scheduling/TaskDecoratorDelay.cpp
	utility/scheduling/TaskDecoratorDelay.h
	utility/scheduling/TaskFindKeyOnBlackboard.cpp
	utility/scheduling/TaskFindKeyOnBlackboard.h
	utility/scheduling/TaskGroup.cpp
	utility/scheduling/TaskGroup.h
	utility/scheduling/TaskGroupParallel.cpp
	utility/scheduling/TaskGroupParallel.h
	utility/scheduling/TaskGroupSelector.cpp
	utility/scheduling/TaskGroupSelector.h
	utility/scheduling/TaskGroupSequence.cpp
	utility/scheduling/TaskGroupSequence.h
	utility/scheduling/TaskLambda.cpp
	utility/scheduling/TaskLambda.h
	utility/scheduling/TaskManager.cpp
	utility/scheduling/TaskManager.h
	utility/scheduling/TaskReturnSuccessIf.h
	utility/scheduling/TaskRunner.cpp
	utility/scheduling/TaskRunner.h
	utility/scheduling/TaskScheduler.cpp
	utility/scheduling/TaskScheduler.h
	utility/scheduling/TaskSetValue.h

	utility/text/TextAccess.cpp
	utility/text/TextAccess.h

	utility/ApplicationArchitectureType.h
	utility/ConfigManager.cpp
	utility/ConfigManager.h
	utility/LowMemoryStringMap.h
	utility/Optional.h
	utility/OrderedCache.h
	utility/OsType.h
	utility/Property.h
	utility/ScopedFunctor.cpp
	utility/ScopedFunctor.h
	utility/ScopedSwitcher.h
	utility/SingleValueCache.h
	utility/TimeStamp.cpp
	utility/TimeStamp.h
	utility/tracing.cpp
	utility/tracing.h
	utility/Tree.h
	utility/types.h
	utility/UnorderedCache.h
	utility/utility.cpp
	utility/utility.h
	utility/utilityLibrary.h
	utility/utilityUuid.cpp
	utility/utilityUuid.h
	utility/utilityXml.cpp
	utility/utilityXml.h
	utility/Version.cpp
	utility/Version.h
)

if (WIN32)
	add_files(
		LIB

		utility/utilityWindows.cpp
		utility/utilityWindows.h
	)
endif()

if (BUILD_CXX_LANGUAGE_PACKAGE)
	add_files(
		LIB

		settings/source_group/component/cxx/SourceGroupSettingsWithCppStandard.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCppStandard.h
		settings/source_group/component/cxx/SourceGroupSettingsWithCStandard.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCStandard.h
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxCdbPath.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxCdbPath.h
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxCodeblocksPath.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxCodeblocksPath.h
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxCrossCompilationOptions.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxCrossCompilationOptions.h
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxPathsAndFlags.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxPathsAndFlags.h
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxPchOptions.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithCxxPchOptions.h
		settings/source_group/component/cxx/SourceGroupSettingsWithIndexedHeaderPaths.cpp
		settings/source_group/component/cxx/SourceGroupSettingsWithIndexedHeaderPaths.h
		settings/source_group/component/cxx/SourceGroupSettingsWithSourceExtensionsC.h
		settings/source_group/component/cxx/SourceGroupSettingsWithSourceExtensionsCpp.h

		settings/source_group/type/SourceGroupSettingsCEmpty.h
		settings/source_group/type/SourceGroupSettingsCppEmpty.h
		settings/source_group/type/SourceGroupSettingsCustomCommand.h
		settings/source_group/type/SourceGroupSettingsCxxCdb.h
		settings/source_group/type/SourceGroupSettingsCxxCodeblocks.h
	)
endif()

if (BUILD_JAVA_LANGUAGE_PACKAGE)
	add_files(
		LIB

		settings/source_group/component/java/SourceGroupSettingsWithClasspath.cpp
		settings/source_group/component/java/SourceGroupSettingsWithClasspath.h
		settings/source_group/component/java/SourceGroupSettingsWithJavaGradle.cpp
		settings/source_group/component/java/SourceGroupSettingsWithJavaGradle.h
		settings/source_group/component/java/SourceGroupSettingsWithJavaMaven.cpp
		settings/source_group/component/java/SourceGroupSettingsWithJavaMaven.h
		settings/source_group/component/java/SourceGroupSettingsWithJavaStandard.cpp
		settings/source_group/component/java/SourceGroupSettingsWithJavaStandard.h
		settings/source_group/component/java/SourceGroupSettingsWithSourceExtensionsJava.h

		settings/source_group/type/SourceGroupSettingsJavaEmpty.h
		settings/source_group/type/SourceGroupSettingsJavaGradle.h
		settings/source_group/type/SourceGroupSettingsJavaMaven.h
	)
endif()

if (BUILD_PYTHON_LANGUAGE_PACKAGE)
	add_files(
		LIB

		settings/source_group/component/python/SourceGroupSettingsWithPythonEnvironmentPath.cpp
		settings/source_group/component/python/SourceGroupSettingsWithPythonEnvironmentPath.h
		settings/source_group/component/python/SourceGroupSettingsWithSourceExtensionsPython.h

		settings/source_group/type/SourceGroupSettingsPythonEmpty.h
	)
endif()
