#ifndef PATH_DETECTOR_BASE_H
#define PATH_DETECTOR_BASE_H

#include <string>
#include <vector>

class FilePath;

class PathDetector
{
public:
	PathDetector(const std::string& name);
	virtual ~PathDetector() = default;

	std::string getName() const;
	std::vector<FilePath> getPaths() const;
	bool isWorking() const;

protected:
	const std::string m_name;

private:
	virtual std::vector<FilePath> doGetPaths() const = 0;
};

#endif	  // PATH_DETECTOR_BASE_H
