#include "QtNetworkFactory.h"

#include "QtIDECommunicationController.h"

QtNetworkFactory::QtNetworkFactory() {}

QtNetworkFactory::~QtNetworkFactory() {}

std::shared_ptr<IDECommunicationController> QtNetworkFactory::createIDECommunicationController(
	StorageAccess* storageAccess) const
{
	return std::make_shared<QtIDECommunicationController>(nullptr, storageAccess);
}
