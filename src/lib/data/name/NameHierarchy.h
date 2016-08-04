#ifndef NAME_HIERARCHY_H
#define NAME_HIERARCHY_H

#include <memory>
#include <string>
#include <vector>

#include "data/name/NameElement.h"

class NameHierarchy
{
public:
	static std::string serialize(NameHierarchy nameHierarchy);
	static NameHierarchy deserialize(const std::string& serializedName);

	NameHierarchy();
	NameHierarchy(const std::string& name);
	NameHierarchy(const std::vector<std::string>& names);
	~NameHierarchy();

	void push(std::shared_ptr<NameElement> element);
	void pop();
	std::shared_ptr<NameElement> back() const;
	std::shared_ptr<NameElement> operator[](size_t pos) const;
	size_t size() const;

	std::string getQualifiedName(const std::string& delimiter = "::") const;
	std::string getQualifiedNameWithSignature(const std::string& delimiter = "::") const;
	std::string getRawName() const;
	std::string getRawNameWithSignature() const;

private:
	std::vector<std::shared_ptr<NameElement>> m_elements;
};

#endif // NAME_ELEMENT_H
