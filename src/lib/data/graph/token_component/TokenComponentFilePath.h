#ifndef TOKEN_COMPONENT_FILE_PATH_H
#define TOKEN_COMPONENT_FILE_PATH_H

#include "FilePath.h"

#include "TokenComponent.h"

class TokenComponentFilePath: public TokenComponent
{
public:
	TokenComponentFilePath(const FilePath& path, bool complete);
	virtual ~TokenComponentFilePath();

	virtual std::shared_ptr<TokenComponent> copy() const;

	const FilePath& getFilePath() const;
	bool isComplete() const;

private:
	const FilePath m_path;
	const bool m_complete;
};

#endif	  // TOKEN_COMPONENT_FILE_PATH_H
