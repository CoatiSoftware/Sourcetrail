# Generates the lincens.h file
# For adding a new 3rd Party License add the License File to the 3rd Party License Folder
# and a new AddLicense Line in below

set(LICENSES "")
set(LICENSE_ARRAY "")
set(LICENSEFOLDER "setup/3rdPartyLicenses")

function(ReadLicense licenseFile licenseVariable)
    file(READ ${licenseFile} tempVariable)
    STRING(REGEX REPLACE "\"" "\\\\\"" tempVariable "${tempVariable}")
    STRING(REGEX REPLACE "\n" "\\\\n\"\n\t\"" tempVariable "${tempVariable}")
    set(var "\n\nstatic const char *${licenseVariable}=\n\t\"")
    set(LICENSES "${LICENSES}${var}${tempVariable}\"\;" PARENT_SCOPE)
endfunction(ReadLicense)

function(AddLicense licenseName licenseURL licenseFile)
    ReadLicense(${licenseFile} ${licenseName}_license)
    set(LICENSES ${LICENSES} PARENT_SCOPE)
    set(LICENSE_ARRAY "${LICENSE_ARRAY}\n\tThirdPartyLicense(\"${licenseName}\", \"${licenseURL}\", ${licenseName}_license)," PARENT_SCOPE)
endfunction(AddLicense)

AddLicense("Qt" "http://qt.io" "${LICENSEFOLDER}/qt_LICENSE.TXT")
AddLicense("Clang" "http://clang.llvm.org/" "${LICENSEFOLDER}/clang_LICENSE.TXT")
AddLicense("Boost" "http://www.boost.org" "${LICENSEFOLDER}/boost_LICENSE.TXT")
AddLicense("TinyXMl" "http://www.grinninglizard.com/tinyxml2/" "${LICENSEFOLDER}/tinyxml_LICENSE.TXT")
AddLicense("CppSQLite" "http://www.codeproject.com/Articles/6343/CppSQLite-C-Wrapper-for-SQLite" "${LICENSEFOLDER}/CppSQLite_LICENSE.TXT")
AddLicense("Eigen" "http://eigen.tuxfamily.org" "${LICENSEFOLDER}/eigen_LICENSE.TXT")

set(LICENSE_ARRAY "${LICENSE_ARRAY}\n")

configure_file(
        ${CMAKE_SOURCE_DIR}/src/lib_gui/licenses.h.in
        ${CMAKE_SOURCE_DIR}/build/src/lib_gui/licenses.h
)
