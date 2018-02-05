#ifndef CXX_TYPE_NAME_H
#define CXX_TYPE_NAME_H

#include <memory>
#include <string>
#include <vector>

#include "data/name/NameHierarchy.h"
#include "data/parser/cxx/name/CxxName.h"
#include "data/parser/cxx/name/CxxQualifierFlags.h"

class CxxTypeName: public CxxName
{
public:
	static std::shared_ptr<CxxTypeName> makeUnsolvedIfNull(std::shared_ptr<CxxTypeName> name);

	struct Modifier
	{
		Modifier(std::wstring&& symbol);
		std::wstring symbol;
		CxxQualifierFlags qualifierFlags;
	};

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxTypeName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateArguments
	//);

	CxxTypeName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateArguments
	);

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxTypeName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateArguments,
	//	std::shared_ptr<CxxName> parent
	//);

	CxxTypeName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateArguments,
		std::shared_ptr<CxxName> parent
	);

	virtual NameHierarchy toNameHierarchy() const;

	void addQualifier(const CxxQualifierFlags::QualifierType qualifier);
	void addModifier(const Modifier& modifier);

	std::wstring toString() const;

private:
	std::wstring getTypeNameString() const;

	std::wstring m_name;
	std::vector<std::wstring> m_templateArguments;

	CxxQualifierFlags m_qualifierFlags;
	std::vector<Modifier> m_modifiers;
};

#endif // CXX_TYPE_NAME_H
