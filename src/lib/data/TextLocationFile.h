#ifndef TEXT_LOCATION_FILE_H
#define TEXT_LOCATION_FILE_H

#include <memory>
#include <string>
#include <vector>

#include "data/TextLocationLine.h"

class TextLocationFile
{
public:
	TextLocationFile(const std::string& filePath);
	~TextLocationFile();

private:
	const std::string m_filePath;
	std::vector<std::shared_ptr<TextLocationLine> > m_textLocationLine;
};


#endif // TEXT_LOCATION_FILE_H
