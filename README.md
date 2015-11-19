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

#### Changelog

##### 0.3.0

* Added Tutorial project
* Added communication interface from and to Coati via TCP sockets
* Added plugin for Visual Studio (2012, 2013, 2015)
* Added plugin for Sublime Text (2 & 3)
* Increased autocompletion performance by caching and limiting results to 100
* Save Coati version to sqlite and reparse project whenever the version changes
* Improved source locations of template scopes
* Fixed horizontal scroll in code view not activated while scrolling vertically
* Save errors to storage and reactivate them via button in the status bar
* Save macro scopes
* Fixed old data was still in new project window when creating another project
* Save comment locations and display them when right next to active symbol
* Reworked code view highlighting
* Fixed cursor was jumping at end of query when editing a search query
* Fixed search query disappeared when not token was found for it
* Improved code view auto scroll to center the first active line in the middle
* Focus all overlapping source locations in the code view
* Added recent projects menu option
* Saving all template arguments type uses
