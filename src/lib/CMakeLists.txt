add_files(
	CLANG_FILES

	data/parser/cxx/ASTAction.cpp
	data/parser/cxx/ASTAction.h
	data/parser/cxx/ASTActionFactory.cpp
	data/parser/cxx/ASTActionFactory.h
	data/parser/cxx/ASTConsumer.cpp
	data/parser/cxx/ASTConsumer.h
	data/parser/cxx/ASTVisitor.cpp
	data/parser/cxx/ASTVisitor.h
	data/parser/cxx/CxxParser.cpp
	data/parser/cxx/CxxParser.h
)

add_files(
	LIB_FILES

	component/controller/CodeController.cpp
	component/controller/CodeController.h
	component/controller/Controller.cpp
	component/controller/Controller.h
	component/controller/DummyController.cpp
	component/controller/DummyController.h

	component/view/DummyView.cpp
	component/view/DummyView.h
	component/view/View.cpp
	component/view/View.h
	component/view/ViewManager.cpp
	component/view/ViewManager.h

	component/Component.cpp
	component/Component.h
	component/ComponentFactory.cpp
	component/ComponentFactory.h
	component/ComponentManager.cpp
	component/ComponentManager.h

	data/access/CodeAccess.cpp
	data/access/CodeAccess.h
	data/access/GraphAccess.cpp
	data/access/GraphAccess.h

	data/parser/ParseLocation.cpp
	data/parser/ParseLocation.h
	data/parser/Parser.cpp
	data/parser/Parser.h
	data/parser/ParserClient.cpp
	data/parser/ParserClient.h
	data/parser/ParseVariable.cpp
	data/parser/ParseVariable.h

	data/Edge.cpp
	data/Edge.h
	data/Element.cpp
	data/Element.h
	data/ElementIndex.cpp
	data/ElementIndex.h
	data/Graph.cpp
	data/Graph.h
	data/Node.cpp
	data/Node.h
	data/SearchIndex.cpp
	data/SearchIndex.h
	data/Storage.cpp
	data/Storage.h
	data/TextLocation.cpp
	data/TextLocation.h
	data/TextLocationFile.cpp
	data/TextLocationFile.h
	data/TextLocationLine.cpp
	data/TextLocationLine.h

	gui/GuiArea.cpp
	gui/GuiArea.h
	gui/GuiCanvas.cpp
	gui/GuiCanvas.h
	gui/GuiContainer.cpp
	gui/GuiContainer.h
	gui/GuiControl.cpp
	gui/GuiControl.h
	gui/GuiElement.cpp
	gui/GuiElement.h
	gui/GuiElementFactory.cpp
	gui/GuiElementFactory.h
	gui/GuiLayout.cpp
	gui/GuiLayout.h
	gui/GuiWindow.cpp
	gui/GuiWindow.h
	gui/WidgetWrapper.cpp
	gui/WidgetWrapper.h

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
	utility/math/Vector2.h
	utility/math/Vector4.h

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
	utility/Vector.h

	Application.cpp
	Application.h
	ApplicationSettings.cpp
	ApplicationSettings.h
	Project.cpp
	Project.h
	ProjectSettings.cpp
	ProjectSettings.h
)
