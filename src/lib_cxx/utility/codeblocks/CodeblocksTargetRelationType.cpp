#include "utility/codeblocks/CodeblocksTargetRelationType.h"

namespace Codeblocks
{
	int targetRelationTypeToInt(TargetRelationType v)
	{
		switch (v)
		{
		case CODEBLOCKS_TARGET_RELATION_IGNORE_TARGET:
			return 0;
		case CODEBLOCKS_TARGET_RELATION_IGNORE_PROJECT:
			return 1;
		case CODEBLOCKS_TARGET_RELATION_TARGET_BEFORE_PROJECT:
			return 2;
		case CODEBLOCKS_TARGET_RELATION_TARGET_AFTER_PROJECT:
			return 3;
		}
		return 99;
	}

	TargetRelationType intToTargetRelationType(int v)
	{
		if (v == targetRelationTypeToInt(CODEBLOCKS_TARGET_RELATION_IGNORE_TARGET))
		{
			return CODEBLOCKS_TARGET_RELATION_IGNORE_TARGET;
		}
		else if (v == targetRelationTypeToInt(CODEBLOCKS_TARGET_RELATION_IGNORE_PROJECT))
		{
			return CODEBLOCKS_TARGET_RELATION_IGNORE_PROJECT;
		}
		else if (v == targetRelationTypeToInt(CODEBLOCKS_TARGET_RELATION_TARGET_BEFORE_PROJECT))
		{
			return CODEBLOCKS_TARGET_RELATION_TARGET_BEFORE_PROJECT;
		}
		else if (v == targetRelationTypeToInt(CODEBLOCKS_TARGET_RELATION_TARGET_AFTER_PROJECT))
		{
			return CODEBLOCKS_TARGET_RELATION_TARGET_AFTER_PROJECT;
		}
		return CODEBLOCKS_TARGET_RELATION_UNKNOWN;
	}
}
