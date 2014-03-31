#ifndef TEXT_LOCATION_H
#define TEXT_LOCATION_H

#include <memory>

class Element;
class TextLocationLine;

class TextLocation
{
public:
	TextLocation(
		const std::weak_ptr<TextLocationLine>& textLocationLine,
		const std::weak_ptr<Element>& element,
		unsigned int column);

	~TextLocation();

private:
	const unsigned int m_column;
	const std::weak_ptr<TextLocationLine> m_textLocationLine;
	const std::weak_ptr<Element> m_element;
};


#endif // TEXT_LOCATION_H
