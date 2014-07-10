#include "data/type/modifier/DataTypeModifier.h"

DataTypeModifier::DataTypeModifier()
{
}

DataTypeModifier::~DataTypeModifier()
{
}

void DataTypeModifier::applyTo(std::string& typeName) const
{
	doApplyTo(typeName);
	typeName = m_qualifierList.applyTo(typeName);
}

void DataTypeModifier::addQualifier(DataTypeQualifierList::QualifierType qualifier)
{
	m_qualifierList.addQualifier(qualifier);
}

void DataTypeModifier::removeQualifier(DataTypeQualifierList::QualifierType qualifier)
{
	m_qualifierList.removeQualifier(qualifier);
}

bool DataTypeModifier::hasQualifier(DataTypeQualifierList::QualifierType qualifier) const
{
	return m_qualifierList.hasQualifier(qualifier);
}