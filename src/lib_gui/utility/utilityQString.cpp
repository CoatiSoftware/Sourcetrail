#include "utility/utilityQString.h"

#include <QString>

std::wstring utility::decodeFromUtf8(std::string s)
{
	QString qs = QString::fromUtf8(s.c_str());
	return qs.toStdWString();
}

std::string utility::encodeToUtf8(std::wstring s)
{
	QString qs = QString::fromStdWString(s);
	return qs.toUtf8().toStdString();
}