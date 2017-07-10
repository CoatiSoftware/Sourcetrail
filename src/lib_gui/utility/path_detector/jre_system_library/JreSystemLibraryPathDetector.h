#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H

#include <memory>

#include "utility/path_detector/PathDetector.h"

class JavaPathDetector;

class JreSystemLibraryPathDetector: public PathDetector
{
public:
	JreSystemLibraryPathDetector(std::shared_ptr<JavaPathDetector> javaPathDetector);
	virtual ~JreSystemLibraryPathDetector();

	virtual std::vector<FilePath> getPaths() const;

private:
	std::shared_ptr<JavaPathDetector> m_javaPathDetector;
};

#endif // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_H
