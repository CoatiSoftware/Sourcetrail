#ifndef NAME_HIERARCHY_H
#define NAME_HIERARCHY_H

#include <memory>
#include <string>
#include <vector>

#include "data/name/NameElement.h"

class NameHierarchy
{
public:
	NameHierarchy();
	NameHierarchy(const std::string& name);
	~NameHierarchy();

	void push(std::shared_ptr<NameElement> element);
	void pop();
	std::shared_ptr<NameElement> back();
	std::shared_ptr<NameElement> operator[](size_t pos) const;
	size_t size() const;

	std::string getFullName() const;
	std::string getName() const;

private:
	std::vector<std::shared_ptr<NameElement>> m_elements;
};

#endif // NAME_ELEMENT_H
