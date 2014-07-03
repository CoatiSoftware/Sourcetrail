#ifndef TOKEN_LOCATION_FILE_H
#define TOKEN_LOCATION_FILE_H

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "utility/types.h"

class TokenLocation;
class TokenLocationLine;

class TokenLocationFile
{
public:
	typedef std::map<unsigned int, std::shared_ptr<TokenLocationLine> > TokenLocationLineMapType;
	typedef std::pair<unsigned int, std::shared_ptr<TokenLocationLine> > TokenLocationLinePairType;

	TokenLocationFile(const std::string& filePath);
	~TokenLocationFile();

	const TokenLocationLineMapType& getTokenLocationLines() const;
	size_t getTokenLocationLineCount() const;

	const std::string& getFilePath() const;

	TokenLocation* addTokenLocation(
		Id tokenId,
		unsigned int startLineNumber, unsigned int startColumnNumber,
		unsigned int endLineNumber, unsigned int endColumnNumber);
	void removeTokenLocation(TokenLocation* location);

	TokenLocationLine* findTokenLocationLineByNumber(unsigned int lineNumber) const;

	void forEachTokenLocationLine(std::function<void(TokenLocationLine*)> func) const;
	void forEachTokenLocation(std::function<void(TokenLocation*)> func) const;

	TokenLocation* addTokenLocationAsPlainCopy(const TokenLocation* location);

private:
	TokenLocationLine* findTokenLocationLine(unsigned int lineNumber) const;
	TokenLocationLine* createTokenLocationLine(unsigned int lineNumber);

	std::map<unsigned int, std::shared_ptr<TokenLocationLine> > m_lines;
	const std::string m_filePath;
};

std::ostream& operator<<(std::ostream& ostream, const TokenLocationFile& file);

#endif // TOKEN_LOCATION_FILE_H
