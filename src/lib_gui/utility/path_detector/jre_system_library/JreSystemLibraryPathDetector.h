#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H

#include <memory>

#include "PathDetector.h"

class JavaPathDetector;

class JreSystemLibraryPathDetector: public PathDetector
{
public:
	JreSystemLibraryPathDetector(std::shared_ptr<JavaPathDetector> javaPathDetector);

private:
	std::vector<FilePath> doGetPaths() const override;

	std::shared_ptr<JavaPathDetector> m_javaPathDetector;
};

#endif	  // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H
