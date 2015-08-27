#ifndef TOKEN_LOCATION_LINE_H
#define TOKEN_LOCATION_LINE_H

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "utility/file/FilePath.h"
#include "utility/types.h"

class TokenLocation;
class TokenLocationFile;

class TokenLocationLine
{
public:
	typedef std::multimap<unsigned int, std::shared_ptr<TokenLocation> > TokenLocationMapType;
	typedef std::pair<unsigned int, std::shared_ptr<TokenLocation> > TokenLocationPairType;

	TokenLocationLine(TokenLocationFile* file, unsigned int lineNumber);
	~TokenLocationLine();

	const TokenLocationMapType& getTokenLocations() const;
	size_t getTokenLocationCount() const;

	TokenLocationFile* getTokenLocationFile() const;
	const FilePath& getFilePath() const;

	unsigned int getLineNumber() const;

	TokenLocation* addStartTokenLocation(Id locationId, Id tokenId, unsigned int columnNumber);
	TokenLocation* addEndTokenLocation(TokenLocation* start, unsigned int columnNumber);
	void removeTokenLocation(TokenLocation* location);

	TokenLocation* getTokenLocationById(Id id) const;

	void forEachTokenLocation(std::function<void(TokenLocation*)> func) const;
	void forEachStartTokenLocation(std::function<void(TokenLocation*)> func) const;
	void forEachEndTokenLocation(std::function<void(TokenLocation*)> func) const;

	TokenLocation* addTokenLocationAsPlainCopy(const TokenLocation* location);

private:
	TokenLocationMapType m_locations;

	TokenLocationFile* const m_file;
	const unsigned int m_lineNumber;
};

std::ostream& operator<<(std::ostream& ostream, const TokenLocationLine& line);

#endif // TOKEN_LOCATION_LINE_H
