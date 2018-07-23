#include "qt/view/graphElements/QtGraphNodeText.h"

QtGraphNodeText::QtGraphNodeText(const std::wstring& name, int fontSizeDiff)
	: m_fontSizeDiff(fontSizeDiff)
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
	setStyle(GraphViewStyle::getStyleOfTextNode(m_fontSizeDiff));
}
