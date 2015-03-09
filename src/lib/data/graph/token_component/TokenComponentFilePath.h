#ifndef TOKEN_COMPONENT_FILE_PATH_H
#define TOKEN_COMPONENT_FILE_PATH_H

#include "utility/file/FilePath.h"

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentFilePath
	: public TokenComponent
{
public:
	TokenComponentFilePath(const FilePath& path);
	virtual ~TokenComponentFilePath();

	virtual std::shared_ptr<TokenComponent> copy() const;

	const FilePath& getFilePath() const;

private:
	const FilePath m_path;
};

#endif // TOKEN_COMPONENT_FILE_PATH_H
