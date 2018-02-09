#include "data/parser/cxx/utilityClang.h"

#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>

#include "utility/file/FilePath.h"
#include "utility/utilityString.h"

bool utility::isImplicit(const clang::Decl* d)
{
	if (!d)
	{
		return false;
	}

	if (d->isImplicit())
	{
		if (const clang::RecordDecl* rd = clang::dyn_cast_or_null<clang::RecordDecl>(d))
		{
			if (rd->isLambda())
			{
				return isImplicit(clang::dyn_cast_or_null<clang::Decl>(d->getDeclContext()));
			}
		}
		return true;
	}
	else if (const clang::ClassTemplateSpecializationDecl* ctsd = clang::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(d))
	{
		if (!ctsd->isExplicitSpecialization())
		{
			return true;
		}
	}
	else if (const clang::FunctionDecl* fd = clang::dyn_cast_or_null<clang::FunctionDecl>(d))
	{
		if (
			fd->isTemplateInstantiation() && 
			fd->getTemplateSpecializationKind() != clang::TSK_ExplicitSpecialization) // or undefined??
		{
			return true;
		}
		else if (
			fd->getMemberSpecializationInfo() && 
			fd->getMemberSpecializationInfo()->getTemplateSpecializationKind() == clang::TSK_ExplicitSpecialization)
		{
			return false;
		}
	}

	return isImplicit(clang::dyn_cast_or_null<clang::Decl>(d->getDeclContext()));
}

AccessKind utility::convertAccessSpecifier(clang::AccessSpecifier access)
{
	switch (access)
	{
	case clang::AS_public:
		return ACCESS_PUBLIC;
	case clang::AS_protected:
		return ACCESS_PROTECTED;
	case clang::AS_private:
		return ACCESS_PRIVATE;
	case clang::AS_none:
		return ACCESS_NONE;
	}
}

SymbolKind utility::convertTagKind(const clang::TagTypeKind tagKind)
{
	switch (tagKind)
	{
	case clang::TTK_Struct:
		return SYMBOL_STRUCT;
	case clang::TTK_Union:
		return SYMBOL_UNION;
	case clang::TTK_Class:
		return SYMBOL_CLASS;
	case clang::TTK_Enum:
		return SYMBOL_ENUM;
	case clang::TTK_Interface:
		return SYMBOL_KIND_MAX;
	}
}

SymbolKind utility::getSymbolKind(const clang::VarDecl* d)
{
	SymbolKind symbolKind = SYMBOL_KIND_MAX;

	if (llvm::isa<clang::ParmVarDecl>(d))
	{
		symbolKind = SYMBOL_PARAMETER;
	}
	else if (d->getParentFunctionOrMethod() == NULL)
	{
		if (d->getAccess() == clang::AS_none)
		{
			symbolKind = SYMBOL_GLOBAL_VARIABLE;
		}
		else
		{
			symbolKind = SYMBOL_FIELD;
		}
	}
	else
	{
		symbolKind = SYMBOL_LOCAL_VARIABLE;
	}

	return symbolKind;
}

std::wstring utility::getFileNameOfFileEntry(const clang::FileEntry* entry)
{
	std::wstring fileName = L"";
	if (entry != nullptr && entry->isValid())
	{
		fileName = utility::decodeFromUtf8(entry->tryGetRealPathName());
		if (fileName.empty())
		{
			fileName = utility::decodeFromUtf8(entry->getName());
		}
		else
		{
			fileName = FilePath(utility::decodeFromUtf8(entry->getName().str())).getParentDirectory().concatenate(FilePath(fileName).fileName()).wstr();
		}
	}
	return fileName;
}
