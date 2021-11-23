# Generates the lincens.h file
# For adding a new 3rd Party License add the License File to the 3rd Party License Folder
# and a new AddLicense Line in below

set(LICENSES "")
set(LICENSE_ARRAY "")
set(LICENSEFOLDER "${CMAKE_SOURCE_DIR}/bin/app/data/license/3rd_party_licenses")

function(ReadLicense licenseFile licenseVariable)
    file(READ ${licenseFile} tempVariable)
    STRING(REGEX REPLACE "\"" "\\\\\"" tempVariable "${tempVariable}")
    STRING(REGEX REPLACE "\n" "\\\\n\"\n\t\"" tempVariable "${tempVariable}")
    set(var "\n\nstatic const char *${licenseVariable}=\n\t\"")
    set(LICENSES "${LICENSES}${var}${tempVariable}\"\;" PARENT_SCOPE)
endfunction(ReadLicense)

function(AddLicense softwareName softwareVersion softwareURL licenseFile)
        ReadLicense(${licenseFile} ${softwareName}_license)
        set(LICENSES ${LICENSES} PARENT_SCOPE)
        set(LICENSE_ARRAY "${LICENSE_ARRAY}\n\tLicenseInfo(\"${softwareName}\", \"${softwareVersion}\", \"${softwareURL}\", ${softwareName}_license)," PARENT_SCOPE)
endfunction(AddLicense)

ReadLicense(${CMAKE_SOURCE_DIR}/LICENSE.txt Sourcetrail_license)
set(LICENSE_APP "LicenseInfo(\"Sourcetrail\", \"${VERSION_STRING}\", \"https://github.com/CoatiSoftware/Sourcetrail\", Sourcetrail_license)")

AddLicense("Boost" "1.68" "http://www.boost.org" "${LICENSEFOLDER}/license_boost.txt")
AddLicense("catch" "2.5.0" "https://github.com/catchorg/Catch2" "${LICENSEFOLDER}/license_catch.txt")
AddLicense("Clang" "8.0.1" "http://clang.llvm.org/" "${LICENSEFOLDER}/license_clang.txt")
AddLicense("CppSQLite" "3.2" "http://www.codeproject.com/Articles/6343/CppSQLite-C-Wrapper-for-SQLite" "${LICENSEFOLDER}/license_cpp_sqlite.txt")
AddLicense("EclipseJDT" "" "https://github.com/eclipse/eclipse.jdt.core" "${LICENSEFOLDER}/license_eclipse.txt")
AddLicense("Gradle" "4.2" "https://github.com/gradle/gradle" "${LICENSEFOLDER}/license_gradle.txt")
AddLicense("OpenSSL" "" "https://www.openssl.org/" "${LICENSEFOLDER}/license_openssl.txt")
AddLicense("Qt" "5.12.6" "http://qt.io" "${LICENSEFOLDER}/license_qt.txt")
AddLicense("TinyXML" "2.0" "https://sourceforge.net/projects/tinyxml/" "${LICENSEFOLDER}/license_tinyxml.txt")

set(LICENSE_ARRAY "${LICENSE_ARRAY}\n")

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/licenses.h.in
    ${CMAKE_BINARY_DIR}/src/lib_gui/licenses.h
)
