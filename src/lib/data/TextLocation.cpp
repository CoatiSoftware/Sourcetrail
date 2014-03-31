#include "data/TextLocation.h"

#include "data/Element.h"
#include "data/TextLocationLine.h"

TextLocation::TextLocation(
	const std::weak_ptr<TextLocationLine>& textLocationLine,
	const std::weak_ptr<Element>& element,
	unsigned int column)
: m_textLocationLine(textLocationLine)
, m_element(element)
, m_column(column)
{
}

TextLocation::~TextLocation()
{
}
