#ifndef UUID_UTILITY_H
#define UUID_UTILITY_H

#include <string>
#include <boost/uuid/uuid.hpp>

typedef boost::uuids::uuid UUID;

class UUIDUtility
{
public:
	static UUID getUUID();
	static std::string UUIDtoString(const UUID& uuid);
	static std::string getUUIDString();
};

#endif // UUID_UTILITY_H
