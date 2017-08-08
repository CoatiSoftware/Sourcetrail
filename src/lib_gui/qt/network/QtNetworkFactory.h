#ifndef QT_NETWORK_FACTORY_H
#define QT_NETWORK_FACTORY_H

#include "component/NetworkFactory.h"

class QtNetworkFactory
	: public NetworkFactory
{
public:
	QtNetworkFactory();
	virtual ~QtNetworkFactory();

	virtual std::shared_ptr<IDECommunicationController>
		createIDECommunicationController(StorageAccess* storageAccess) const override;
	virtual std::shared_ptr<UpdateChecker> createUpdateChecker() const override;
};

#endif // QT_NETWORK_FACTORY_H
