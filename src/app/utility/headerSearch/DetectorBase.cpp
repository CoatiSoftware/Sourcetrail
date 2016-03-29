#include "utility/headerSearch/DetectorBase.h"

#include <iostream>

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
	std::cout << "setName()" << std::endl;
	if (!name.empty())
	{
		std::cout << "name set to " << name << std::endl;
		m_name = name;
	}
}
