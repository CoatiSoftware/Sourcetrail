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
	bool isWorking() const;
	virtual std::vector<FilePath> getPaths() const = 0;

protected:
	const std::string m_name;
};

#endif // PATH_DETECTOR_BASE_H
