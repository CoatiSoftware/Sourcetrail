#ifndef NETWORK_FACTORY_H
#define NETWORK_FACTORY_H

#include <memory>

class IDECommunicationController;

class NetworkFactory
{
public:
	NetworkFactory();
	virtual ~NetworkFactory();

	virtual std::shared_ptr<IDECommunicationController> createIDECommunicationController() const = 0;
};

#endif // NETWORK_FACTORY_H