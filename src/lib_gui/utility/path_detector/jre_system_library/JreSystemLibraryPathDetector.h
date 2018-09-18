#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H

#include <memory>

#include "PathDetector.h"

class JavaPathDetector;

class JreSystemLibraryPathDetector: public PathDetector
{
public:
	JreSystemLibraryPathDetector(std::shared_ptr<JavaPathDetector> javaPathDetector);
	std::vector<FilePath> getPaths() const override;

private:
	std::shared_ptr<JavaPathDetector> m_javaPathDetector;
};

#endif // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H
