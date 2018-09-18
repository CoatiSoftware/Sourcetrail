#include "MessageStatus.h"

#include "utilityString.h"

MessageStatus::MessageStatus(const std::wstring& status, bool isError, bool showLoader, bool showInStatusBar)
	: isError(isError)
	, showLoader(showLoader)
	, showInStatusBar(showInStatusBar)
{
	m_stati.push_back(utility::replace(status, L"\n", L" "));

	setSendAsTask(false);
}

MessageStatus::MessageStatus(const std::vector<std::wstring>& stati, bool isError, bool showLoader, bool showInStatusBar)
	: isError(isError)
	, showLoader(showLoader)
	, showInStatusBar(showInStatusBar)
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

void MessageStatus::print(std::wostream& os) const
{
	for (const std::wstring& status : m_stati)
	{
		os << status;

		if (m_stati.size() > 1)
		{
			os << L" - ";
		}
	}

	if (isError)
	{
		os << L" - error";
	}

	if (showLoader)
	{
		os << L" - loading";
	}
}

