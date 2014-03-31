#ifndef TEXT_LOCATION_STORAGE_H
#define TEXT_LOCATION_STORAGE_H

#include <vector>

#include "data/TextLocationFile.h"

class TextLocationStorage
{
public:
	TextLocationStorage();
	~TextLocationStorage();

private:
	std::vector<std::shared_ptr<TextLocationFile>> m_textLocationFiles;
};


#endif // TEXT_LOCATION_STORAGE_H
