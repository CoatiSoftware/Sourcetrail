#ifndef JAVA_PATH_DETECTOR_LINUX_H
#define JAVA_PATH_DETECTOR_LINUX_H

#include "JavaPathDetector.h"

class JavaPathDetectorLinux: public JavaPathDetector
{
public:
	JavaPathDetectorLinux(const std::string javaVersion);

private:
	std::vector<FilePath> doGetPaths() const override;
	FilePath getJavaInPath() const;
	FilePath readLink(const FilePath& path) const;
	FilePath getJavaInJavaHome() const;
	bool checkVersion(const FilePath& path) const;

	FilePath getFilePathRelativeToJavaExecutable(FilePath& javaExecutablePath) const;
};

#endif	  // JAVA_PATH_DETECTOR_LINUX_H
