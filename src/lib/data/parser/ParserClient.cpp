#include "data/parser/ParserClient.h"

#include <sstream>

#include "data/parser/ParseLocation.h"

std::string ParserClient::addAccessPrefix(const std::string& str, AccessKind access)
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return "public " + str;
	case ACCESS_PROTECTED:
		return "protected " + str;
	case ACCESS_PRIVATE:
		return "private " + str;
	case ACCESS_DEFAULT:
		return "default " + str;
	default:
		break;
	}
	return str;
}

std::string ParserClient::addStaticPrefix(const std::string& str, bool isStatic)
{
	if (isStatic)
	{
		return "static " + str;
	}
	return str;
}

std::string ParserClient::addConstPrefix(const std::string& str, bool isConst, bool atFront)
{
	if (isConst)
	{
		return atFront ? "const " + str : str + " const";
	}
	return str;
}

std::string ParserClient::addLocationSuffix(const std::string& str, const ParseLocation& location)
{
	std::stringstream ss;
	ss << str;
	ss << " <" << location.startLineNumber << ":" << location.startColumnNumber << " ";
	ss << location.endLineNumber << ":" << location.endColumnNumber << ">";
	return ss.str();
}

std::string ParserClient::addLocationSuffix(
	const std::string& str, const ParseLocation& location, const ParseLocation& scopeLocation
){
	if (!location.isValid())
	{
		return addLocationSuffix(str, scopeLocation);
	}
	else if (!scopeLocation.isValid())
	{
		return addLocationSuffix(str, location);
	}

	std::stringstream ss;
	ss << str;
	ss << " <" << scopeLocation.startLineNumber << ":" << scopeLocation.startColumnNumber;
	ss << " <" << location.startLineNumber << ":" << location.startColumnNumber << " ";
	ss << location.endLineNumber << ":" << location.endColumnNumber << "> ";
	ss << scopeLocation.endLineNumber << ":" << scopeLocation.endColumnNumber << ">";
	return ss.str();
}

ParserClient::ParserClient()
	: m_hasFatalErrors(false)
{
}

ParserClient::~ParserClient()
{
}

void ParserClient::onErrorParsed(const ParseLocation& location, const std::string& message, bool fatal, bool indexed)
{
	this->onError(location, message, fatal, indexed);

	if (fatal)
	{
		m_hasFatalErrors = true;
	}
}

bool ParserClient::hasFatalErrors() const
{
	return m_hasFatalErrors;
}
