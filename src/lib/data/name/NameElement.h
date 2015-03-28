#ifndef NAME_ELEMENT_H
#define NAME_ELEMENT_H

#include <string>

class NameElement
{
public:
	NameElement(std::string name);
	~NameElement();

	std::string getFullName() const;

private:
	std::string m_name;
};

#endif // NAME_ELEMENT_H
