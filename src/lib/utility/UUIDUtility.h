#ifndef UUID_UTILITY_H
#define UUID_UTILITY_H

#include <boost/uuid/uuid.hpp>
#include <string>

class UUIDUtility
{
public:
	static boost::uuids::uuid getUUID();
	static std::string UUIDtoString(const boost::uuids::uuid& uuid);
	static std::string getUUIDString();
};

#endif // UUID_UTILITY_H
