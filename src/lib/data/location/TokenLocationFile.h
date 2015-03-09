#ifndef TOKEN_LOCATION_FILE_H
#define TOKEN_LOCATION_FILE_H

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "utility/file/FilePath.h"
#include "utility/types.h"

class TokenLocation;
class TokenLocationLine;

class TokenLocationFile
{
public:
	typedef std::map<unsigned int, std::shared_ptr<TokenLocationLine>> TokenLocationLineMapType;
	typedef std::pair<unsigned int, std::shared_ptr<TokenLocationLine>> TokenLocationLinePairType;

	TokenLocationFile(const FilePath& filePath);
	~TokenLocationFile();

	const TokenLocationLineMapType& getTokenLocationLines() const;
	size_t getTokenLocationLineCount() const;

	const FilePath& getFilePath() const;

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
	FilePath m_filePath;
};

std::ostream& operator<<(std::ostream& ostream, const TokenLocationFile& file);

#endif // TOKEN_LOCATION_FILE_H
