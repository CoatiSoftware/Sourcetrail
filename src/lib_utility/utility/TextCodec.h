#ifndef TEXT_CODEC_H
#define TEXT_CODEC_H

#include <string>

class TextCodec
{
public:
	TextCodec(const std::string& name);

	std::string getName() const;
	bool isValid() const;

	std::wstring decode(const std::string& unicodeString) const;

	std::string encode(const std::wstring& string) const;

private:
	const std::string m_name;
};

#endif // TEXT_CODEC_H
