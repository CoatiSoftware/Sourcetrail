#ifndef CODEBLOCKS_TARGET_H
#define CODEBLOCKS_TARGET_H

#include <memory>
#include <string>
#include <vector>

#include "CodeblocksTargetRelationType.h"

class TiXmlElement;

namespace Codeblocks
{
	class Compiler;

	class Target
	{
	public:
		static std::string getXmlElementName();
		static std::shared_ptr<Target> create(const TiXmlElement* element);

		std::wstring getTitle() const;
		std::shared_ptr<const Compiler> getCompiler() const;

	private:
		Target();

		std::wstring m_title;
		TargetRelationType m_projectCompilerOptionsRelation;
		TargetRelationType m_projectIncludeDirsRelation;
		std::shared_ptr<Compiler> m_compiler;
	};
}

#endif // CODEBLOCKS_TARGET_H
