#ifndef CXX_QUALIFIER_FLAGS_H
#define CXX_QUALIFIER_FLAGS_H

#include <string>

class CxxQualifierFlags
{
public:
	enum QualifierType
	{
		QUALIFIER_NONE = 0,
		QUALIFIER_CONST = 1
	};

	CxxQualifierFlags();
	CxxQualifierFlags(const char flags);

	void addQualifier(QualifierType qualifier);
	void removeQualifier(QualifierType qualifier);

	bool empty() const;
	std::wstring toString() const;

private:
	char m_flags;
};

#endif	  // CXX_QUALIFIER_FLAGS_H
