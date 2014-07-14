add_files(
	CLANG_FILES

	data/parser/cxx/ASTAction.cpp
	data/parser/cxx/ASTAction.h
	data/parser/cxx/ASTActionFactory.cpp
	data/parser/cxx/ASTActionFactory.h
	data/parser/cxx/ASTBodyVisitor.cpp
	data/parser/cxx/ASTBodyVisitor.h
	data/parser/cxx/ASTBodyVisitorClient.cpp
	data/parser/cxx/ASTBodyVisitorClient.h
	data/parser/cxx/ASTConsumer.cpp
	data/parser/cxx/ASTConsumer.h
	data/parser/cxx/ASTVisitor.cpp
	data/parser/cxx/ASTVisitor.h
	data/parser/cxx/CxxParser.cpp
	data/parser/cxx/CxxParser.h
	data/parser/cxx/utilityCxx.cpp
	data/parser/cxx/utilityCxx.h
)

add_files(
	LIB_FILES

	component/controller/CodeController.cpp
	component/controller/CodeController.h
	component/controller/Controller.cpp
	component/controller/Controller.h
	component/controller/GraphController.cpp
	component/controller/GraphController.h
	component/controller/SearchController.cpp
	component/controller/SearchController.h

	component/view/graphElements/GraphEdge.cpp
	component/view/graphElements/GraphEdge.h
	component/view/graphElements/GraphNode.cpp
	component/view/graphElements/GraphNode.h

	component/view/CodeView.cpp
	component/view/CodeView.h
	component/view/GraphView.cpp
	component/view/GraphView.h
	component/view/MainView.cpp
	component/view/MainView.h
	component/view/SearchView.cpp
	component/view/SearchView.h
	component/view/View.cpp
	component/view/View.h
	component/view/ViewLayout.cpp
	component/view/ViewLayout.h

	component/Component.cpp
	component/Component.h
	component/ComponentFactory.cpp
	component/ComponentFactory.h
	component/ComponentManager.cpp
	component/ComponentManager.h

	data/access/LocationAccess.cpp
	data/access/LocationAccess.h
	data/access/LocationAccessProxy.cpp
	data/access/LocationAccessProxy.h
	data/access/GraphAccess.cpp
	data/access/GraphAccess.h
	data/access/GraphAccessProxy.cpp
	data/access/GraphAccessProxy.h

	data/graph/edgeComponent/EdgeComponent.cpp
	data/graph/edgeComponent/EdgeComponent.h
	data/graph/edgeComponent/EdgeComponentDataType.cpp
	data/graph/edgeComponent/EdgeComponentDataType.h

	data/graph/Edge.cpp
	data/graph/Edge.h
	data/graph/Graph.cpp
	data/graph/Graph.h
	data/graph/Node.cpp
	data/graph/Node.h
	data/graph/Token.cpp
	data/graph/Token.h

	data/location/TokenLocation.cpp
	data/location/TokenLocation.h
	data/location/TokenLocationCollection.cpp
	data/location/TokenLocationCollection.h
	data/location/TokenLocationFile.cpp
	data/location/TokenLocationFile.h
	data/location/TokenLocationLine.cpp
	data/location/TokenLocationLine.h

	data/parser/ParseLocation.cpp
	data/parser/ParseLocation.h
	data/parser/Parser.cpp
	data/parser/Parser.h
	data/parser/ParserClient.cpp
	data/parser/ParserClient.h
	data/parser/ParseTypeUsage.cpp
	data/parser/ParseTypeUsage.h
	data/parser/ParseVariable.cpp
	data/parser/ParseVariable.h

	data/type/modifier/DataTypeModifier.cpp
	data/type/modifier/DataTypeModifier.h
	data/type/modifier/DataTypeModifierArray.cpp
	data/type/modifier/DataTypeModifierArray.h
	data/type/modifier/DataTypeModifierPointer.cpp
	data/type/modifier/DataTypeModifierPointer.h
	data/type/modifier/DataTypeModifierReference.cpp
	data/type/modifier/DataTypeModifierReference.h

	data/type/DataType.cpp
	data/type/DataType.h
	data/type/DataTypeModifierStack.cpp
	data/type/DataTypeModifierStack.h
	data/type/DataTypeQualifierList.cpp
	data/type/DataTypeQualifierList.h

	data/ElementIndex.cpp
	data/ElementIndex.h
	data/SearchIndex.cpp
	data/SearchIndex.h
	data/Storage.cpp
	data/Storage.h

	gui/GuiFactory.cpp
	gui/GuiFactory.h
	gui/GuiWidgetWrapper.cpp
	gui/GuiWidgetWrapper.h

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
	utility/logging/PlainFileLogger.cpp
	utility/logging/PlainFileLogger.h

	utility/math/Color.h
	utility/math/Vector2.h
	utility/math/Vector4.h
	utility/math/VectorBase.h

	utility/messaging/type/MessageActivateToken.h
	utility/messaging/type/MessageFinishedParsing.h
	utility/messaging/type/MessageLoadProject.h

	utility/messaging/Message.h
	utility/messaging/MessageBase.h
	utility/messaging/MessageListener.h
	utility/messaging/MessageListenerBase.h
	utility/messaging/MessageQueue.cpp
	utility/messaging/MessageQueue.h

	utility/text/TextAccess.cpp
	utility/text/TextAccess.h

	utility/ConfigManager.cpp
	utility/ConfigManager.h
	utility/FileSystem.cpp
	utility/FileSystem.h
	utility/Property.h
	utility/types.h
	utility/utilityString.cpp
	utility/utilityString.h
	utility/Vector.h

	Application.cpp
	Application.h
	ApplicationSettings.cpp
	ApplicationSettings.h
	Project.cpp
	Project.h
	ProjectSettings.cpp
	ProjectSettings.h
	Settings.cpp
	Settings.h
)
