#include "QtNetworkFactory.h"

#include "QtIDECommunicationController.h"
#include "QtUpdateChecker.h"

QtNetworkFactory::QtNetworkFactory()
{
}

QtNetworkFactory::~QtNetworkFactory()
{
}

std::shared_ptr<IDECommunicationController> QtNetworkFactory::createIDECommunicationController(StorageAccess* storageAccess) const
{
	return std::make_shared<QtIDECommunicationController>(nullptr, storageAccess);
}

std::shared_ptr<UpdateChecker> QtNetworkFactory::createUpdateChecker() const
{
	return std::make_shared<QtUpdateChecker>();
}
