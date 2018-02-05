#include "qt/view/graphElements/QtGraphNodeText.h"

QtGraphNodeText::QtGraphNodeText(const std::wstring& name)
{
	setName(name);
}

QtGraphNodeText::~QtGraphNodeText()
{
}

bool QtGraphNodeText::isTextNode() const
{
	return true;
}

void QtGraphNodeText::updateStyle()
{
	setStyle(GraphViewStyle::getStyleOfTextNode());
}
