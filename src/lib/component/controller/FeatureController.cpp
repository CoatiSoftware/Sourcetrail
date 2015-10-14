#include "component/controller/FeatureController.h"

#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageRefresh.h"

#include "data/access/StorageAccess.h"
#include "settings/ApplicationSettings.h"

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

void FeatureController::handleMessage(MessageSwitchColorScheme* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setColorSchemePath(message->colorSchemeFilePath);
	settings->save();

	MessageRefresh().refreshUiOnly().dispatch();
}

void FeatureController::handleMessage(MessageZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setFontSize(std::max(settings->getFontSize() + (message->zoomIn ? 1 : -1), 5));
	settings->save();

	MessageRefresh().refreshUiOnly().dispatch();
}

