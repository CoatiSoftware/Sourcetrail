#ifndef NAME_ELEMENT_H
#define NAME_ELEMENT_H

#include <string>

class NameElement
{
public:
	NameElement(const std::string& name);
	NameElement(const std::string& name, const std::string& signature);
	~NameElement();

	std::string getFullName() const;
	std::string getFullSignature() const;

private:
	std::string m_name;
	std::string m_signature;
};

#endif // NAME_ELEMENT_H
