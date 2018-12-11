#ifndef NAME_HIERARCHY_H
#define NAME_HIERARCHY_H

#include <string>
#include <vector>

#include "NameDelimiterType.h"
#include "NameElement.h"

class NameHierarchy
{
public:
	static std::wstring serialize(const NameHierarchy& nameHierarchy);
	static std::wstring serializeRange(const NameHierarchy& nameHierarchy, size_t first, size_t last);
	static NameHierarchy deserialize(const std::wstring& serializedName);

	NameHierarchy(std::wstring delimiter);
	NameHierarchy(std::wstring name, std::wstring delimiter);
	NameHierarchy(const std::vector<std::wstring>& names, std::wstring delimiter);

	NameHierarchy(const NameDelimiterType delimiterType = NAME_DELIMITER_UNKNOWN);
	NameHierarchy(std::wstring name, const NameDelimiterType delimiterType);
	NameHierarchy(const std::vector<std::wstring>& names, const NameDelimiterType delimiterType);

	NameHierarchy(const NameHierarchy& other);
	NameHierarchy(NameHierarchy&& other);
	~NameHierarchy();

	const std::wstring& getDelimiter() const;
	void setDelimiter(std::wstring delimiter);

	void push(NameElement element);
	void push(std::wstring name);
	void pop();

	NameElement& back();
	const NameElement& back() const;
	NameElement& operator[](size_t pos);
	const NameElement& operator[](size_t pos) const;

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
	std::vector<NameElement> m_elements;
	std::wstring m_delimiter;
};

#endif // NAME_ELEMENT_H
