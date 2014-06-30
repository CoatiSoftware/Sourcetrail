#ifndef TEXT_LOCATION_H
#define TEXT_LOCATION_H

#include <memory>

class Token;
class TextLocationLine;

class TextLocation
{
public:
	TextLocation(
		std::weak_ptr<TextLocationLine> textLocationLine,
		std::weak_ptr<Token> token,
		unsigned int column
	);

	~TextLocation();

	unsigned int getColumn() const;

private:
	const std::weak_ptr<Token> m_token;
	const std::weak_ptr<TextLocationLine> m_textLocationLine;
	const unsigned int m_column;
};

#endif // TEXT_LOCATION_H
