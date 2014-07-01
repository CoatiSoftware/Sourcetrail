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
	TokenLocation(Id tokenId, TokenLocationLine* line, unsigned int columnNumber, bool isStart);
	TokenLocation(Id id, Id tokenId, TokenLocationLine* line, unsigned int columnNumber, bool isStart);
	~TokenLocation();

	Id getId() const;
	Id getTokenId() const;

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

	std::shared_ptr<TokenLocation> createPlainCopy(TokenLocationLine* line) const;

private:
	static Id s_locationId;

	const Id m_id;
	const Id m_tokenId;

	TokenLocationLine* const m_line;
	const unsigned int m_columnNumber;

	TokenLocation* m_other;
	const bool m_isStart;
};

std::ostream& operator<<(std::ostream& ostream, const TokenLocation& location);

#endif // TOKEN_LOCATION_H
