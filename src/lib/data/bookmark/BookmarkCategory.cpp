#include "BookmarkCategory.h"

BookmarkCategory::BookmarkCategory()
	: m_id(-1)
	, m_name("")
{
}

BookmarkCategory::~BookmarkCategory()
{
}

Id BookmarkCategory::getId() const
{
	return m_id;
}

void BookmarkCategory::setId(const Id id)
{
	m_id = id;
}

std::string BookmarkCategory::getName() const
{
	return m_name;
}

void BookmarkCategory::setName(const std::string& name)
{
	m_name = name;
}