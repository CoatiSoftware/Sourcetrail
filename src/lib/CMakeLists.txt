add_files(
	CLANG_FILES

	data/parser/clang.cpp
	data/parser/clang.h
)

add_files(
	LIB_FILES

	component/controller/CodeController.cpp
	component/controller/CodeController.h
	component/controller/Controller.cpp
	component/controller/Controller.h

	component/view/CodeView.cpp
	component/view/CodeView.h
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

	data/Edge.cpp
	data/Edge.h
	data/Element.cpp
	data/Element.h
	data/ElementIndex.cpp
	data/ElementIndex.h
	data/Graph.cpp
	data/Graph.h
	data/GraphStorage.cpp
	data/GraphStorage.h
	data/Node.cpp
	data/Node.h
	data/Parser.cpp
	data/Parser.h
	data/SearchIndex.cpp
	data/SearchIndex.h
	data/TextLocation.cpp
	data/TextLocation.h
	data/TextLocationFile.cpp
	data/TextLocationFile.h
	data/TextLocationLine.cpp
	data/TextLocationLine.h
	data/TextLocationStorage.cpp
	data/TextLocationStorage.h

	gui/GuiElement.cpp
	gui/GuiElement.h
	gui/GuiElementFactory.cpp
	gui/GuiElementFactory.h

	utility/logging/ConsoleLogger.cpp
	utility/logging/ConsoleLogger.h
	utility/logging/Logger.cpp
	utility/logging/Logger.h
	utility/logging/logging.h
	utility/logging/LogManager.cpp
	utility/logging/LogManager.h
	utility/logging/LogMessage.h

	utility/math/Vector2.h

	utility/ConfigManager.cpp
	utility/ConfigManager.h

	Application.cpp
	Application.h
	ApplicationSettings.cpp
	ApplicationSettings.h
	Project.cpp
	Project.h
	ProjectSettings.cpp
	ProjectSettings.h
)
