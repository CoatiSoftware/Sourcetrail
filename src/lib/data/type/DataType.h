#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <string>
#include <vector>

class DataType
{
public:
	enum QualifierType
	{
		QUALIFIER_NONE = 0,
		QUALIFIER_CONST = 1
	};

	DataType();
	virtual ~DataType();

	virtual std::string getFullTypeName() const = 0;
	virtual std::string getRawTypeName() const = 0;
	virtual const std::vector<std::string>& getTypeNameHierarchy() const = 0;

	void addQualifier(QualifierType qualifier);
	void removeQualifier(QualifierType qualifier);
	bool hasQualifier(QualifierType qualifier) const;

protected:
	void applyQualifieres(std::string& typeName) const;

private:
	char m_qualifiers;
};

#endif // DATA_TYPE_H
