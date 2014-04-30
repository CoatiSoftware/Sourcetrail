#include "data/TextLocation.h"

#include "data/Element.h"
#include "data/TextLocationLine.h"

TextLocation::TextLocation(
	std::weak_ptr<TextLocationLine> textLocationLine,
	std::weak_ptr<Element> element,
	unsigned int column
)
	: m_element(element)
	, m_textLocationLine(textLocationLine)
	, m_column(column)
{
}

TextLocation::~TextLocation()
{
}

unsigned int TextLocation::getColumn() const
{
	return m_column;
}
