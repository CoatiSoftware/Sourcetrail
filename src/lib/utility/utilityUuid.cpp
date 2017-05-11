#include "utility/utilityUuid.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

boost::uuids::uuid utility::getUuid()
{
	return boost::uuids::random_generator()();
}

std::string utility::uuidToString(const boost::uuids::uuid& uuid)
{
	return boost::lexical_cast<std::string>(uuid);
}

std::string utility::getUuidString()
{
	return utility::uuidToString(utility::getUuid());
}
