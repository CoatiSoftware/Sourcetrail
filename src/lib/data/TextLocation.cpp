#include "data/TextLocation.h"

#include "data/graph/Token.h"
#include "data/TextLocationLine.h"

TextLocation::TextLocation(
	std::weak_ptr<TextLocationLine> textLocationLine,
	std::weak_ptr<Token> token,
	unsigned int column
)
	: m_token(token)
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
