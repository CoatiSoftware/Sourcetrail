#include "UUIDUtility.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

UUID UUIDUtility::getUUID()
{
	return boost::uuids::random_generator()();
}

std::string UUIDUtility::UUIDtoString(const UUID& uuid)
{
	return boost::lexical_cast<std::string>(uuid);
}

std::string UUIDUtility::getUUIDString()
{
	return UUIDtoString(getUUID());
}
