#include "SolutionParserUtility.h"

#include "utility/logging/logging.h"

#include "utility/messaging/type/MessageStatus.h"

#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

TiXmlElement* SolutionParserUtility::getFirstTagByName(TiXmlElement* root, const std::string& tag)
{
	TiXmlElement* element = root;

	while (element)
	{
		if (element == NULL)
		{
		}

		std::string value = element->Value();

		if (value == tag) // "ClInclude") // || value == "ClCompile")
		{
			if (element->Parent() != NULL)
			{
				return element; // ->Parent()->ToElement();
			}
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			if (element == NULL)
			{
			}

			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				if (newElement == NULL)
				{
				}

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}

std::vector<TiXmlElement*> SolutionParserUtility::getAllTagsByName(TiXmlElement* root, const std::string& tag)
{
	std::vector<TiXmlElement*> nodes;

	TiXmlElement* element = root;

	while (element)
	{
		std::string value = element->Value();

		if (value == tag)
		{
			nodes.push_back(element);
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			if (element == NULL)
			{
			}

			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				if (newElement == NULL)
				{
				}

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				break;
			}
		}
	}

	return nodes;
}

TiXmlElement* SolutionParserUtility::getFirstTagByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute)
{
	TiXmlElement* element = root;

	while (element)
	{
		if (element == NULL)
		{
		}

		std::string value = element->Value();

		bool hasAttribute = false;
		if (element->Attribute(attribute.c_str()) != NULL)
		{
			hasAttribute = true;
		}

		if (value == tag && hasAttribute) // "ClInclude") // || value == "ClCompile")
		{
			if (element->Parent() != NULL)
			{
				return element; // ->Parent()->ToElement();
			}
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}

std::vector<TiXmlElement*> SolutionParserUtility::getAllTagsByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute)
{
	std::vector<TiXmlElement*> nodes;

	TiXmlElement* element = root;

	while (element)
	{
		std::string value = element->Value();

		bool hasAttribute = false;
		if (element->Attribute(attribute.c_str()) != NULL)
		{
			hasAttribute = true;
		}

		if (value == tag && hasAttribute)
		{
			nodes.push_back(element);
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				break;
			}
		}
	}

	return nodes;
}

bool SolutionParserUtility::checkValidFileExtension(const std::string& file, const std::vector<std::string>& validExtensions)
{
	for (unsigned int i = 0; i < validExtensions.size(); i++)
	{
		size_t pos = file.find(validExtensions[i]);

		if (pos != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

std::vector<std::string> SolutionParserUtility::resolveEnvironmentVariables(const std::vector<std::string>& paths)
{
	std::vector<std::string> resolvedPaths;

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		resolvedPaths.push_back(findAndResolveEnvironmentVariable(paths[i]));
	}

	return resolvedPaths;
}

std::string SolutionParserUtility::findAndResolveEnvironmentVariable(const std::string& path)
{
	std::string resolvedPath;

	size_t pos = path.find("$(");

	if (pos != std::string::npos)
	{
		size_t endPos = path.substr(pos).find(")");

		std::string envVariable = path.substr(pos + 2, (endPos - 2) - pos);
		std::string envPath = getenv(envVariable.c_str());

		std::string prePath = path.substr(0, pos);
		std::string postPath = path.substr(endPos + 1);

		resolvedPath = prePath + envPath + postPath;
	}
	else
	{
		resolvedPath = path;
	}

	return resolvedPath;
}

std::vector<std::string> SolutionParserUtility::makePathsCanonical(const std::vector<std::string>& paths)
{
	std::vector<std::string> canonicalPaths;

	int errorCount = 0;

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		try
		{
			boost::filesystem::path canonicalPath = boost::filesystem::canonical(boost::filesystem::path(paths[i]));
			canonicalPaths.push_back(canonicalPath.string());
		}
		catch (std::exception& e)
		{
			std::string what = e.what();
			LOG_WARNING_STREAM(<< e.what());
			errorCount++;
		}
	}

	if (errorCount > 0)
	{
		std::stringstream errorMessage;
		errorMessage << "Detected " << errorCount << " invalid include file paths. Check if the include paths of your VS project exist.";
		MessageStatus(errorMessage.str(), true).dispatch();
	}

	return canonicalPaths;
}
