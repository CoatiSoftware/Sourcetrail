#include "TextAccess.h"

#include <fstream>

#include "utility/logging/logging.h"

std::shared_ptr<TextAccess> TextAccess::createFromFile(const FilePath& filePath)
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

FilePath TextAccess::getFilePath() const
{
	return m_filePath;
}

std::string TextAccess::getLine(const unsigned int lineNumber) const
{
	if (!checkIndexInRange(lineNumber))
	{
		return "";
	}

	return m_lines[lineNumber - 1]; // -1 to correct for use as index
}

std::vector<std::string> TextAccess::getLines(const unsigned int firstLineNumber, const unsigned int lastLineNumber)
{
	if (!checkIndexIntervalInRange(firstLineNumber, lastLineNumber))
	{
		return std::vector<std::string>();
	}

	std::vector<std::string>::iterator first = m_lines.begin() + firstLineNumber - 1; // -1 to correct for use as index
	std::vector<std::string>::iterator last = m_lines.begin() + lastLineNumber;
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

std::vector<std::string> TextAccess::readFile(const FilePath& filePath)
{
	std::vector<std::string> result;

	std::ifstream srcFile;
	srcFile.open(filePath.str());

	if (srcFile.fail())
	{
		LOG_ERROR("Could not open file " + filePath.str());
		return result;
	}

	while (!srcFile.eof())
	{
		std::string line;
		std::getline(srcFile, line);
		result.push_back(line + '\n');
	}
	srcFile.close();

	if (!result.empty())
	{
		std::string last = result.back().substr(0, result.back().size() - 1);
		result.pop_back();
		if (!last.empty())
		{
			result.push_back(last);
		}
	}

	return result;
}

std::vector<std::string> TextAccess::splitStringByLines(const std::string& text)
{
	std::vector<std::string> result;
	size_t prevIndex = 0;
	size_t index = text.find('\n');

	while (index != std::string::npos)
	{
		result.push_back(text.substr(prevIndex, index - prevIndex) + '\n');

		prevIndex = index + 1;
		index = text.find('\n', prevIndex);
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
	if (index < 1)
	{
		LOG_WARNING_STREAM(<< "Line numbers start with one, is " << index);
		return false;
	}
	else if (index > m_lines.size())
	{
		LOG_WARNING_STREAM(<< "Tried to access index " << index << ". Maximum index is " << m_lines.size());
		return false;
	}

	return true;
}

bool TextAccess::checkIndexIntervalInRange(
	const unsigned int firstIndex,
	const unsigned int lastIndex
) const
{
	if (!checkIndexInRange(firstIndex) || !checkIndexInRange(lastIndex))
	{
		return false;
	}
	else if (firstIndex > lastIndex)
	{
		LOG_WARNING_STREAM(
			<< "Index 'firstLine' has to be lower or equal index 'lastLine', is " << firstIndex << " > " << lastIndex
		);
		return false;
	}

	return true;
}
