#ifndef UTILITY_UUID_H
#define UTILITY_UUID_H

#include <boost/uuid/uuid.hpp>
#include <string>

namespace utility
{
	boost::uuids::uuid getUuid();
	std::string uuidToString(const boost::uuids::uuid& uuid);
	std::string getUuidString();
};

#endif // UTILITY_UUID_H
