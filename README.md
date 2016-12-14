## Coati

### Setup

#### External Software

* JDK 1.8
* QT 5.6
* CxxTest 4.3
* Valgrind 3.9.0 (linux)
* Clang & LLVM 3.8 (doesnt quite work for windows, use unix setup below and skip all the ninja stuff)(installation guide http://clang.llvm.org/docs/LibASTMatchersTutorial.html)
* Boost 1.59
* Botan 1.11.34

also something about java, but who knows...

#### Environment Variables

* QT_DIR - .../Qt/Qt5.6.0/5.6/<IDE>
* CXX_TEST_DIR - .../cxxtest-4.3
* CLANG_DIR - .../clang-llvm
* BOOST_159_DIR - .../boost_1_59_0
* BOTAN_DIR - .../Botan-1.11.34

For Win32:
* VLD_DIR - .../Visual Leak Detector
* path
	- apped path to git.exe
	- apped path to jdk/bin
	- append path to VisualStudio/Common7/Tools
	- append path to VisualStudio/Common7/IDE

##### Botan setup

Build release lib in ${BOTAN_DIR}/release
Build debug lib in ${BOTAN_DIR}/debug

for Windows release build:
$ python configure.py --cc=msvc --cpu=x86_32 --disable-shared

for Windows debug build:
python configure.py --cc=msvc --cpu=x86_32 --disable-shared --no-optimizations --with-debug-info

for Mac:
$ python configure.py --disable-shared --disable-modules=darwin_secrandom

for Linux:
$ python configure.py --disable-shared

#### Settings

Run setup script:
$ ./script/setup.sh

### Release

#### App
* create release build
* make sure it uses the Coati icon
* obfuscate the executable using upx

#### Data folder
* put data folder in working directory
* add empty log folder
* add working tictactoe.xml project to /projects/tictactoe with sources in /projects/tictactoe/src
* add empty app settings with one recent project tictactoe.xml
* leave window_settings.ini with correct Coati start layout

#### Package
* add app
* add web/coati_manual.pdf to package

#### Updating Clang on UNIX

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

#### Remarks

* Boost lib dir: rename library directory for your system (e.g.: 'lib32-msvc-12.0') to 'lib'

### Tagging

$ git tag -a VERSION_NUMBER -m "DESCRIPTION"
$ git push --tags

### Loader gif creation from png sequence

// from png sequence
convert -delay 3 -loop 0 coati_*.png coati.gif

// less colors
gifsicle --colors 255 coati.gif > color.gif

// crop size
gifsicle --crop 0,5+0x-5 color.gif > crop.gif

// add transparency
gifsicle --unoptimize --disposal=previous --transparent="#FFFFFF" crop.gif > trans.gif

// resize
gifsicle --resize-height 18 trans.gif > loader.gif

### Changelog

#### 0.9.22
released 2016-10-27

* Windows: Added logging and exception handling to VS plugin
* Windows: Added VC++ runtime dlls for windows installer
* Windows: Added missing VC++ runtime dlls (issue #231)
* Added setting for default graph zoom on mouse wheel to preferences (issue #237)
* Scroll errors table to newest error during indexing
* Fixed best scoring for search autocompletions skipped some combinations
* Fixed flickering of errors table when indexing
* Don't show error snippets in overview screen anymore
* Fixed crash when Project Paths contain files and lazy include search in enabled (issue #201)
* Fixed edge cases that caused deadlocks in indexing
* Added language specific project icons to start screen
* Fixed application couldn't be quit when start screen was showing (issue #227)
* Graph: Fixed missing image file ending when exporting graph for Linux
* Log Coati version when logging gets enabled
* Fixed non-indexed errors filter in errors table

#### 0.9.0
released 2016-10-14

* Extended Visual Studio Plugin to export clang Compilation Databases
* Implemented new installer for Windows
* Run System Header Include Paths and Java Path detection when starting application
* Added Log Window with Errors tab showing a table of all errors
* Added filters to bottom of error table to define which errors are displayed
* Removed ring logging and use one log file again instead
* Smaller indexing progress dialog with bar in the middle instead of golden ratio
* Code: Set default code snippet expand range to 3 lines for more context
* Graph: Fixed undoing of aggregation edge activation caused crash
* Graph: Move aggregation and inheritance edges to front when hovered or active
* Graph: Push parent nodes of active nodes to back to make all incoming and outgoing edges visible
* Properly quit application when window is closed while confirm box is open
* Properly quit application when window is closed while indexing (issue #121)
* Graph: Count only top level nodes in bundle nodes of graph
* Search: Improved scoring of autocompletion matches to use best score for match
* App will go into trial mode without valid license
* Java: Auto detect java root source directories
* Fixed refresh crash (issue #220)
* Java: improved indexer reliability a lot
* Java: Changed name of generic type to use name of declaration
* Fixed project refresh does not recognize removed source paths
* Java: Define maximum allocated jvm memory in preferences
* Follow symbolic directory links within source paths (issue #205)
* Use white size grip in blue dialogs
* Graph: Use type node colors for undefined nodes
* Java: Show import not found as error
* Code: Select and copy source code in code view with SHIFT + mouse drag (issue #7)
* Code: Added markers next to line numbers in code view showing active locations and scopes
* Merged app and trial to one build

#### 0.8.21
released 2016-09-16

* Added options for checking multiple paths in Indexed Header Paths dialog during project setup from Compilation Database #190
* Added all C/C++ standards supported by clang
* Fixed C language setting in empty project setup not propagated to indexer #177
* Added Sublime plugin to Sublime Text Package Control
* Improved indexer parallelization for faster indexing
* Changed all font-sizes to px for similar look on all platforms
* Added font face dropdown to Preferences showing all available monospace fonts #172
* Smoother font rendering on Windows by setting font engine to FreeType via qt.conf file #137
* Interrupt indexing while AST visiting for C/C++ for shorter waiting time
* Save graph as PNG image via context menu in graph view #180
* Added base folder to Linux package #181
* Use inverted golden ratio in indexing progress dialog

#### 0.8.0
released 2016-09-01

* Improved titles and help texts in project setup UI
* Fixed error locations not saved with absolute file paths in Compilation Database projects
* Added missing source files extensions setting to the Edit Project dialog #132
* Changed default text colors to black in project setup UI #140
* Added plugin ports settings to Preferences #145
* Improved project state handling on refreshing
* Added Java sample project JavaParser: http://javaparser.org/
* Fixed regex for system includes used wrongly in syntax highlighter
* Added setting to disable file and console logging, default is off #157
* Added migrations between different ApplicationSettings versions
* Added auto detection for Java library to Preferences
* Removed auto refresh option from Search view
* Added progress dialogs for indexing and block UI #143
* Hide name qualifiers for graph nodes in Java projects
* Added handling when java library not found
* Added plain text editing dialog to path list boxes
* Use language dependent symbol name delimiter in UI
* Added scroll speed setting to preferences #112
* Added Java project setup UI
* Record start and end locations of scopes in Java
* Display unresolved types names in Java as 'unresolved-type'
* Cache stats for overview to speed up display
* Fixed graph nodes to restore their expand state when going back #118
* Added shortcuts for navigating to next and previous reference in code view
* Pressing Tab in search completes up to next ::, Delete erases to last :: #133
* Bundle anonymous namespaces separately in graph overview #126
* Added 'Show Data Folder' and 'Show Log Folder' items to Help menu
* Added new status bar loader gif with higher resolution
* Record type aliases in C++
* Added Java indexer
* Changed public icon in graph and added default icon
* Scroll to file location when finding no symbol at specified location from plugin message
* Added shortcut for Preferences on Windows/Linux
* Leave code view at original line when expanding scopes
* Pan graph view to center on name of active symbol after activation
* Moved color scheme selection to Preferences
* Moved Preferences menu item to Edit menu on Windows/Linux #113
* Added setting for hiding non-fatal errors in unindexed files to preferences, turned on by default
* Fixed file out-of-date asterisk disappearing when showing snippets after minimize state
* Implemented Atom plugin: https://github.com/CoatiSoftware/atom-coati
* Implemented CLion/IntelliJ plugin: https://github.com/CoatiSoftware/idea-coati
* Implemented Eclipse plugin: https://github.com/CoatiSoftware/eCoati
* Implemented emacs plugin: https://github.com/CoatiSoftware/emacs-coati
* Improved code view rendering times
* Don't show implicit nodes in search auto completion list


#### 0.7.0.22
released 2016-07-07

* Fixed detected global header search paths lost correct order and caused problems with #include_next directives
* Fixed MaxOSX build crashed on launch due to unknown command line option passed
* Removed 'Save Project' and 'Save Project As' actions from menu
* Fixed indexer getting stuck at 'building caches' when indexing is interrupted
* Fixed database not properly closed when errors appeared during indexing
* Fixed autocompletions popup flickering while typing
* Added resize grip to bottom right corner of every dialog window
* Fixed 'paths don't exist' dialog window getting too big with lots of paths shown
* Added context menu with 'back' and 'forward' actions available in all views
* Removed Clang compiler warnings from console output
* Added handling of project specific VS macros to VS solution parser

#### 0.7.0.0
released 2016-06-15

* Updated to clang 3.8
* Fixed crash in search autocompletion retrieval due to edge case in sorting
* Fixed size policies in code view to avoid global scrollbar
* Added full text search via search field: ?<query> for case-insensitive search, ??<query> for case-sensitive search
* Made initialization for full text search lazy: done on first full text search
* Defined full text search color in all color schemes
* Increased code view performance by caching file IDs and paths
* Added multithreaded indexing. Number of threads can be set in the Preferences UI, default is 4
* Reworked graph node bundling to only distinguish defined and undefined nodes in each bucket
* Made graph node sorting alphabetical and put bundles at end
* Changed code view snippet sorting to always show definition on top
* Fixed local symbols were not available within lambdas
* Added settings for font family, font size and tab with to Preferences UI
* Fixed VS solution parser issue with internal macros
* Removed header extensions, all included files within the project paths are now analyzed
* Fixed crash in sqlite when symbol name has special characters
* Improved code view performance by reducing amount of initially open code files
* Improved graph view performance through faster data retrieval and smarter structures
* Increased aggregation click speed with faster edge id retrieval
* Fixed png color profiles to avoid console warning: "libpng warning: iCCP: known incorrect sRGB profile"
* Fixed graph edge drawn wrong when leaving from child node of smaller node
* Fixed Sublime Text Plugin issue with wrong encoding
* Randomized source file order for multithreaded analysis to increase performance
* Show name of removed symbol when going back after refreshing
* Fixed macro definition not connected to file
* Fixed macro use not found in #ifdef, #ifndef, defined() and some expand cases
* Removed back and forward steps for some graph and code view manipulations
* Fixed code view style broken on Mac due to change of working directory when analyzing compilation database
* Added database optimization step after indexing to reduce database size by ~25%
* Fixed artifacts after graph view change by clearing view
* Fixed autocompletion list highlights at wrong location for long names
* Allow files and directories to be excluded from analysis in advanced settings for project setup
* Added Qt gif plugin to Mac Release so that loader gif in status bar is visible
* Added link to download page to start screen to remind users of updating once in a while
* Check if compilation database still exists before refreshing
* Added separate step for defining project name, project location and compiliation database to project setup
* Explain that the project stays up-to-date with the compilation databes on refresh in project setup UI
* Disregard source extensions when loading from compilation database
* Removed error logs in name resolver
* Added plugin for VIM: https://github.com/CoatiSoftware/vim-coati

#### 0.6.0.0
released 2016-04-27

* Smoother graph panning and zooming on keyboard using update loop
* Differentiate between explicit and implicit nodes and only show implicit nodes in the graph view when connected
* Improved file clearing performance on project refresh
* Show deleted projects on the start screen and show messagebox to delete them
* Show analysis duration in hh:mm:ss
* Added suppport for temporary test licenses and show error message when it expired
* Increased graph performance by reducing to fixed number of requests
* Add vs-clang compatibility flags when creating project from Visual Studio solution
* Increased graph edge drawing performance
* Added auto detection for system header paths to project wizard and preferences when using gcc, clang or vs compilers
* Added bug tracker to help menu
* Increased search autocompletion performance to stay mostly below 100ms
* Changed style of aggregation edge to cleaner look
* Allow certain edge types to enter and leave at top and bottom of nodes as well e.g. inheritance
* Increased project load performance
* Increased graph creation performance for overview screen
* Changed namespace activation to show the namespace name in the search bar and the namespace definitions in the code view
* Updated to Qt 5.6
* Added analysis for local symbols and allow selecting them in the code view
* Improved error display performance by not expanding all of them at once
* Added home button next to search field for showing the project overview
* Added shortcut and menu item to show project overview
* Fixed Path boxes not sized correctly when switching back and forth in the project wizard
* Allow support for environment variables in project setup with syntax ${VARIABLE_NAME}
* Show quit button on license enter screen when no valid license was entered yet
* Allow color schemes to define text colors for code view highlights
* Added support for function pointers to analysis
* Added Windows system dlls to installer on Windows so users don't not need to install the redistributable package
* Changed style of out-of-date files to be followed by *
* Set Qt flag for high DPI scaling
* Check analyzed paths for existence in project wizard
* Inform the user about project changes and ask before reparsing the project
* Removed "simple setup" step in project wizard and added "lazy include search"-checkbox
* Edit the project settings by clicking the project name in the overview stats
* Added welcome message to Coati trial
* Defined new highlight colors for code view in all color schemes

#### 0.5.1.21
released 2016-03-16

* Fixed file logger using wrong directory for first log file
* Fixed crash when accessing files without read permission
* Center windows on top of main window
* Added font size limits
* Prevent network message handling without valid license
* Fixed type references in headers saved multiple times
* Improved context detection for template parameters
* Fixed Visual Studio plugin project loading
* Added delay for changing fontsize via mouse wheel
* Disabled config file warnings
* Fixed interrupting of analysis to show message in status bar
* Fixed children on namespace activation were bundled
* Fixed expanding file with errors showed all locations in red
* Show loader animation when loading project
* Removed high traffic log messages
* Fixed crash when opening newer project with older version of Coati
* Fixed project paths are now saved relative if defined relative
* Fixed project setup window sizes to adapt to content size
* Fixed linux package creating ~/.config if not there
* Show fatal errors in UI that stopped analysis
* Added compiler flags UI to advanced settings in project setup summary
* Added file extension UI to advanced settings in project setup summary
* Added graph zooming with Shift + mouse wheel or Shift + WS
* Added graph panning with WASD
* Improved project load and code view performance
* Improved analysis performance for large projects (10x as fast for 1.000.000 LoC)
* Added project setup for Compilation Databases

#### 0.5.0.4
released 2016-02-25

* Fixed preferences not saved
* Added link to documentation to Help menu
* Improved painting of icons in graph to be smoother
* Improved logging to alternate between 2 files of 1000 lines to decrease logfile sizes
* Improved analysis performance to take only 66% of the time
* Save user data in ~/Library/Application Support/Coati on Mac
* Improved style of ui windows
* Added Visual Studio solution parsing via project wizard or VS plugin
* Added project setup wizard for step-by-step project setup
* Added overview and error keywords to search view
* Show overview on project open with node bundles per type
* Added application move protection to reenter license key
* Improved log file names to be sorted chronologically
* Improved graph view node bundeling to bundle more
* Display scope name at bottom of code snippet
* Added .coatiproject file associations with icon for Mac and Windows
* Return to last scroll position when going back in code view
* Improved back & forward in code view
* Update error count while analysis
* Improved code view annotation painting performance
* Added web documentation
* Added trial target without analysis
* Added license key window and license checker
* Refresh project editing settings
* Added C and C++ standard setting to project
* Fixed cursor changing for resize and text interactions
* Added commandline option for project file
* Improved performance of token name saving
* Fixed number postions in graph
* Added shortcuts for font size resetting to View menu
* Added C support
* Added option to hide widget window title bars to View menu

#### 0.4.0
released 2016-01-07

* navigate back via backspace key
* navigate back&forward via mouse buttons
* renamed undo&redo to back&forward
* Fixed tooltip colors in dark color scheme
* Fixed style of function or method nodes with children
* Improved lambda handling in analysis
* Added help texts to project setup and preferences window
* Removed 'close window' action from menu
* Fixed constructors and destructors using return type 'void' withou source location
* Parse errors will be displayed as they appear during analysis
* Redesigned the about window
* Installing prerequisits in Windows Installer
* Added installer for Windows
* Introduced file endings .coatiproject and .coatidb for Coati's project files
* Removed Coati directory from Linux package
* Fixed graph font size was too small for Windows and Linux
* Show function and method signature in tooltip when hovering the graph node
* Added analysis for template parameter packs
* Fixed syntax highlighting to correctly identify strings, comments and block comments
* Fixed source locations of many declarations
* Coati shows a template's general definition when an implicit specialization is activated
* Added new color scheme called "bad_rainbow"
* Extended color schemes to allow more extensive use in the graph
* Coati now handles anonymous symbols when parsing files
* Improved handling of template arguments in analysis
* Coati can now handle inclass initialization of members
* Network ports for IDE communication can be defined in the ApplicationSettings.xml

#### 0.3.0
released 2015-12-14

* Saving all template arguments type uses
* Added recent projects menu option
* Focus all overlapping source locations in the code view
* Improved code view auto scroll to center the first active line in the middle
* Fixed search query disappeared when not token was found for it
* Fixed cursor was jumping at end of query when editing a search query
* Reworked code view highlighting
* Save comment locations and display them when right next to active symbol
* Fixed old data was still in new project window when creating another project
* Save macro scopes
* Save errors to storage and reactivate them via button in the status bar
* Fixed horizontal scroll in code view not activated while scrolling vertically
* Improved source locations of template scopes
* Save Coati version to sqlite and reparse project whenever the version changes
* Increased autocompletion performance by caching and limiting results to 100
* Added plugin for Sublime Text (2 & 3)
* Added plugin for Visual Studio (2012, 2013, 2015)
* Added communication interface from and to Coati via TCP sockets
* Added Tutorial project
