#include "TextCodec.h"

#include <QTextCodec>

TextCodec::TextCodec(const std::string& name)
	: m_name(name)
{
}

std::string TextCodec::getName() const
{
	return m_name;
}

bool TextCodec::isValid() const
{
	QTextCodec* codec = QTextCodec::codecForName(m_name.c_str());
	if (codec)
	{
		return true;
	}
	return false;
}

std::wstring TextCodec::decode(const std::string& unicodeString) const
{
	QTextCodec* codec = QTextCodec::codecForName(m_name.c_str());
	if (codec)
	{
		QTextDecoder decoder(codec);
		return decoder.toUnicode(unicodeString.c_str()).toStdWString();
	}
	return QString::fromStdString(unicodeString).toStdWString();
}

std::string TextCodec::encode(const std::wstring& string) const
{
	QTextCodec* codec = QTextCodec::codecForName(m_name.c_str());
	if (codec)
	{
		QTextEncoder encoder(codec);
		return encoder.fromUnicode(QString::fromStdWString(string)).toStdString();
	}

	return QString::fromStdWString(string).toStdString();
}
