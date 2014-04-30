#ifndef TEXT_LOCATION_LINE_H
#define TEXT_LOCATION_LINE_H

#include <memory>
#include <vector>

#include "data/TextLocation.h"

class TextLocationFile;

class TextLocationLine
{
public:
	TextLocationLine(std::weak_ptr<TextLocationFile> textLocationFile, unsigned int lineNumber);

	~TextLocationLine();

	unsigned int getLineNumber() const;

private:
	const std::weak_ptr<TextLocationFile> m_textLocationFile;
	const unsigned int m_lineNumber;

	const std::vector<std::shared_ptr<TextLocation> > m_textLocations;
};


#endif // TEXT_LOCATION_LINE_H
