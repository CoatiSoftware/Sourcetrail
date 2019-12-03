#ifndef REFRESH_INFO_H
#define REFRESH_INFO_H

#include <set>

#include "FilePath.h"

enum RefreshMode
{
	REFRESH_NONE,
	REFRESH_UPDATED_FILES,
	REFRESH_UPDATED_AND_INCOMPLETE_FILES,
	REFRESH_ALL_FILES
};

struct RefreshInfo
{
	std::set<FilePath> filesToIndex;
	std::set<FilePath> filesToClear;
	std::set<FilePath> nonIndexedFilesToClear;

	RefreshMode mode = REFRESH_NONE;
	bool shallow = false;
};

#endif	  // REFRESH_INFO_H
