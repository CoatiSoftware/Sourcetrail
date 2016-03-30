#include "utility/headerSearch/DetectorBase.h"

#include <iostream>

DetectorBase::DetectorBase(const std::string name)
{
	setName(name);
}

bool DetectorBase::detect()
{
	return !getStandardHeaderPaths().empty();
}

std::string DetectorBase::getName() const
{
	return m_name;
}

void DetectorBase::setName(const std::string& name)
{
	if (!name.empty())
	{
		m_name = name;
	}
}
