#ifndef STORAGE_TRANSFORMATION_ANONYMOUS_TYPEDEF_H
#define STORAGE_TRANSFORMATION_ANONYMOUS_TYPEDEF_H

#include <memory>

class IntermediateStorage;

class StorageTransformationAnonymousTypedef
{
public:
	static void transform(std::shared_ptr<IntermediateStorage> storage);
};

#endif // STORAGE_TRANSFORMATION_ANONYMOUS_TYPEDEF_H
