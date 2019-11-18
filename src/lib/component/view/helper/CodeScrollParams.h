#ifndef CODE_SCROLL_PARAMS_H
#define CODE_SCROLL_PARAMS_H

#include "FilePath.h"
#include "types.h"

struct CodeScrollParams
{
	enum class Type
	{
		NONE,
		TO_REFERENCE,
		TO_FILE,
		TO_LINE,
		TO_VALUE
	};

	enum class Target
	{
		VISIBLE,
		CENTER,
		TOP
	};

	static CodeScrollParams toReference(const FilePath& filePath, Id locationId, Target target)
	{
		return CodeScrollParams(Type::TO_REFERENCE, target, filePath, locationId, 0, 0, false);
	}

	static CodeScrollParams toFile(const FilePath& filePath, Target target)
	{
		return CodeScrollParams(Type::TO_LINE, target, filePath, 0, 0, 0, false);
	}

	static CodeScrollParams toLine(const FilePath& filePath, size_t line, Target target)
	{
		return CodeScrollParams(Type::TO_LINE, target, filePath, 0, line, 0, false);
	}

	static CodeScrollParams toValue(size_t value, bool inListMode)
	{
		return CodeScrollParams(Type::TO_VALUE, Target::VISIBLE, FilePath(), 0, 0, value, inListMode);
	}

	CodeScrollParams(
		Type type, Target target, FilePath filePath, Id locationId, size_t line, size_t value, bool inListMode)
		: type(type)
		, target(target)
		, filePath(filePath)
		, locationId(locationId)
		, line(line)
		, value(value)
		, inListMode(inListMode)
	{
	}

	CodeScrollParams()
		: type(Type::NONE), target(Target::VISIBLE), locationId(0), line(0), value(0), inListMode(false)
	{
	}


	Type type;
	Target target;

	FilePath filePath;

	// Reference
	Id locationId;

	// Line
	size_t line;

	// Value
	size_t value;
	bool inListMode = false;
};

#endif	  // CODE_SCROLL_PARAMS_H
