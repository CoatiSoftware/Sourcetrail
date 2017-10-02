#include "settings/SourceGroupSettingsCxxEmpty.h"

#include "utility/utility.h"
#include "utility/utilityApp.h"

std::vector<std::string> SourceGroupSettingsCxxEmpty::getAvailableArchTypes()
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

std::vector<std::string> SourceGroupSettingsCxxEmpty::getAvailableVendorTypes()
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

std::vector<std::string> SourceGroupSettingsCxxEmpty::getAvailableOsTypes()
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

std::vector<std::string> SourceGroupSettingsCxxEmpty::getAvailableEnvironmentTypes()
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

SourceGroupSettingsCxxEmpty::SourceGroupSettingsCxxEmpty(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, type, projectSettings)
	, m_targetOptionsEnabled(false)
	, m_targetArch("")
	, m_targetVendor("")
	, m_targetSys("")
	, m_targetAbi("")
{
}

SourceGroupSettingsCxxEmpty::~SourceGroupSettingsCxxEmpty()
{
}

void SourceGroupSettingsCxxEmpty::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	setTargetOptionsEnabled(getValue<bool>(key + "/cross_compilation/target_options_enabled", false, config));
	setTargetArch(getValue<std::string>(key + "/cross_compilation/target/arch", "", config));
	setTargetVendor(getValue<std::string>(key + "/cross_compilation/target/vendor", "", config));
	setTargetSys(getValue<std::string>(key + "/cross_compilation/target/sys", "", config));
	setTargetAbi(getValue<std::string>(key + "/cross_compilation/target/abi", "", config));
}

void SourceGroupSettingsCxxEmpty::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	setValue(key + "/cross_compilation/target_options_enabled", getTargetOptionsEnabled(), config);
	setValue(key + "/cross_compilation/target/arch", getTargetArch(), config);
	setValue(key + "/cross_compilation/target/vendor", getTargetVendor(), config);
	setValue(key + "/cross_compilation/target/sys", getTargetSys(), config);
	setValue(key + "/cross_compilation/target/abi", getTargetAbi(), config);
}

bool SourceGroupSettingsCxxEmpty::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxxEmpty> otherCxxEmpty = std::dynamic_pointer_cast<SourceGroupSettingsCxxEmpty>(other);

	return (
		otherCxxEmpty &&
		SourceGroupSettingsCxx::equals(other) &&
		getTargetFlag() == otherCxxEmpty->getTargetFlag()
	);
}

bool SourceGroupSettingsCxxEmpty::getTargetOptionsEnabled() const
{
	return m_targetOptionsEnabled;
}

void SourceGroupSettingsCxxEmpty::setTargetOptionsEnabled(bool targetOptionsEnabled)
{
	m_targetOptionsEnabled = targetOptionsEnabled;
}

std::string SourceGroupSettingsCxxEmpty::getTargetArch() const
{
	return m_targetArch;
}

void SourceGroupSettingsCxxEmpty::setTargetArch(const std::string& arch)
{
	m_targetArch = arch;
}

std::string SourceGroupSettingsCxxEmpty::getTargetVendor() const
{
	return m_targetVendor;
}

void SourceGroupSettingsCxxEmpty::setTargetVendor(const std::string& vendor)
{
	m_targetVendor = vendor;
}

std::string SourceGroupSettingsCxxEmpty::getTargetSys() const
{
	return m_targetSys;
}

void SourceGroupSettingsCxxEmpty::setTargetSys(const std::string& sys)
{
	m_targetSys = sys;
}

std::string SourceGroupSettingsCxxEmpty::getTargetAbi() const
{
	return m_targetAbi;
}

void SourceGroupSettingsCxxEmpty::setTargetAbi(const std::string& abi)
{
	m_targetAbi = abi;
}

std::string SourceGroupSettingsCxxEmpty::getTargetFlag() const
{
	std::string targetFlag = "";
	if (m_targetOptionsEnabled && !m_targetArch.empty())
	{
		targetFlag = "--target=";
		targetFlag += m_targetArch;
		targetFlag += "-" + (m_targetVendor.empty() ? "unknown" : m_targetVendor);
		targetFlag += "-" + (m_targetSys.empty() ? "unknown" : m_targetSys);
		targetFlag += "-" + (m_targetAbi.empty() ? "unknown" : m_targetAbi);
	}
	return targetFlag;
}
