## Sourcetrail

### Setup

#### External Software

* JDK 1.8
* QT 5.10.1
* CxxTest 4.3
* Clang & LLVM 6.0 (doesnt quite work for windows, use unix setup below and skip all the ninja stuff)(installation guide http://clang.llvm.org/docs/LibASTMatchersTutorial.html)
* Boost 1.68
* Botan 2.1.0
* Image Magick 7.0.6
* ccache (Unix)
* Visual Leak Detector (Windows)
* Wix 3.11 (Windows)
* Winrar (Windows)

#### Environment Variables

* CXX_TEST_DIR - .../cxxtest-4.3
* CLANG_DIR - .../clang-llvm
* BOOST_DIR - .../boost_1_68_0
* BOTAN_DIR - .../Botan-2.1.0

For MacOS and Linux
* QT_DIR - .../Qt/Qt5.10.1/5.10.1/<IDE>
* LLVM_DIR - .../clang-llvm

For Windows:
* QT_WIN32_DIR - .../Qt/Qt5.10.1/5.10.1/msvc2015
* QT_WIN64_DIR - .../Qt/Qt5.10.1/5.10.1/msvc2015_64
* VLD_DIR - .../Visual Leak Detector
* JAVA_HOME - .../Java/jdk1.x.x_xxx
* path
	- append path to git.exe
	- append path to jdk/bin
	- append path to VisualStudio/Common7/Tools
	- append path to VisualStudio/Common7/IDE
	- append path to .../Microsoft SDKs/Windows/v7.1A/Bin (for uuidgen in deploy script)
	- append path to .../WiX Toolset v3.11/bin
	- append path to .../WinRAR

##### ccache

* install ccache
* if ccache is in path it will be used for recompilation

##### Clang setup

For Windows:
Execute Cmake twice (once for each target compiler (32 and 64 bit)). Set the respective build path to ${CLANG_DIR}/build_win32 or ${CLANG_DIR}/build_win64

##### Boost setup

For Windows:
Build the Boost libs for 32 and 64 bit. Make sure that the platform specific libs are located in ${BOOST_DIR}/lib32-msvc-14.0 and ${BOOST_DIR}/lib64-msvc-14.0.

For Mac:
$ ./bootstrap.sh --with-libraries=filesystem,program_options,system,date_time
$ ./b2 --link=static --variant=release --threading=multi --runtime-link=static --cxxflags=-fPIC

##### Botan setup

For Windows

Extract the contents of your Botan package into "${BOTAN_DIR}/win32". Duplicate that folder and rename it "win64".

Build 32 bit debug lib in ${BOTAN_DIR}/win32/debug
$ python configure.py --cc=msvc --cpu=x86_32 --disable-shared --no-optimizations --with-debug-info

Build 32 bit release lib in ${BOTAN_DIR}/win32/release
$ python configure.py --cc=msvc --cpu=x86_32 --disable-shared

To build the 64 bit lib start the VS command prompt with the "amd64" argument as described here:https://msdn.microsoft.com/en-us/library/x4d2c09s.aspx

Build 64 bit debug lib in ${BOTAN_DIR}/win64/debug
$ python configure.py --cc=msvc --cpu=x86_64 --disable-shared --no-optimizations --with-debug-info

Build 64 bit release lib in ${BOTAN_DIR}/win64/release
$ python configure.py --cc=msvc --cpu=x86_64 --disable-shared


For MacOS and Linux

Build release lib in ${BOTAN_DIR}/release
Build debug lib in ${BOTAN_DIR}/debug

for Mac:
$ mkdir debug/release
$ cd debug/release
$ python ../configure.py --disable-shared --disable-modules=darwin_secrandom (--with-debug-info)
$ make -j 4

for Linux:
$ python configure.py --disable-shared

#### Settings

Run setup script:
$ ./script/setup.sh

### Release

#### App
* create release build
* make sure it uses the Sourcetrail icon
* obfuscate the executable using upx

#### Data folder
* put data folder in working directory
* add empty log folder
* add working tictactoe.xml project to /projects/tictactoe with sources in /projects/tictactoe/src
* add empty app settings with one recent project tictactoe.xml
* leave window_settings.ini with correct Sourcetrail start layout

#### Package
* add app

#### Updating Clang on UNIX

$ cd .../clang_llvm

$ cd llvm
$ git pull origin master

$ cd tools/clang
$ git pull origin master

$ cd tools/extra/
$ git pull origin master

$ cd ../../../../../build_debug
$ cmake -G Ninja -DLLVM_ENABLE_RTTI=ON ../llvm
$ ninja -j4 check-all

$ cd ../build_release
$ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_RTTI=ON ../llvm
$ ninja -j4 check-all

# Update CMakeLists.txt symlink directory
# Update cmake/linux_package.cmake clang include path
# Update LLVM version in dockerfiles

#### Remarks

* Boost lib dir: rename library directory for your system (e.g.: 'lib32-msvc-12.0') to 'lib'

### Tagging

$ git tag -a VERSION_NUMBER -m "DESCRIPTION"
$ git push --tags

### Loader gif creation from png sequence

// from png sequence
convert -delay 3 -loop 0 souretrail_*.png souretrail.gif

// less colors
gifsicle --colors 255 souretrail.gif > color.gif

// crop size
gifsicle --crop 0,5+0x-5 color.gif > crop.gif

// add transparency
gifsicle --unoptimize --disposal=previous --transparent="#FFFFFF" crop.gif > trans.gif

// resize
gifsicle --resize-height 18 trans.gif > loader.gif
