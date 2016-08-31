#ifndef JAVA_PATH_DETECTOR_LINUX_H
#define JAVA_PATH_DETECTOR_LINUX_H

#include "utility/path_detector/java_runtime/JavaPathDetector.h"

class JavaPathDetectorLinux
	: public JavaPathDetector
{
public:
	JavaPathDetectorLinux(const std::string javaVersion);
	virtual ~JavaPathDetectorLinux();

	virtual std::vector<FilePath> getPaths() const;

private:
	FilePath getJavaInPath() const;
	FilePath readLink(const FilePath& path) const;
	FilePath getJavaInJavaHome() const;
	bool checkVersion(const FilePath& path) const;

	FilePath getFilePathRelativeToJavaExecutable(FilePath& javaExecutablePath) const;
};

#endif // JAVA_PATH_DETECTOR_LINUX_H
