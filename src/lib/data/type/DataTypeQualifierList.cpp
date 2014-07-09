#include "data/type/DataTypeQualifierList.h"

DataTypeQualifierList::DataTypeQualifierList()
	: m_qualifiers(QUALIFIER_NONE)
{
}

DataTypeQualifierList::~DataTypeQualifierList()
{
}

void DataTypeQualifierList::addQualifier(QualifierType qualifier)
{
	m_qualifiers = m_qualifiers | qualifier;
}

void DataTypeQualifierList::removeQualifier(QualifierType qualifier)
{
	m_qualifiers = m_qualifiers & ~qualifier;
}

bool DataTypeQualifierList::hasQualifier(QualifierType qualifier) const
{
	return (m_qualifiers & qualifier) > 0;
}

std::string DataTypeQualifierList::applyTo(const std::string& typeName) const
{
	std::string qualifiedTypeName = typeName;
	if (hasQualifier(QUALIFIER_CONST))
	{
		qualifiedTypeName += " const";
	}
	return qualifiedTypeName;
}