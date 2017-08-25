#ifndef MESSAGE_STATUS_H
#define MESSAGE_STATUS_H

#include <vector>

#include "utility/messaging/Message.h"
#include "utility/utilityString.h"

class MessageStatus
	: public Message<MessageStatus>
{
public:
	MessageStatus(const std::string& status, bool isError = false, bool showLoader = false)
		: isError(isError)
		, showLoader(showLoader)
	{
		m_stati.push_back(utility::replace(status, "\n", " "));

		setSendAsTask(false);
	}

	MessageStatus(const std::vector<std::string>& stati, bool isError = false, bool showLoader = false)
		: isError(isError)
		, showLoader(showLoader)
		, m_stati(stati)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageStatus";
	}

	const std::vector<std::string>& stati() const
	{
		return m_stati;
	}

	std::string status() const
	{
		if (m_stati.size())
		{
			return m_stati[0];
		}

		return "";
	}

	virtual void print(std::ostream& os) const
	{
		for (const std::string& status : m_stati)
		{
			os << status;

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

	const bool isError;
	const bool showLoader;

private:
	std::vector<std::string> m_stati;
};

#endif // MESSAGE_STATUS_H
