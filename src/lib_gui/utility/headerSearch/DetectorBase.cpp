#include "utility/headerSearch/DetectorBase.h"

DetectorBase::DetectorBase(const std::string name)
	: m_name(name)
{
}

DetectorBase::~DetectorBase()
{
}

std::string DetectorBase::getName() const
{
	return m_name;
}

bool DetectorBase::isWorking()
{
	return !getStandardHeaderPaths().empty();
}
