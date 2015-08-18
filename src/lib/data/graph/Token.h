#ifndef TOKEN_H
#define TOKEN_H

#include <typeinfo>
#include <vector>

#include "data/graph/token_component/TokenComponent.h"
#include "utility/types.h"

class Token
{
public:
	static void resetNextId();

	Token();
	virtual ~Token();

	Id getId() const;

	virtual bool isNode() const = 0;
	virtual bool isEdge() const = 0;

	const std::vector<Id>& getLocationIds() const;

	void addLocationId(Id locationId);
	void removeLocationId(Id locationId);

	template <typename ComponentType>
	ComponentType* getComponent() const;

	template <typename ComponentType>
	std::shared_ptr<ComponentType> removeComponent();

	// Logging.
	virtual std::string getTypeString() const = 0;

protected:
	Token(const Token& other);

	void addComponent(std::shared_ptr<TokenComponent> component);
	void copyComponentsFrom(const Token& other);

private:
	static Id s_nextId;

	void operator=(const Token&);

	const Id m_id;	// own id

	std::vector<Id> m_locationIds;
	std::vector<std::shared_ptr<TokenComponent>> m_components;
};

template <typename ComponentType>
ComponentType* Token::getComponent() const
{
	for (std::shared_ptr<TokenComponent> component: m_components)
	{
		TokenComponent* componentPtr = component.get();
		if (typeid(ComponentType) == typeid(*componentPtr))
		{
			return dynamic_cast<ComponentType*>(component.get());
		}
	}
	return nullptr;
}

template <typename ComponentType>
std::shared_ptr<ComponentType> Token::removeComponent()
{
	for (size_t i = 0; i < m_components.size(); i++)
	{
		std::shared_ptr<TokenComponent> component = m_components[i];
		TokenComponent* componentPtr = component.get();
		if (typeid(ComponentType) == typeid(*componentPtr))
		{
			m_components.erase(m_components.begin() + i);
			return std::dynamic_pointer_cast<ComponentType>(component);
		}
	}
	return nullptr;
}

#endif // TOKEN_H
