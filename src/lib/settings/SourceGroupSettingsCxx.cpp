#include "settings/SourceGroupSettingsCxx.h"

#include "utility/utility.h"
#include "utility/utilityApp.h"

std::vector<std::string> SourceGroupSettingsCxx::getAvailableArchTypes()
{
	return {
		"aarch64",
		"aarch64_be",
		"arm",
		"armeb",
		"avr",
		"bpfel",
		"bpfeb",
		"hexagon",
		"mips",
		"mipsel",
		"mips64",
		"mips64el",
		"msp430",
		"powerpc64",
		"powerpc64le",
		"powerpc",
		"r600",
		"amdgcn",
		"riscv32",
		"riscv64",
		"sparc",
		"sparcv9",
		"sparcel",
		"s390x",
		"tce",
		"tcele",
		"thumb",
		"thumbeb",
		"i386",
		"x86_64",
		"xcore",
		"nvptx",
		"nvptx64",
		"le32",
		"le64",
		"amdil",
		"amdil64",
		"hsail",
		"hsail64",
		"spir",
		"spir64",
		"kalimba",
		"lanai",
		"shave",
		"wasm32",
		"wasm64",
		"renderscript32",
		"renderscript64",
	};
}

std::vector<std::string> SourceGroupSettingsCxx::getAvailableVendorTypes()
{
	return {
	  "unknown",
	  "apple",
	  "pc",
	  "scei",
	  "bgp",
	  "bgq",
	  "fsl",
	  "ibm",
	  "img",
	  "mti",
	  "nvidia",
	  "csr",
	  "myriad",
	  "amd",
	  "mesa"
	};
}

std::vector<std::string> SourceGroupSettingsCxx::getAvailableOsTypes()
{
	return {
		"unknown",
		"cloudabi",
		"darwin",
		"dragonfly",
		"freebsd",
		"fuchsia",
		"ios",
		"kfreebsd",
		"linux",
		"lv2",
		"macosx",
		"netbsd",
		"openbsd",
		"solaris",
		"windows",
		"haiku",
		"minix",
		"rtems",
		"nacl",
		"cnk",
		"bitrig",
		"aix",
		"cuda",
		"nvcl",
		"amdhsa",
		"ps4",
		"elfiamcu",
		"tvos",
		"watchos",
		"mesa3d",
		"contiki"
	};
}

std::vector<std::string> SourceGroupSettingsCxx::getAvailableEnvironmentTypes()
{
	return {
		"unknown",
		"gnu",
		"gnuabi64",
		"gnueabihf",
		"gnueabi",
		"gnux32",
		"code16",
		"eabi",
		"eabihf",
		"android",
		"musl",
		"musleabi",
		"musleabihf",
		"msvc",
		"itanium",
		"cygnus",
		"amdopencl",
		"coreclr",
		"opencl"
	};
}

SourceGroupSettingsCxx::SourceGroupSettingsCxx(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
	, m_headerSearchPaths(std::vector<FilePath>())
	, m_frameworkSearchPaths(std::vector<FilePath>())
	, m_compilerFlags(std::vector<std::string>())
	, m_useSourcePathsForHeaderSearch(false)
	, m_hasDefinedUseSourcePathsForHeaderSearch(false)
	, m_compilationDatabasePath(FilePath())
{
}

SourceGroupSettingsCxx::~SourceGroupSettingsCxx()
{
}

bool SourceGroupSettingsCxx::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxx> otherCxx = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(other);

	return (
		otherCxx &&
		SourceGroupSettings::equals(other) &&
		utility::isPermutation(m_headerSearchPaths, otherCxx->m_headerSearchPaths) &&
		utility::isPermutation(m_frameworkSearchPaths, otherCxx->m_frameworkSearchPaths) &&
		utility::isPermutation(m_compilerFlags, otherCxx->m_compilerFlags) &&
		m_useSourcePathsForHeaderSearch == otherCxx->m_useSourcePathsForHeaderSearch &&
		m_hasDefinedUseSourcePathsForHeaderSearch == otherCxx->m_hasDefinedUseSourcePathsForHeaderSearch &&
		m_compilationDatabasePath == otherCxx->m_compilationDatabasePath &&
		getTargetFlag() == otherCxx->getTargetFlag()
	);
}

std::vector<std::string> SourceGroupSettingsCxx::getAvailableLanguageStandards() const
{
	std::vector<std::string> standards;

	switch (getType())
	{
	case SOURCE_GROUP_CPP_EMPTY:
	case SOURCE_GROUP_CXX_CDB:
		standards.push_back("c++1z");
		standards.push_back("gnu++1z");

		standards.push_back("c++14");
		standards.push_back("gnu++14");

		standards.push_back("c++1y");
		standards.push_back("gnu++1y");

		standards.push_back("c++11");
		standards.push_back("gnu++11");

		standards.push_back("c++0x");
		standards.push_back("gnu++0x");

		standards.push_back("c++03");

		standards.push_back("c++98");
		standards.push_back("gnu++98");
		break;

	case SOURCE_GROUP_C_EMPTY:
		standards.push_back("c1x");
		standards.push_back("gnu1x");
		standards.push_back("iso9899:201x");

		standards.push_back("c11");
		standards.push_back("gnu11");
		standards.push_back("iso9899:2011");

		standards.push_back("c9x");
		standards.push_back("gnu9x");
		standards.push_back("iso9899:199x");

		standards.push_back("c99");
		standards.push_back("gnu99");
		standards.push_back("iso9899:1999");

		standards.push_back("iso9899:199409");

		standards.push_back("c90");
		standards.push_back("gnu90");
		standards.push_back("iso9899:1990");

		standards.push_back("c89");
		standards.push_back("gnu89");
		break;

	default:
		break;
	}

	return standards;
}

