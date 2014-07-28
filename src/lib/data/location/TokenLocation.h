#ifndef TOKEN_LOCATION_H
#define TOKEN_LOCATION_H

#include <memory>
#include <ostream>
#include <string>

#include "utility/types.h"

class Token;
class TokenLocationFile;
class TokenLocationLine;

class TokenLocation
{
public:
	enum LocationType
	{
		LOCATION_TOKEN,
		LOCATION_SCOPE
	};

	TokenLocation(Id tokenId, TokenLocationLine* line, unsigned int columnNumber, bool isStart);
	TokenLocation(TokenLocation* other, TokenLocationLine* line, unsigned int columnNumber, bool isStart);
	TokenLocation(const TokenLocation& other, TokenLocationLine* line);
	~TokenLocation();

	Id getId() const;
	Id getTokenId() const;

	LocationType getType() const;
	void setType(LocationType type);

	TokenLocationLine* getTokenLocationLine() const;
	TokenLocationFile* getTokenLocationFile() const;

	unsigned int getColumnNumber() const;
	unsigned int getLineNumber() const;
	const std::string& getFilePath() const;

	TokenLocation* getOtherTokenLocation() const;
	void setOtherTokenLocation(TokenLocation* location);

	TokenLocation* getStartTokenLocation();
	TokenLocation* getEndTokenLocation();

	bool isStartTokenLocation() const;
	bool isEndTokenLocation() const;

private:
	static Id s_locationId;	// next free own id

	const Id m_id;			// own id
	const Id m_tokenId;

	LocationType m_type;

	TokenLocationLine* const m_line;
	const unsigned int m_columnNumber;

	TokenLocation* m_other;
	const bool m_isStart;
};

std::ostream& operator<<(std::ostream& ostream, const TokenLocation& location);

#endif // TOKEN_LOCATION_H
