#ifndef NAME_ELEMENT_H
#define NAME_ELEMENT_H

#include <memory>
#include <string>
#include <vector>

class DataType;

class NameElement
{
public:
	class Signature
	{
	public:
		static std::string serialize(Signature signature);
		static Signature deserialize(const std::string& serialized);

		Signature();
		Signature(std::string prefix, std::string postfix);
		std::string qualifyName(const std::string& name) const;
		bool isValid() const;

		const std::string& getPrefix() const;
		const std::string& getPostfix() const;

	private:
		std::string m_prefix;
		std::string m_postfix;
	};

	NameElement(const std::string& name);
	NameElement(const std::string& name, const Signature& signature);
	~NameElement();

	std::string getName() const;
	std::string getNameWithSignature() const;
	bool hasSignature() const;
	Signature getSignature();

private:
	std::string m_name;
	Signature m_signature;
};

#endif // NAME_ELEMENT_H
