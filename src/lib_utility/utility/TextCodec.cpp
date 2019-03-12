#include "TextCodec.h"

#include <QTextCodec>

TextCodec::TextCodec(const std::string& name)
	: m_name(name)
{
	m_codec = QTextCodec::codecForName(m_name.c_str());
	m_decoder = std::make_shared<QTextDecoder>(m_codec);
	m_encoder = std::make_shared<QTextEncoder>(m_codec);
}

std::string TextCodec::getName() const
{
	return m_name;
}

bool TextCodec::isValid() const
{
	if (m_codec)
	{
		return true;
	}
	return false;
}

std::wstring TextCodec::decode(const std::string& unicodeString) const
{
	if (m_decoder)
	{
		return m_decoder->toUnicode(unicodeString.c_str()).toStdWString();
	}
	return QString::fromStdString(unicodeString).toStdWString();
}

std::string TextCodec::encode(const std::wstring& string) const
{
	if (m_encoder)
	{
		return m_encoder->fromUnicode(QString::fromStdWString(string)).toStdString();
	}
	return QString::fromStdWString(string).toStdString();
}
