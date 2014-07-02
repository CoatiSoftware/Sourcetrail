#include "TextAccess.h"

#include <fstream>
#include <sstream>

#include "utility/logging/logging.h"

std::shared_ptr<TextAccess> TextAccess::createFromFile(const std::string& filePath)
{
	std::shared_ptr<TextAccess> result(new TextAccess());

	result->m_filePath = filePath;
	result->m_lines = readFile(filePath);

	return result;
}

std::shared_ptr<TextAccess> TextAccess::createFromString(const std::string& text)
{
	std::shared_ptr<TextAccess> result(new TextAccess());

	result->m_lines = splitStringByLines(text);

	return result;
}

TextAccess::~TextAccess()
{
}

unsigned int TextAccess::getLineCount() const
{
	return m_lines.size();
}

std::string TextAccess::getFilePath() const
{
	return m_filePath;
}

std::string TextAccess::getLine(const unsigned int lineNumber) const
{
	if (lineNumber < 1)
	{
		std::stringstream message;
		message << "Line numbers start with one, is " << lineNumber;
		LOG_WARNING(message.str());

		return "";
	}

	if (checkIndexInRange(lineNumber-1) == false)
	{
		return "";
	}

	return m_lines[lineNumber-1]; // -1 to correct for use as index
}

std::vector<std::string> TextAccess::getLines(const unsigned int firstLineNumber, const unsigned int lastLineNumber)
{
	if(firstLineNumber < 1)
	{
		std::stringstream message;
		message << "Line numbers start with one, is " << firstLineNumber;
		LOG_WARNING(message.str());

		return std::vector<std::string>();
	}

	if(lastLineNumber < 1)
	{
		std::stringstream message;
		message << "Line numbers start with one, is " << lastLineNumber;
		LOG_WARNING(message.str());

		return std::vector<std::string>();
	}

	if (checkIndexIntervalInRange(firstLineNumber-1, lastLineNumber-1) == false)
	{
		return std::vector<std::string>();
	}

	std::vector<std::string>::iterator first = m_lines.begin() + firstLineNumber-1; // -1 to correct for use as index
	std::vector<std::string>::iterator last = m_lines.begin() + lastLineNumber; // no correction needed because 'last' needs to point one behind the last included element
	return std::vector<std::string>(first, last);
}

std::vector<std::string> TextAccess::getAllLines() const
{
	return m_lines;
}

std::string TextAccess::getText() const
{
	std::string result = "";

	for (unsigned int i = 0; i < m_lines.size(); i++)
	{
		result += m_lines[i];
	}

	return result;
}

std::vector<std::string> TextAccess::readFile(const std::string& filePath)
{
	std::vector<std::string> result;

	std::ifstream srcFile;
	srcFile.open(filePath);

	if (srcFile.fail())
	{
		LOG_ERROR("Could not open file " + filePath);
		return result;
	}

	while (!srcFile.eof())
	{
		std::string line;
		std::getline(srcFile, line);
		result.push_back(line + "\n");
	}
	srcFile.close();

	return result;
}

std::vector<std::string> TextAccess::splitStringByLines(const std::string& text)
{
	std::vector<std::string> result;
	size_t prevIndex = 0;
	size_t index = text.find("\n");

	while (index != std::string::npos)
	{
		result.push_back(text.substr(prevIndex, index - prevIndex) + "\n");

		prevIndex = index + 1;
		index = text.find("\n", prevIndex);
	}

	if (prevIndex < text.length() - 1)
	{
		result.push_back(text.substr(prevIndex));
	}

	return result;
}

TextAccess::TextAccess()
	: m_filePath("")
{
}

bool TextAccess::checkIndexInRange(const unsigned int index) const
{
	if (index >= m_lines.size())
	{
		std::stringstream message;
		message << "Tried to access index " << index;
		message << ". Maximum index is " << m_lines.size() - 1;
		LOG_WARNING(message.str());

		return false;
	}

	return true;
}

bool TextAccess::checkIndexIntervalInRange(
	const unsigned int firstIndex,
	const unsigned int lastIndex
) const
{
	if (checkIndexInRange(firstIndex) == false)
	{
		return false;
	}

	if (checkIndexInRange(lastIndex) == false)
	{
		return false;
	}

	if (firstIndex > lastIndex)
	{
		std::stringstream message;
		message << "Index 'firstLine' has to be lower or equal index 'lastLine'";
		message << ", is " << firstIndex << " > " << lastIndex;
		LOG_WARNING(message.str());

		return false;
	}

	return true;
}
