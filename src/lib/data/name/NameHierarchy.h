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

	static const std::string& getDelimiter();
	static void setDelimiter(const std::string& delimiter);

	NameHierarchy();
	NameHierarchy(const std::string& name);
	NameHierarchy(const std::vector<std::string>& names);
	~NameHierarchy();

	void push(std::shared_ptr<NameElement> element);
	void pop();
	std::shared_ptr<NameElement> back() const;
	std::shared_ptr<NameElement> operator[](size_t pos) const;
	size_t size() const;

	std::string getQualifiedName() const;
	std::string getQualifiedNameWithSignature() const;
	std::string getRawName() const;
	std::string getRawNameWithSignature() const;

private:
	static std::string s_delimiter;

	std::vector<std::shared_ptr<NameElement>> m_elements;
};

#endif // NAME_ELEMENT_H
