#include "settings/SourceGroupSettingsWithCxxCrossCompilationOptions.h"

#include "settings/ProjectSettings.h"


std::vector<std::wstring> SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableArchTypes()
{
	return{
		L"aarch64",
		L"aarch64_be",
		L"arm",
		L"armeb",
		L"avr",
		L"bpfel",
		L"bpfeb",
		L"hexagon",
		L"mips",
		L"mipsel",
		L"mips64",
		L"mips64el",
		L"msp430",
		L"powerpc64",
		L"powerpc64le",
		L"powerpc",
		L"r600",
		L"amdgcn",
		L"riscv32",
		L"riscv64",
		L"sparc",
		L"sparcv9",
		L"sparcel",
		L"s390x",
		L"tce",
		L"tcele",
		L"thumb",
		L"thumbeb",
		L"i386",
		L"x86_64",
		L"xcore",
		L"nvptx",
		L"nvptx64",
		L"le32",
		L"le64",
		L"amdil",
		L"amdil64",
		L"hsail",
		L"hsail64",
		L"spir",
		L"spir64",
		L"kalimba",
		L"lanai",
		L"shave",
		L"wasm32",
		L"wasm64",
		L"renderscript32",
		L"renderscript64",
	};
}

std::vector<std::wstring> SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableVendorTypes()
{
	return{
		L"unknown",
		L"apple",
		L"pc",
		L"scei",
		L"bgp",
		L"bgq",
		L"fsl",
		L"ibm",
		L"img",
		L"mti",
		L"nvidia",
		L"csr",
		L"myriad",
		L"amd",
		L"mesa"
	};
}

std::vector<std::wstring> SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableOsTypes()
{
	return{
		L"unknown",
		L"cloudabi",
		L"darwin",
		L"dragonfly",
		L"freebsd",
		L"fuchsia",
		L"ios",
		L"kfreebsd",
		L"linux",
		L"lv2",
		L"macosx",
		L"netbsd",
		L"openbsd",
		L"solaris",
		L"windows",
		L"haiku",
		L"minix",
		L"rtems",
		L"nacl",
		L"cnk",
		L"bitrig",
		L"aix",
		L"cuda",
		L"nvcl",
		L"amdhsa",
		L"ps4",
		L"elfiamcu",
		L"tvos",
		L"watchos",
		L"mesa3d",
		L"contiki"
	};
}

std::vector<std::wstring> SourceGroupSettingsWithCxxCrossCompilationOptions::getAvailableEnvironmentTypes()
{
	return{
		L"unknown",
		L"gnu",
		L"gnuabi64",
		L"gnueabihf",
		L"gnueabi",
		L"gnux32",
		L"code16",
		L"eabi",
		L"eabihf",
		L"android",
		L"musl",
		L"musleabi",
		L"musleabihf",
		L"msvc",
		L"itanium",
		L"cygnus",
		L"amdopencl",
		L"coreclr",
		L"opencl"
	};
}

SourceGroupSettingsWithCxxCrossCompilationOptions::SourceGroupSettingsWithCxxCrossCompilationOptions()
	: m_targetOptionsEnabled(false)
	, m_targetArch(L"")
	, m_targetVendor(L"")
	, m_targetSys(L"")
	, m_targetAbi(L"")
{
}

bool SourceGroupSettingsWithCxxCrossCompilationOptions::equals(std::shared_ptr<SourceGroupSettingsWithCxxCrossCompilationOptions> other) const
{
	return (
		other &&
		getTargetFlag() == other->getTargetFlag()
	);
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setTargetOptionsEnabled(config->getValueOrDefault<bool>(key + "/cross_compilation/target_options_enabled", false));
	setTargetArch(config->getValueOrDefault<std::wstring>(key + "/cross_compilation/target/arch", L""));
	setTargetVendor(config->getValueOrDefault<std::wstring>(key + "/cross_compilation/target/vendor", L""));
	setTargetSys(config->getValueOrDefault<std::wstring>(key + "/cross_compilation/target/sys", L""));
	setTargetAbi(config->getValueOrDefault<std::wstring>(key + "/cross_compilation/target/abi", L""));
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/cross_compilation/target_options_enabled", getTargetOptionsEnabled());
	config->setValue(key + "/cross_compilation/target/arch", getTargetArch());
	config->setValue(key + "/cross_compilation/target/vendor", getTargetVendor());
	config->setValue(key + "/cross_compilation/target/sys", getTargetSys());
	config->setValue(key + "/cross_compilation/target/abi", getTargetAbi());
}

bool SourceGroupSettingsWithCxxCrossCompilationOptions::getTargetOptionsEnabled() const
{
	return m_targetOptionsEnabled;
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::setTargetOptionsEnabled(bool targetOptionsEnabled)
{
	m_targetOptionsEnabled = targetOptionsEnabled;
}

std::wstring SourceGroupSettingsWithCxxCrossCompilationOptions::getTargetArch() const
{
	return m_targetArch;
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::setTargetArch(const std::wstring& arch)
{
	m_targetArch = arch;
}

std::wstring SourceGroupSettingsWithCxxCrossCompilationOptions::getTargetVendor() const
{
	return m_targetVendor;
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::setTargetVendor(const std::wstring& vendor)
{
	m_targetVendor = vendor;
}

std::wstring SourceGroupSettingsWithCxxCrossCompilationOptions::getTargetSys() const
{
	return m_targetSys;
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::setTargetSys(const std::wstring& sys)
{
	m_targetSys = sys;
}

std::wstring SourceGroupSettingsWithCxxCrossCompilationOptions::getTargetAbi() const
{
	return m_targetAbi;
}

void SourceGroupSettingsWithCxxCrossCompilationOptions::setTargetAbi(const std::wstring& abi)
{
	m_targetAbi = abi;
}

std::wstring SourceGroupSettingsWithCxxCrossCompilationOptions::getTargetFlag() const
{
	std::wstring targetFlag = L"";
	if (m_targetOptionsEnabled && !m_targetArch.empty())
	{
		targetFlag = L"--target=";
		targetFlag += m_targetArch;
		targetFlag += L"-" + (m_targetVendor.empty() ? L"unknown" : m_targetVendor);
		targetFlag += L"-" + (m_targetSys.empty() ? L"unknown" : m_targetSys);
		targetFlag += L"-" + (m_targetAbi.empty() ? L"unknown" : m_targetAbi);
	}
	return targetFlag;
}
