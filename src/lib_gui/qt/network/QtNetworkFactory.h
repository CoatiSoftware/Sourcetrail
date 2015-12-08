#ifndef QT_NETWORK_FACTORY_H
#define QT_NETWORK_FACTORY_H

#include "component/controller/NetworkFactory.h"

class QtNetworkFactory : public NetworkFactory
{
public:
	QtNetworkFactory();
	virtual ~QtNetworkFactory();

	virtual std::shared_ptr<IDECommunicationController> createIDECommunicationController(StorageAccess* storageAccess) const;
};

#endif // QT_NETWORK_FACTORY_H