#ifndef MESSAGE_STATUS_H
#define MESSAGE_STATUS_H

#include <string>
#include <vector>

#include "Message.h"

class MessageStatus
	: public Message<MessageStatus>
{
public:
	MessageStatus(const std::wstring& status, bool isError = false, bool showLoader = false, bool showInStatusBar = true);
	MessageStatus(const std::vector<std::wstring>& stati, bool isError = false, bool showLoader = false, bool showInStatusBar = true);

	static const std::string getStaticType();

	const std::vector<std::wstring>& stati() const;
	std::wstring status() const;
	virtual void print(std::wostream& os) const;

	const bool isError;
	const bool showLoader;
	const bool showInStatusBar;

private:
	std::vector<std::wstring> m_stati;
};

#endif // MESSAGE_STATUS_H
