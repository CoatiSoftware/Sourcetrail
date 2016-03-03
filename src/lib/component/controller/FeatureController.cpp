#include "component/controller/FeatureController.h"

#include "data/access/StorageAccess.h"
#include "settings/ApplicationSettings.h"

#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"

FeatureController::FeatureController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_activationTranslator(storageAccess)
{
}

FeatureController::~FeatureController()
{
}

void FeatureController::handleMessage(MessageActivateEdge* message)
{
	std::shared_ptr<MessageActivateTokens> m = m_activationTranslator.translateMessage(message);
	if (m)
	{
		m->dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateFile* message)
{
	std::shared_ptr<MessageActivateTokens> m = m_activationTranslator.translateMessage(message);
	if (m)
	{
		m->dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateNodes* message)
{
	std::shared_ptr<MessageActivateTokens> m = m_activationTranslator.translateMessage(message);
	if (m)
	{
		m->dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageSearch* message)
{
	std::shared_ptr<MessageActivateTokens> m = m_activationTranslator.translateMessage(message);
	if (m)
	{
		m->dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateTokenIds* message)
{
	std::shared_ptr<MessageActivateTokens> m = m_activationTranslator.translateMessage(message);
	if (m)
	{
		m->dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateTokenLocations* message)
{
	std::vector<Id> nodeIds = m_storageAccess->getNodeIdsForLocationIds(message->locationIds);
	nodeIds = m_storageAccess->getActiveTokenIdsForTokenIds(nodeIds);

	MessageActivateNodes m;
	for (Id nodeId : nodeIds)
	{
		m.addNode(
			nodeId,
			m_storageAccess->getNodeTypeForNodeWithId(nodeId),
			m_storageAccess->getNameHierarchyForNodeWithId(nodeId)
			);
	}
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageResetZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	int fontSizeStd = settings->getFontSizeStd();

	if (settings->getFontSize() != fontSizeStd)
	{
		settings->setFontSize(fontSizeStd);
		settings->save();

		MessageRefresh().refreshUiOnly().dispatch();
	}

	MessageStatus("Zoom: 100%").dispatch();
}

void FeatureController::handleMessage(MessageSwitchColorScheme* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setColorSchemePath(message->colorSchemeFilePath);
	settings->save();

	MessageRefresh().refreshUiOnly().dispatch();
}

void FeatureController::handleMessage(MessageZoom* message)
{
	bool zoomIn = message->zoomIn;

	ApplicationSettings* settings = ApplicationSettings::getInstance().get();

	int fontSize = settings->getFontSize();
	int standardSize = settings->getFontSizeStd();
	int maxSize = settings->getFontSizeMax();
	int minSize = settings->getFontSizeMin();

	if ((fontSize >= maxSize && zoomIn)
		|| (fontSize <= minSize && !zoomIn))
	{
		return;
	}

	// settings->setFontSize(std::max(settings->getFontSize() + (message->zoomIn ? 1 : -1), 5));
	settings->setFontSize((settings->getFontSize() + (message->zoomIn ? 1 : -1)));
	settings->save();

	fontSize = settings->getFontSize();
	int zoom = (fontSize * 100) / standardSize;

	std::stringstream text;
	text << "Zoom: " << zoom << "%";
	MessageStatus(text.str()).dispatch();

	MessageRefresh().refreshUiOnly().dispatch();
}

