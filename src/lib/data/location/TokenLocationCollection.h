#ifndef TOKEN_LOCATION_COLLECTION_H
#define TOKEN_LOCATION_COLLECTION_H

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "utility/types.h"

class TokenLocation;
class TokenLocationFile;
class TokenLocationLine;

class TokenLocationCollection
{
public:
	typedef std::map<std::string, std::shared_ptr<TokenLocationFile> > TokenLocationFileMapType;
	typedef std::pair<std::string, std::shared_ptr<TokenLocationFile> > TokenLocationFilePairType;

	TokenLocationCollection();
	~TokenLocationCollection();

	const TokenLocationFileMapType& getTokenLocationFiles() const;
	size_t getTokenLocationFileCount() const;

	const std::map<Id, TokenLocation*>& getTokenLocations() const;
	size_t getTokenLocationCount() const;

	TokenLocation* addTokenLocation(
		Id tokenId, const std::string& filePath,
		unsigned int startLineNumber, unsigned int startColumnNumber,
		unsigned int endLineNumber, unsigned int endColumnNumber);
	void removeTokenLocation(TokenLocation* location);

	TokenLocation* findTokenLocationById(Id id) const;
	TokenLocationFile* findTokenLocationFileByPath(const std::string& filePath) const;

	void forEachTokenLocationFile(std::function<void(TokenLocationFile*)> func) const;
	void forEachTokenLocationLine(std::function<void(TokenLocationLine*)> func) const;
	void forEachTokenLocation(std::function<void(TokenLocation*)> func) const;

	void removeTokenLocationFile(TokenLocationFile* file);

	TokenLocation* addTokenLocationAsPlainCopy(const TokenLocation* location);

	void clear();

private:
	TokenLocationFile* createTokenLocationFile(const std::string& filePath);

	TokenLocationFileMapType m_files;
	std::map<Id, TokenLocation*> m_locations;
};

std::ostream& operator<<(std::ostream& ostream, const TokenLocationCollection& base);

#endif // TOKEN_LOCATION_COLLECTION_H
