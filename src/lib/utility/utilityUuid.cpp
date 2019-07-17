#include "utilityUuid.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace utility
{
	boost::uuids::uuid getUuid()
	{
		return boost::uuids::random_generator()();
	}

	std::string uuidToString(const boost::uuids::uuid& uuid)
	{
		return boost::lexical_cast<std::string>(uuid);
	}

	std::string getUuidString()
	{
		return utility::uuidToString(utility::getUuid());
	}
}
