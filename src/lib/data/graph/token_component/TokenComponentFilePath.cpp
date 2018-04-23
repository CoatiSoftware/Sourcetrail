#include "data/graph/token_component/TokenComponentFilePath.h"

TokenComponentFilePath::TokenComponentFilePath(const FilePath& path, bool complete)
	: m_path(path)
	, m_complete(complete)
{
}

TokenComponentFilePath::~TokenComponentFilePath()
{
}

std::shared_ptr<TokenComponent> TokenComponentFilePath::copy() const
{
	return std::make_shared<TokenComponentFilePath>(*this);
}

const FilePath& TokenComponentFilePath::getFilePath() const
{
	return m_path;
}

bool TokenComponentFilePath::isComplete() const
{
	return m_complete;
}
