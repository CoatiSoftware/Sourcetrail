#ifndef TEXT_CODEC_H
#define TEXT_CODEC_H

#include <string>
#include <memory>

class QTextCodec;
class QTextDecoder;
class QTextEncoder;

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
	QTextCodec* m_codec;
	std::shared_ptr<QTextDecoder> m_decoder;
	std::shared_ptr<QTextEncoder> m_encoder;
};

#endif // TEXT_CODEC_H
