#ifndef NAME_HIERARCHY_H
#define NAME_HIERARCHY_H

#include <memory>
#include <string>
#include <vector>

#include "NameDelimiterType.h"
#include "NameElement.h"

class NameHierarchy
{
public:
	static std::wstring serialize(const NameHierarchy& nameHierarchy);
	static NameHierarchy deserialize(const std::wstring& serializedName);

	NameHierarchy(const NameDelimiterType delimiter = NAME_DELIMITER_UNKNOWN);
	NameHierarchy(const std::wstring& name, const NameDelimiterType delimiter);
	NameHierarchy(const std::vector<std::wstring>& names, const NameDelimiterType delimiter);
	NameHierarchy(const NameHierarchy& other);
	NameHierarchy(NameHierarchy&& other);
	~NameHierarchy();

	NameDelimiterType getDelimiter() const;
	void setDelimiter(const NameDelimiterType delimiter);

	void push(std::shared_ptr<NameElement> element);
	void pop();

	std::shared_ptr<NameElement> back() const;
	std::shared_ptr<NameElement> operator[](size_t pos) const;
	NameHierarchy& operator=(const NameHierarchy& other);
	NameHierarchy& operator=(NameHierarchy&& other);

	NameHierarchy getRange(size_t first, size_t last) const;

	size_t size() const;

	std::wstring getQualifiedName() const;
	std::wstring getQualifiedNameWithSignature() const;
	std::wstring getRawName() const;
	std::wstring getRawNameWithSignature() const;
	std::wstring getRawNameWithSignatureParameters() const;

	bool hasSignature() const;
	NameElement::Signature getSignature() const;

private:
	std::vector<std::shared_ptr<NameElement>> m_elements;
	NameDelimiterType m_delimiter;
};

#endif // NAME_ELEMENT_H
