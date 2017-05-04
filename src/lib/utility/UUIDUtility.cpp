#include "UUIDUtility.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

boost::uuids::uuid UUIDUtility::getUUID()
{
	return boost::uuids::random_generator()();
}

std::string UUIDUtility::UUIDtoString(const boost::uuids::uuid& uuid)
{
	return boost::lexical_cast<std::string>(uuid);
}

std::string UUIDUtility::getUUIDString()
{
	return UUIDtoString(getUUID());
}
