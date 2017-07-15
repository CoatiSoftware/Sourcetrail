#ifndef NAME_HIERARCHY_H
#define NAME_HIERARCHY_H

#include <memory>
#include <string>
#include <vector>

#include "data/name/NameDelimiterType.h"
#include "data/name/NameElement.h"

class NameHierarchy
{
public:
	static std::string serialize(NameHierarchy nameHierarchy);
	static NameHierarchy deserialize(const std::string& serializedName);

	NameHierarchy(const NameDelimiterType delimiter);
	NameHierarchy(const std::string& name, const NameDelimiterType delimiter);
	NameHierarchy(const std::vector<std::string>& names, const NameDelimiterType delimiter);
	~NameHierarchy();

	NameDelimiterType getDelimiter() const;
	void setDelimiter(const NameDelimiterType delimiter);

	void push(std::shared_ptr<NameElement> element);
	void pop();

	std::shared_ptr<NameElement> back() const;
	std::shared_ptr<NameElement> operator[](size_t pos) const;

	NameHierarchy getRange(size_t first, size_t last) const;

	size_t size() const;

	std::string getQualifiedName() const;
	std::string getQualifiedNameWithSignature() const;
	std::string getRawName() const;
	std::string getRawNameWithSignature() const;

private:
	std::vector<std::shared_ptr<NameElement>> m_elements;
	NameDelimiterType m_delimiter;
};

#endif // NAME_ELEMENT_H
