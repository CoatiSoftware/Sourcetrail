#include "utility/headerSearch/DetectorBase.h"

DetectorBase::DetectorBase(const std::string name)
{
	setName(name);
}

bool DetectorBase::detect()
{
	return !getStandardHeaderPaths().empty();
}

std::vector<FilePath> DetectorBase::getStandardFrameworkPaths()
{
	return std::vector<FilePath>();
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
