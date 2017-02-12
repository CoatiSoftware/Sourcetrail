#include "Bookmark.h"

Bookmark::Bookmark()
	: m_id(-1)
	, m_tokenTypes()
	, m_tokenIds()
	, m_tokenNames()
	, m_comment("")
	, m_displayName("")
	, m_valid(false)
	, m_timeStamp()
	, m_category()
{
}

Bookmark::Bookmark(const std::string& displayName, const std::vector<Id>& tokens, const std::vector<std::string>& tokenNames, const std::string& comment, const TimePoint& timeStamp)
	: m_id(-1)
	, m_tokenTypes()
	, m_tokenIds(tokens)
	, m_tokenNames(tokenNames)
	, m_comment(comment)
	, m_displayName(displayName)
	, m_valid(false)
	, m_timeStamp(timeStamp)
	, m_category()
{
}

Bookmark::~Bookmark()
{
}

Id Bookmark::getId() const
{
	return m_id;
}

void Bookmark::setId(const Id id)
{
	m_id = id;
}

std::vector<int> Bookmark::getTokenTypes() const
{
	return m_tokenTypes;
}

void Bookmark::setTokenTypes(const std::vector<int>& types)
{
	m_tokenTypes = types;
}

std::vector<Id> Bookmark::getTokenIds() const
{
	return m_tokenIds;
}

void Bookmark::setTokenIds(const std::vector<Id>& ids)
{
	m_tokenIds = ids;
}

std::vector<std::string> Bookmark::getTokenNames() const
{
	return m_tokenNames;
}

void Bookmark::setTokenNames(const std::vector<std::string>& names)
{
	m_tokenNames = names;
}

std::string Bookmark::getComment() const
{
	return m_comment;
}

void Bookmark::setComment(const std::string& comment)
{
	m_comment = comment;
}

std::string Bookmark::getDisplayName() const
{
	return m_displayName;
}

void Bookmark::setDisplayName(const std::string& name)
{
	m_displayName = name;
}

bool Bookmark::isValid() const
{
	return m_valid;
}

void Bookmark::setValid(const bool valid)
{
	m_valid = valid;
}

TimePoint Bookmark::getTimeStamp() const
{
	return m_timeStamp;
}

BookmarkCategory Bookmark::getCategory() const
{
	return m_category;
}

void Bookmark::setCategory(const BookmarkCategory& category)
{
	m_category = category;
}