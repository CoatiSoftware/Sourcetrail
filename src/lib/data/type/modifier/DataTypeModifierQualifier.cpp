#include "data/type/modifier/DataTypeModifierQualifier.h"

DataTypeModifierQualifier::DataTypeModifierQualifier()
	: m_qualifiers(QUALIFIER_NONE)
{
}

DataTypeModifierQualifier::~DataTypeModifierQualifier()
{
}

void DataTypeModifierQualifier::addQualifier(QualifierType qualifier)
{
	m_qualifiers = m_qualifiers | qualifier;
}

void DataTypeModifierQualifier::removeQualifier(QualifierType qualifier)
{
	m_qualifiers = m_qualifiers & ~qualifier;
}

bool DataTypeModifierQualifier::hasQualifier(QualifierType qualifier) const
{
	return (m_qualifiers & qualifier) > 0;
}

void DataTypeModifierQualifier::applyTo(std::string& typeName) const
{
	if (hasQualifier)
	{
		typeName.append(" const");
	}
}
