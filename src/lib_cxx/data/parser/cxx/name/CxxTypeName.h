#ifndef CXX_TYPE_NAME_H
#define CXX_TYPE_NAME_H

#include <memory>
#include <string>
#include <vector>

#include "CxxName.h"
#include "CxxQualifierFlags.h"
#include "NameHierarchy.h"

class CxxTypeName: public CxxName
{
public:
	static std::unique_ptr<CxxTypeName> getUnsolved();
	static std::unique_ptr<CxxTypeName> makeUnsolvedIfNull(std::unique_ptr<CxxTypeName> name);

	struct Modifier
	{
		Modifier(std::wstring symbol);

		const std::wstring symbol;
		CxxQualifierFlags qualifierFlags;
	};

	CxxTypeName(std::wstring name);

	CxxTypeName(std::wstring name, std::vector<std::wstring> templateArguments);

	CxxTypeName(
		std::wstring name,
		std::vector<std::wstring> templateArguments,
		std::shared_ptr<CxxName> parent);

	NameHierarchy toNameHierarchy() const override;

	void addQualifier(const CxxQualifierFlags::QualifierType qualifier);
	void addModifier(Modifier modifier);

	std::wstring toString() const;

private:
	const std::wstring m_name;
	const std::vector<std::wstring> m_templateArguments;

	CxxQualifierFlags m_qualifierFlags;
	std::vector<Modifier> m_modifiers;
};

#endif	  // CXX_TYPE_NAME_H
