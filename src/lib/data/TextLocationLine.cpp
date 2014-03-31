#include "data/TextLocationLine.h"

#include "data/TextLocationFile.h"

TextLocationLine::TextLocationLine(const std::weak_ptr<TextLocationFile>& textLocationFile, unsigned int lineNumber)
: m_textLocationFile(textLocationFile)
, m_lineNumber(lineNumber)
{
}

TextLocationLine::~TextLocationLine()
{
}
