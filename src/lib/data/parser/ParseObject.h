#ifndef PARSE_OBJECT_H
#define PARSE_OBJECT_H

struct ParseObject
{
	ParseObject(
		const std::string& name,
		const std::string& fileName,
		unsigned int lineNumber,
		unsigned int columnNumber
	)
		: name(name)
		, fileName(fileName)
		, lineNumber(lineNumber)
		, columnNumber(columnNumber)
	{
	}

	const std::string name;
	const std::string fileName;
	const unsigned int lineNumber;
	const unsigned int columnNumber;
};

#endif // PARSE_OBJECT_H
