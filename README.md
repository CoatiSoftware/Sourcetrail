### Masterproject

#### Setup

##### External Software

* QT 5.5
* CxxTest 4.3
* Valgrind 3.9.0 (linux)
* Clang & LLVM (installation guide http://clang.llvm.org/docs/LibASTMatchersTutorial.html)
* Boost 1.59
* Eigen 3.2.6

##### Environment Variables

* QT_DIR - ...\Qt\Qt5.2.1\5.2.1\<IDE>\
* CXX_TEST_DIR - .../cxxtest-4.3/
* CLANG_DIR - .../clang-llvm/
* BOOST_159_DIR - .../boost_1_59_0
* EIGEN_DIR - .../eigen

For Win32:
* VLD_DIR - .../Visual Leak Detector
* path - apped path to git.exe

##### Settings

Run setup script:
$ ./script/setup.sh

#### Release

##### App
* create release build
* make sure it uses the Coati icon
* obfuscate the executable using upx

##### Data folder
* put data folder in working directory
* add empty log folder
* add working tictactoe.xml project to /projects/tictactoe with sources in /projects/tictactoe/src
* add empty app settings with one recent project tictactoe.xml
* leave window_settings.ini with correct Coati start layout

##### Package
* add app
* add web/coati_manual.pdf to package

##### Updating Clang on UNIX

$ cd .../clang_llvm

$ cd llvm
$ git pull origin master

$ cd tools/clang
$ git pull origin master

$ cd tools/extra/
$ git pull origin master

$ cd ../../../../../build_debug
$ cmake -G Ninja ../llvm
$ ninja -j4 check-all

$ cd ../release_build
$ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../llvm
$ ninja -j4 check-all

##### Remarks

* Boost lib dir: rename library directory for your system (e.g.: 'lib32-msvc-12.0') to 'lib'