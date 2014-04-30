#ifndef TEXT_LOCATION_H
#define TEXT_LOCATION_H

#include <memory>

class Element;
class TextLocationLine;

class TextLocation
{
public:
	TextLocation(
		std::weak_ptr<TextLocationLine> textLocationLine,
		std::weak_ptr<Element> element,
		unsigned int column
	);

	~TextLocation();

	unsigned int getColumn() const;

private:
	const std::weak_ptr<Element> m_element;
	const std::weak_ptr<TextLocationLine> m_textLocationLine;
	const unsigned int m_column;
};

#endif // TEXT_LOCATION_H
