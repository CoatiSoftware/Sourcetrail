#include "utility/messaging/type/MessageStatus.h"

#include "utility/utilityQString.h"
#include "utility/utilityString.h"

MessageStatus::MessageStatus(const std::string& status, bool isError, bool showLoader)
	: isError(isError)
	, showLoader(showLoader)
{
	m_stati.push_back(utility::decodeFromUtf8(utility::replace(status, "\n", " ")));

	setSendAsTask(false);
}

MessageStatus::MessageStatus(const std::wstring& status, bool isError, bool showLoader)
	: isError(isError)
	, showLoader(showLoader)
{
	m_stati.push_back(utility::replace(status, L"\n", L" "));

	setSendAsTask(false);
}

MessageStatus::MessageStatus(const std::vector<std::wstring>& stati, bool isError, bool showLoader)
	: isError(isError)
	, showLoader(showLoader)
	, m_stati(stati)
{
	setSendAsTask(false);
}

const std::string MessageStatus::getStaticType()
{
	return "MessageStatus";
}

const std::vector<std::wstring>& MessageStatus::stati() const
{
	return m_stati;
}

std::wstring MessageStatus::status() const
{
	if (m_stati.size())
	{
		return m_stati[0];
	}

	return L"";
}

void MessageStatus::print(std::ostream& os) const
{
	for (const std::wstring& status : m_stati)
	{
		os << utility::encodeToUtf8(status);

		if (m_stati.size() > 1)
		{
			os << " - ";
		}
	}

	if (isError)
	{
		os << " - error";
	}

	if (showLoader)
	{
		os << " - loading";
	}
}