std::vector<FilePath> SourceGroupSettingsCxx::getHeaderSearchPaths() const
{
	return m_headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getHeaderSearchPathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getHeaderSearchPaths());
}

void SourceGroupSettingsCxx::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	m_headerSearchPaths = headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getFrameworkSearchPaths());
}

void SourceGroupSettingsCxx::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	m_frameworkSearchPaths = frameworkSearchPaths;
}

bool SourceGroupSettingsCxx::getTargetOptionsEnabled() const
{
	return m_targetOptionsEnabled;
}

void SourceGroupSettingsCxx::setTargetOptionsEnabled(bool targetOptionsEnabled)
{
	m_targetOptionsEnabled = targetOptionsEnabled;
}

std::string SourceGroupSettingsCxx::getTargetArch() const
{
	return m_targetArch;
}

void SourceGroupSettingsCxx::setTargetArch(const std::string& arch)
{
	m_targetArch = arch;
}

std::string SourceGroupSettingsCxx::getTargetVendor() const
{
	return m_targetVendor;
}

void SourceGroupSettingsCxx::setTargetVendor(const std::string& vendor)
{
	m_targetVendor = vendor;
}

std::string SourceGroupSettingsCxx::getTargetSys() const
{
	return m_targetSys;
}

void SourceGroupSettingsCxx::setTargetSys(const std::string& sys)
{
	m_targetSys = sys;
}

std::string SourceGroupSettingsCxx::getTargetAbi() const
{
	return m_targetAbi;
}

void SourceGroupSettingsCxx::setTargetAbi(const std::string& abi)
{
	m_targetAbi = abi;
}

std::string SourceGroupSettingsCxx::getTargetFlag() const
{
	std::string targetFlag = "";
	if (m_targetOptionsEnabled)
	{
		targetFlag = "--target=";
		targetFlag += m_targetArch;
		if (!m_targetSub.empty())
		{
			targetFlag += m_targetSub;
		}
		targetFlag += "-" + m_targetVendor;
		targetFlag += "-" + m_targetSys;
		targetFlag += "-" + m_targetAbi;
	}
	return targetFlag;
}

std::vector<std::string> SourceGroupSettingsCxx::getCompilerFlags() const
{
	return m_compilerFlags;
}

void SourceGroupSettingsCxx::setCompilerFlags(const std::vector<std::string>& compilerFlags)
{
	m_compilerFlags = compilerFlags;
}

bool SourceGroupSettingsCxx::getUseSourcePathsForHeaderSearch() const
{
	return m_useSourcePathsForHeaderSearch;
}

void SourceGroupSettingsCxx::setUseSourcePathsForHeaderSearch(bool useSourcePathsForHeaderSearch)
{
	m_useSourcePathsForHeaderSearch = useSourcePathsForHeaderSearch;
}

bool SourceGroupSettingsCxx::getHasDefinedUseSourcePathsForHeaderSearch() const
{
	return m_hasDefinedUseSourcePathsForHeaderSearch;
}

void SourceGroupSettingsCxx::setHasDefinedUseSourcePathsForHeaderSearch(bool hasDefinedUseSourcePathsForHeaderSearch)
{
	m_hasDefinedUseSourcePathsForHeaderSearch = hasDefinedUseSourcePathsForHeaderSearch;
}

FilePath SourceGroupSettingsCxx::getCompilationDatabasePath() const
{
	return m_compilationDatabasePath;
}

FilePath SourceGroupSettingsCxx::getCompilationDatabasePathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getCompilationDatabasePath());
}

void SourceGroupSettingsCxx::setCompilationDatabasePath(const FilePath& compilationDatabasePath)
{
	m_compilationDatabasePath = compilationDatabasePath;
}

bool SourceGroupSettingsCxx::getShouldApplyAnonymousTypedefTransformation() const
{
	return m_shouldApplyAnonymousTypedefTransformation;
}

void SourceGroupSettingsCxx::setShouldApplyAnonymousTypedefTransformation(bool shouldApplyAnonymousTypedefTransformation)
{
	m_shouldApplyAnonymousTypedefTransformation = shouldApplyAnonymousTypedefTransformation;
}

std::vector<std::string> SourceGroupSettingsCxx::getDefaultSourceExtensions() const
{
	std::vector<std::string> defaultValues;

	switch (getType())
	{
	case SOURCE_GROUP_CPP_EMPTY:
		defaultValues.push_back(".cpp");
		defaultValues.push_back(".cxx");
		defaultValues.push_back(".cc");
		break;
	case SOURCE_GROUP_C_EMPTY:
		defaultValues.push_back(".c");
		break;
	case SOURCE_GROUP_CXX_CDB:
	default:
		break;
	}

	return defaultValues;
}

std::string SourceGroupSettingsCxx::getDefaultStandard() const
{
	switch (getType())
	{
	case SOURCE_GROUP_CPP_EMPTY:
		return "c++1z";
	case SOURCE_GROUP_C_EMPTY:
		return "c1x";
	case SOURCE_GROUP_CXX_CDB:
	default:
		break;
	}

	return "";
}
