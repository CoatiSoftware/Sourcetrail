#ifndef NAME_ELEMENT_H
#define NAME_ELEMENT_H

#include <memory>
#include <string>
#include <vector>

class DataType;

class NameElement
{
public:
	class Signature
	{
	public:
		Signature();
		Signature(std::wstring prefix, std::wstring postfix);

		std::wstring qualifyName(const std::wstring& name) const;
		bool isValid() const;

		const std::wstring& getPrefix() const;
		const std::wstring& getPostfix() const;
		std::wstring getParameterString() const;

	private:
		const std::wstring m_prefix;
		const std::wstring m_postfix;
	};

	NameElement(std::wstring name);
	NameElement(std::wstring name, std::wstring prefix, std::wstring postfix);
	~NameElement();

	const std::wstring& getName() const;
	std::wstring getNameWithSignature() const;
	std::wstring getNameWithSignatureParameters() const;
	bool hasSignature() const;
	const Signature& getSignature();

private:
	const std::wstring m_name;
	const Signature m_signature;
};

#endif // NAME_ELEMENT_H
