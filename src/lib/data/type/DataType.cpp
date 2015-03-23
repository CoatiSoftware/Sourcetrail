#include "data/type/DataType.h"

DataType::DataType()
	: m_qualifiers(QUALIFIER_NONE)
{
}

DataType::~DataType()
{
}

void DataType::addQualifier(QualifierType qualifier)
{
	m_qualifiers = m_qualifiers | qualifier;
}

void DataType::removeQualifier(QualifierType qualifier)
{
	m_qualifiers = m_qualifiers & ~qualifier;
}

bool DataType::hasQualifier(QualifierType qualifier) const
{
	return (m_qualifiers & qualifier) > 0;
}

void DataType::applyQualifieres(std::string& typeName) const
{
	if (hasQualifier(QUALIFIER_CONST))
	{
		typeName += " const";
	}
}