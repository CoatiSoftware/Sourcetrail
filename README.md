### Masterproject

#### Setup

##### External Software

* QT 5.5
* CxxTest 4.3
* Valgrind 3.9.0 (linux)
* Clang & LLVM (installation guide http://clang.llvm.org/docs/LibASTMatchersTutorial.html)
* Boost 1.59
* Eigen 3.2.6
* Botan 1.11.24

##### Environment Variables

* QT_DIR - ...\Qt\Qt5.2.1\5.2.1\<IDE>\
* CXX_TEST_DIR - .../cxxtest-4.3/
* CLANG_DIR - .../clang-llvm/
* BOOST_159_DIR - .../boost_1_59_0
* EIGEN_DIR - .../eigen
* BOTAN_DIR - .../Botan-1.11.24

For Win32:
* VLD_DIR - .../Visual Leak Detector
* path - apped path to git.exe

###### Botan setup

Build release lib in ${BOTAN_DIR}/release
Build debug lib in ${BOTAN_DIR}/debug

for Windows:
$ python configure.py --cc=msvc --cpu=x86_32 --via-amalgamation --disable-shared

for other:
$ python configure.py --via-amalgamation --disable-shared

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

##### 0.4.0

* Network ports for IDE communication can be defined in the ApplicationSettings.xml
* Coati can now handle inclass initialization of members
* Improved handling of template arguments in analysis
* Coati now handles anonymous symbols when parsing files
* Extended color schemes to allow more extensive use in the graph
* Added new color scheme called "bad_rainbow"
* Coati shows a template's general definition when an implicit specialization is activated
* Fixed source locations of many declarations
* Fixed syntax highlighting to correctly identify strings, comments and block comments
* Added analysis for template parameter packs
* Show function and method signature in tooltip when hovering the graph node
* Fixed graph font size was too small for Windows and Linux
* Removed Coati directory from Linux package
* Introduced file endings .coatiproject and .coatidb for Coati's project files
* Added installer for Windows
* Installing prerequisits in Windows Installer
* Redesigned the about window
* Parse errors will be displayed as they appear during analysis
* Fixed constructors and destructors using return type 'void' withou source location
* Removed 'close window' action from menu
* Added help texts to project setup and preferences window
* Improved lambda handling in analysis
* Fixed style of function or method nodes with children
* Fixed tooltip colors in dark color scheme
* renamed undo&redo to back&forward
* navigate back&forward via mouse buttons
* navigate back via backspace key

