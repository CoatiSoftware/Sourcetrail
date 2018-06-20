### Changelog

#### 2018.2.77
released 2018-06-18

* C/C++: Fixed incomplete header file got changed to complete from correct translation unit, although it had errors
* Fixed show errors button click on incomplete file with no associated errors showed all errors
* C/C++: Ship clang compiler headers within package on macOS and Linux (issue #99)
* Graph: Fixed undoing one of multiple edge activations caused multiple recenterings of the graph
* Fixed links not clickable in source group type selection
* Explicitly clear all errors when refreshing incomplete files
* Graph: Fixed splitting anonymous namespace bundle increased group nesting by 1 every time
* Graph: Fixed typedef nodes not grouped within file group
* Code: Fixed snippet minimize click expanded snippet again
* Fixed changing of selected source group not possible if validity check fails.
* C/C++: Added C/C++ project setup from Code::Blocks (.cdb), which is also created for cmake projects in QtCreator
* C/C++: fixed endless loop in include path auto detection (issue #585, #470)
* C/C++: fixed crash in include validation in source group setup
* Fixed typo in update check (issue #586)
* C/C++: Added some compiler flags checks
* C/C++: Errors without location are now recorded for current main file


#### 2018.2.36
released 2018-05-04

* Graph: Fixed crash when clicking on namespace label
* Improved help dialog for compiler flags in project setup
* Fixed handling case insensitive filepath matches when activating a symbol via editor plugin
* Wrote cmake export compile command to description in source group setup
* Fixed macOS gatekeeper refusing to open app bundle (issue #568)
* Use custom radio button style on start indexing dialog to fix radio indicator not visible on old Windows systems
* Fixed indexed header paths in compilation database project change the processing order of includes (issue #571)
* Allow tooltips to be as wide as the window (issue #570)
* Graph: Improved edge routing in graph layout
* Graph: Only use base/derived bundles for inheritance


#### 2018.2.23
released 2018-04-24

* C/C++: removed check if header was already indexed to fix missing data due to different preprocessor conditions.
* Fixed fulltext search missing results for queries with many matches in the same file.
* Fixed crash on converting relative paths to canonical paths in project setup
* Also clear errors in non-indexed files when refreshing "updated and incomplete files" (issue #564)
* Updated about dialog to new company name
* Graph: Improved layout of aggregation edges
* Fixed race condition causing crashes when showing tooltips
* Code: Fixed switch of view mode didn't show active reference
* Fixed carriage return in line endings breaks screen search in code view
* Fixed logic to detect excluded files on project refresh
* Add "Copy" to duplicated source group's name
* Preferences: show 'default' in indexer threads combo instead of 0
* Fixed empty exclude path excludes everything
* C/C++: Improved indexer performance by skipping traversal of contents of non-indexed files
* Linux: Fixed error in Sourcetrail.sh script
* Search: Give children of matched symbols a lower score
* Search: Improved fulltext search index creation speed
* Don't show title bars of views by default
* Graph: Improved horizontal layout to avoid edges overlapping nodes
* Show versions of 3rd party dependencies in 3rd party dialog
* Updated help info for update check
* Graph: Group graph nodes by file or namespace (issues #171, #439, #522)
* Graph: Layout template specializations vertically and show them in inheritance depth graph
* Graph: Show symbols defined within file when activating file node (issue #268)
* UI: Updated to Qt 5.10.1
* C/C++: Updated to clang 6.0
* Graph: better contrast for non-indexed hatching colors
* Graph: group nodes with same base and derived nodes in inheritance depth graph (issue #459)
* Code: Lazy load code annotations for shorter display time (issue #389)
* Code: Improved snippet display time (issue #389)
* C++: fixed infinite recursion while solving c++ template parameter names (issue #553)
* Added close button to status view
* Improved style of tab bar in status view
* Added Show Errors button to title bar of incomplete files to show only errors related to certain file (issue #246)
* Added sorting to error table columns (issue #559)
* Added wildcards for excluded paths (issue #475)
* Option to accept EULA via commandline API
* Changed seats to users for Commercial License in EULA
* Graph: Improved vertical layout for override and inheritance edges
* Speed up preferences dialog loading time, by delaying font face loading
* Allow non-ASCII characters in symbol, full-text and on-screen search
* Allow non-ASCII characters in bookmarks
* Allow non-ASCII characters in file paths


#### 2018.1.19
released 2018-01-30

* Fixed crash on filling DirectoryListBox with no selected items (issue #544, #545)
* Graph: Improved type node contrast in bright color scheme
* Code: Fixed going back in history sometimes showed the wrong file when an edge was activated last
* Code: Clicking on active location will activate it again and show it's definition
* Graph: Don't keep expanded nodes expanded after using symbol search
* Fixed file picker didn't start in recent directory when exporting graph as image (issue #541)
* Code: Fixed file content shown multiple times when clicking title button of non-indexed file with errors
* Preferences: Only show latin fonts in the font face dropdown
* Code: Fixed gaps in line indicators for some font faces
* Windows: Show indexing progress in Windows task bar
* Linux: Added settings to change screen scaling to preferences (issue #518, #523)
* Allow for disabling certain Source Groups via Status checkbox in the "Edit Project" dialog
* C/C++: Fixed no indexed information saved for compilation database referencing certain compiler (issue #531)
* Increased history dropdown list size to 50 items
* Still allow project refresh if database file can't be loaded
* Removed focus outlines from push buttons
* Graph: Display warning when depth graph has more than 1000 nodes
* Code: Fixed screen search matches not always cleared in single file mode
* C/C++: Implemented handling Microsoft inline assembly statements
* C/C++: Added automatic include path detection to project setup
* Java: Fixed project setup from Maven not working for projects without sub-modules
* Windows: Changed default install directory to ".../Sourcetrail/"
* Windows: Changed Sourcetrail font color in installer image
* Windows: Fixed sourcetrail loader gif not showing up on some windows systems
* Custom style for dock widget title bars with different colors in each color scheme
* C++: Fixed bug where anonymous symbols were only partly named like the enclosing typedef
* C/C++: Fixed policy for recording elements inside macro arguments and bodies
* Allow users to ignore warning on non-existing paths in project setup
* Code: Switch to single file view when clicking snippet maximize
* Code: Show scrollbar at bottom of snippet list if bottom snippet is horizontally scrollable (issue #432)
* Code: Keep snippet title bar at top when scrolling down (issue #479)
* Fixed history dropdown opened again when clicking on button again
* Code: Fixed single file view cleared when refreshing
* Graph: Added "Exported from Sourcetrail" note to exported graph images
* C/C++: Don't store commandline string for each error to save memory
* Fixed delay when starting indexing until progress dialog shows up
* Group include validation results by file and add line breaks for readability
* Fixed location pickers used system root directory when empty
* Graph: Add "bidirectional" to tooltip of bidirectional aggregation edges
* Linux: Don't explicitely open terminal when running on Linux (issue #525)
* Fixed files and directories dialog to offer selection of files (issue #521)
* C++: Fixed cases where references to members were recorded as usage instead of call
* Fixed crash when Start Screen is closed while checking for update
* Search: Fixed selected text not deleted when pasting into search field (issue #527)
* C++: Record template variable specialization edges
* Code: Use different background color in snippet list to improve visual layering
* Don't force full project refresh when project settings changed (issue #493)
* Added indexing mode to reindex incomplete files (issue #496)
* Java: Allow using .aar files as Java project dependencies
* Java: fixed some Java exceptions and write Java exceptions to log file
* Java: Note architecture of Java version in preferences dialog
* Code: Show line indicators for fulltext search results and fixed matches at line start
* Graph: Update visible depth graph when changing depth level slider (issue #484)
* Always show all context menu actions, but disable unavailable ones
* Graph: Hide nodes and edges via context menu action or Alt + Click (issue #472)
* Graph: Added option to preferences to show/hide builtin types, default is hidden (issue #409)
* Improved style and color of scrollbars and main window separators
* Linux: Fixed icon sizes when scaling to highDPI fails
* Search: Fixed cursor disappearing when entering long string in the search field
* C++: Fixed parent node hierarchy for C++ template parameters of variable declarations
* Linux: Fixed syntax error in Sourcetrail.sh (issue #519)
* Graph: Improved exported image quality
* Graph: Fixed exported graph image were too large (issue #511)
* Fixed crashes due to double deleted Qt widgets
* Fixed crash when specifying an unknown text encoding in the preferences
* C++: Fixed retrieval of CXX template parameter declarations of parent
* C/C++: Fixed filepaths to always be canonical (issue #516)
* Graph: Fixed implicit nodes not auto-expanded when activated
* C/C++: Add filename info to static functions (issue #515)


#### 2017.4.46
released 2017-11-09

* C/C++: Add filename info to static global variables to avoid collision (issue #514)
* Allow duplicate indexing of source files if the indexer command differs
* Fixed single header files within indexed paths were not indexed (issue #513
* Status: Explain differences of errors and how to fix them in help message dialog (issue #501)
* C/C++: Add parent directories of source files in the compilation database automatically to indexed header paths
* C/C++: Give clang higher precedence in automatic global header search path detection
* C/C++: Added new clang 5.0.0 language standards to project setup dropdown box
* Graph: Show nodes with more than 20 children initially collapsed (issue #509)
* Show message box informing about shared memory problems before indexing (issue #508)
* Fixed files from symlinked directories within indexed directories were not indexed
* Start from correct directory for relative paths in project setup location pickers
* Java: Added all previous Java versions to language standard selection
* C++: Fixed recording template member specializations of methods as explicit nodes
* C++: Fixed template members variables sometimes recorded as functions with call edges
* C/C++: Fixed recorded location of function bodies defined inside macros
* C/C++: Skip recording macro usages in non-indexed files
* Search: Fixed focus and selection policy of search elements for Windows and Linux
* Graph: Fixed type usage edges not shown between class members and inner class types
* Graph: Fixed activation of bookmarked edge did not highlight edge in all situations
* Status: Fixed resize status view table columns were only resizeable in the title bar (issue #506)
* Added setting for text encoding to preferences (issue #500)
* Search: Fixed low contrast of search field cursor in dark schemes (issue #487)


#### 2017.4.20
released 2017-10-25

* Code: Fixed file state of snippets wrong after iterating references in single file view
* Search: Improved selection on focus to allow editing of search query (issue #483)
* Code: Fixed last line removed in full file view
* Fixed local symbol activation via plugin shows empty view
* Fixed handling of symbolic links when activated via plugin (issue #489)
* Code: Fixed crash due to double deletion of elements (issue #488)
* Code: Show shortcut 'Ctrl + Left Mouse' in code for 'Show in IDE' context menu action
* Improved names, texts and order in project setup
* macOS: Fixed start screen did not vanish when opening project with double click
* Added Non-Commercial use option and removed trial mode
* Linux: Moved install and uninstall scripts to top level directory in package
* C++: Fixed record rvalue references as "&&" instead of "&"
* C/C++: Added prefilling of indexed header paths for compilation database projects
* Moved automatic update check setting to preferences and enable by default
* Code: Added indexing of qualifier source locations and allow symbol activation by clicking qualifiers
* Linux: Enabled Qt highDPI scaling to fix UI issues on high resolution screens
* Fixed handling of non-indexed files in Graph and Code
* C/C++: Removed include check validataion for compilation database source groups
* Graph: Bundle nested anonymous namespaces into anonymous namespace bundle in namespace overview (issue #465)
* Search: Fixed vertical growth of search bar area when resizing window
* Visual Studio plugin: Lots off fixes, see changelog (https://github.com/CoatiSoftware/vs-sourcetrail/blob/master/CHANGELOG.md)
* Sublime Text plugin: Fixed plugin not working when installed via Package Control (issue #468)
* Windows: Added missing Qt dlls to save graph images as .jpg (issue #474)
* C/C++: Updated to Clang 5.0.0
* Code: Fixed annotations missing for full file snippets
* Java: Added project setup from Gradle (issue #379)
* Added "Find On-Screen" option with search bar at bottom of window for searching Graph and Code on-screen (issue #79)
* Use "Find On-Screen" with 'Ctrl + D' or '/'
* Graph: Added C++ template specialization and template member specialization edges
* Graph: Show implicit children of implicit nodes
* Code: Fixed syntax highlighting of inline comments
* Code: Added syntax highlighting rule for char literals
* C++: Improved recording of template member specializations
* Refresh only files where content actually changed


#### 2017.3.48
released 2017-09-07

* Java: migrated Java indexer from JavaSymbolSolver to Eclipse JDT
* Java: Use same node order in graph as in file
* Code: Added second line marker color to better highlight hovered and local symbols
* Added "Skip this Version" option to update dialog
* Search: replace template arguments with .. in non-indexed nodes to reduce clutter in search results
* Code: Fixed default text color not reset text was annotated in dark schemes
* Code: Fixed text color not correctly changed when annotation type changes
* Fixed history list activating wrong symbol after aggregation edge was active
* Added node type keywords to filter autocompletions or activate all nodes (issue #78)
* File dialog shows home directory instead of working directory for first file dialog (issue #448)
* Java: improved Maven timeout policy (issue #449)
* Fixed indexing results insertion starving when indexers are really fast because of other UI updates

#### 2017.3.26
released 2017-08-24

* Java: fixed indexer to continue working when a dependency path does not exist
* Changed default indexer thread count to 0, which uses optimal thread count
* Java: Changed default value for maven dependencies directory
* Java: improved indexing speed to be 2.5 time faster
* Added commandline API for indexing projects and configuring indexing related preferences (issue #383)
* Mention call/inheritance graphs in tutorial
* CXX: Save command line info of indexer command to errors and show within errors table (issue #351)
* CXX: Added cross-compilation UI to project setup (issue #370)
* Added update checker connecting to online API and checkbox to enable daily update checks on start screen.
* Java: Implemented interrupting AST visiting of Java indexer
* Java: Updated to JavaSymbolSolver 0.6.0.X and Javaparser 3.3.0
* CXX: Removed preprocessor only option, because of new include path validation
* CXX: Fixed uppercase CXX header file name and extensions saved in lowercase (issue #437)
* Code: When clicking source location with multiple tokens or local symbols show a list to select one of these
* Sort indexer commands for increased performance
* Added custom tooltipping to Code and Graph with clickable types of variables and signatures (issue #195)
* Java: fix issue where Maven project did not show any source files (issue #428)
* CXX: clean indexed header path selection for project setup from Compilation Database
* Added progress dialog for "show source files" button in project setup UI
* CXX: Added include path validation to project settings
* Search: Select text elements when clicking into search bar
* Search: Fixed crash when fulltext search yields no results
* Updated to Qt 5.9.1
* CXX: Added node type for "union"
* CXX: Merge anonymous types and the respective typedef (issue #189, #292)
* CXX: Updated to clang 4.0.1
* Added Menu option to show Start Window
* Don't show start window when opening project with double-click
* Show license type label in title bar: "Sourcetrail [trial, test, non-commercial]"
* Graph: Fixed class expand toggle shown in overview and namespace lists
* Graph: Show inheritance edges between parents of active symbol and other visible symbols (issue #167)
* Improved project loading performance
* Code: improved location retrieval and hover performance
* Show recent bookmarks in bookmarks menu (issue #414)
* Code: Fixed references and buttons when activating file in snippet mode
* Java: added auto-detection for JRE System Library
* Java: Prefill JRE system library path on first Sourcetrail launch
* macOS: fixed qt.conf for case-sensitive file system (issue #404)
* Improved database saving performance
* Improved database file clearing performance
* Improved contrasts of UI elements in bright color scheme
* Graph: Show zoom level as percent label next to zoom buttons
* Added Visual Studio Code plugin (issue #397)
* Added "Edit Project" button below error table
* Java: Maven fixes regarding JAVA_HOME variable (issue #405)

#### 2017.2
released 2017-06-21

* Fixed crash when using shortcuts containing letters with focus on the graph on macOS (issue #390)
* Improved Visual Studio plugin performance
* Updated End User License Agreement and have users accept on first run on macOS and Linux
* Fixed memory issues when using large Compilation Database files (issue #396)
* Updated to JavaParser and JavaSymbolSolver
* Fixed issue where black borders appeared around dialogs for some Linux window managers (issue #376)
* Use platform default window decorations for UI dialogs
* Code: Made snippet minimize/maximize buttons more interactive by showing hover states
* Code: improved code view loading performance
* Java: Record more "import not found" errors
* Support Visual Studio 2017 in Visual Studio Plugin (issue #381)
* Graph: increased creation speed by requesting less data
* macOS: Codesign .app bundle to avoid "from unidentified developer" warning
* Error: Use same error order in error table and code view
* Error: Only show first 1000 errors, click button in lower right of error table to show all (issue #385)
* Graph: use bezier edges when activating aggregation edge
* Send ping after changing plugin ports
* Added menu action to display EULA and force accepting on macOS
* Fixed use of environment variables in project setup paths
* Multi-language project setup via Source Groups setup UI (issue #230)
* C/C++: Fixed no files indexed when relative file paths are provided in Compilation Database (issue #388)
* C/C++: Display an error when loading a Compilation Database fails
* C/C++: Fixed source files within Compilation Database not indexed unless within Indexed Header Paths
* Fixed status message length limiting window resizing (issue #372)
* Match source file extensions case insensitive (issue #384)
* Fixed indexing progress file count in status bar and status view (issue #387)
* Windows: remove setup.exe from windows installer with all necessary .dll files
* Added history list button between back and forward button
* Added history menu showing activated symbols in chronologic order

#### 0.12.25
released 2017-05-16

* Added QtCreator plugin
* Graph: Added context menu action to create bookmark for node under mouse cursor (issue #373)
* Code: Fixed code view not scrolled to first fulltext search match in single file mode
* Set unrestricted permission for accessing shared memory to fix crash on Windows
* Improved setting descriptions in preferences dialog
* Java: Updated to JavaParser 3.2.0 and JavaSymbolSolver 0.5.3
* Graph: Fixed order of nodes is same as within first encountered header file
* Save timestamp after project indexing and show it on the overview screen
* Fixed use of correct name delimiters in indexer processes
* Fixed crash when running 32 and 64 bit versions in parallel
* Bookmark: Changed menu action to "Bookmark Active Symbol"
* Graph: Zooming on Ctrl + Mouse Wheel now (Shift + Mouse Wheel still working)
* Added dialog to warn about "verbose indexer logging" before indexing.
* Added multi process indexing for C/C++ projects to better handle crashes during indexing.
* Reduced file accesses during C/C++ indexing.
* Graph: Show call graphs, inheritance trees and include trees for active symbol. UI in the top left of the graph view for defining direction and depth. (issues #249 #337)
* Improved performance by running UI updates in parallel for each view
* Fixed project refresh not recognizing newly added files
* Show progress dialog when clicking refresh while computing files (issue #341)
* Added show files button to summary of compilation database project setup (issue #354)
* Show option to run only C/C++ preprocessor when indexing on indexing start dialog (issue #297)
* Added bookmarking feature for nodes and edges. Bookmarks can have categories and comments. Data gets stored in .srctrlbm next to project file. Shortcuts similar to web browsers. (issue #138)
* Mark files with errors and all files within a translation unit with fatal errors as incomplete (issue #358)
* Fixed compilation database projects using C++ source extensions and standard (issue #366)

#### 0.11.86
released 2017-04-12

* Terminating all running tasks before closing the application (issue #343)
* Use all available cores when setting indexer threads to 0 (issue #342)
* Added reset window layout option to view menu (issue #289)
* Fixed colorscheme gets refreshed when cancelling preferences dialog
* Log: Scroll to first line in errors table after indexing and after display
* Copy old ApplicationSettings.xml and window_settings.ini files from previous Coati install if available.
* New projects are created with .srctrlprj and .srctrldb endings, but still support old file extensions.
* Renamed application from Coati to Sourcetrail
* Fixed issues in tutorial project
* Graph: Fixed node with no children but qualifier shows expand button
* Enable undo/redo actions in context menu only when available
* Java: Updated to JavaParser 3.1.2 and JavaSymbolSolver 0.5.2.x
* Revised undo/redo stack to handle messages differently
* Java: Added support for project setup from Maven
* Windows: Hide console when running release builds
* Show license information on start screen
* Added ping support to plugins and display current connected editor/IDE in status bar.

#### 0.11.15
released 2017-03-01

* Don't accept richt text in 'enter license' dialog field (issue #207)
* Updated EULA to include Test License and better clarify other license types
* Fixed icons in project setup dialogs blurry on highDPI screens
* Added link to additional downloadable pre-indexed projects to trial start screen
* Fixed crash when saving preferences if they contain relative paths
* Convert all paths chosen by patch picker relative to project location in project setup
* Disabled name and location changing in project editing and removed project moving
* Split default file extensions for C++ and C to avoid wrong files getting indexed
* Deprecated 'Lazy Include Search', it's only visible to previously created projects (issue #335)
* Support relative paths and paths containing environment variables for compilation database path (issue #312)
* Removed 'Advanced Settings' section in project setup and show theses options in separate dialog (#293)
* Support environment variables containing multiple files in project setup paths (issue #283)
* Warn users when no 'Indexed Header Paths' were set in compilation database project (issue #254)
* Renamed "Project Paths" to "Indexed Paths" in project setup
* Renamed "Project File Location" to "Coati Project Location" in project setup (issue #234)
* Allow opening indexed projects in trial mode
* Record node type of non-indexed qualifiers (e.g. "std" in "std::string" as namespace)
* Log: Fixed error icon loaded for every row (issue #287)
* Graph: Fixed lambda are placed within white area to better see call edges
* Graph: Show non-indexed nodes within their parents instead of using namespace labeling
* Code: Disabled syntax highlighting for project description
* Graph: Fixed different font-faces causing graph nodes to expand to bigger cell amount
* Mac: Fixed wrong READMEs at some plugins (issue #315)
* Mac: Removed boost libraries and libLTO from Mac bundle
* Windows: Add Visual Studio compatibility flags to new project when Visual Studio include paths are present
* Renamed undefined nodes to non-indexed nodes for clarification
* Show progress in file clearing dialog
* Extended syntax highlighting for Java and distinguish by project type
* offer Windows 64bit build (issue #300)
* Fixed legacy Java 6 install necessary on MacOS (issue #280)
* Updated to Qt 5.8
* Updated to clang/llvm 3.9
* Added multithreading to Visual Studio plugin compilation database creation
* Graph: Use new list layouting for overview bundles and namespaces/packages
* Graph: Group nodes in list layout by starting character and allow jumping to group by pressing the key
* Graph: Use node specific styles for overview bundles
* Visual Studio plugin can now handle projects with non-native build tool for compilation database export
* Changed menu action 'Licenses' to '3rd Party Licenses'
* Added dialog with all shortcuts to Help menu
* Log: Show error in errors table when clicked in code view (issue #323)
* Code: Switched default mouse drag behavior to selection, panning on Shift + drag, and show suitable cursors
* Updated company address in about dialog
* Graph: Added on-screen zoom buttons
* Graph: Split aggregation edge when expanding nodes if possible
* Uncommented lines using the std library in TicTacToe sample
* Fixed issue with lambda return type (issue #318)
* Save builtin types as separate node type (issue #2)
* Remove unused included files after refreshing
* Java: Fixed packages that only contain packages to show up as non-indexed
* Added context-menu actions to copy node names, file paths and show files in containing directory (issue #320)
* Fixed crash when entering a single space in 'enter license' dialog (issue #277)
* Graph: Made edge lines thicker
* Fixed handling and storing of files with the same name
* Fixed environment variable detection within project paths for %VARIABLE_NAME% syntax
* Coati states whether it needs 32bit or 64bit JRE
* Java: Updated indexer to fix a lot of unsolved symbol issues
* Graph: Fixed layouter couldn't handle nodes with same name
* Graph: Fixed weird looking vertical aggregation edges to child nodes
* Code: Don't show stats for non loaded project
* Added option to disable UI animations to preferences
* Graph: Removed double line around graph view on Mac
* Graph: Fixed namespace label cut off at edge of screen
* Code: Added single file view mode
* Code: Added navigation bar with mode toggle and buttons to navigate references
* Graph: Removed underscores in edge hover names
* Infere some non-indexed node types by their edges


#### 0.10.0
released 2016-12-13

* Windows: Added runtime DLLs that could be missing on some systems
* Click message in status bar to open status window
* Graph: Sort nodes alphabetically within each section
* Graph: Aggregate type use edges to make referencing nodes appear collapsed
* Code: Improved performance through faster source location retrieval for files
* Code: Scroll active symbol definition to top of view if possible
* Added status tab to Log View showing status bar information
* Graph: Fixed graph animation used when clicking edges
* Graph: Disable bundling when files or macros are active
* Graph: Bundle importing files of active symbol in Java projects
* Improved speed of depending file search on refresh
* Search: Fixed crashes related to empty searches (issue #251)
* Search: Do fulltext search when no autocompletion match is available
* Added checkbox for full project refresh to start indexing dialog
* Disabled clang error limit by default to make sure all errors are shown
* Linux: Updated libs and added missing ones (issue #245)
* Updated icons for some graph nodes and project types
* Windows: Updated Visual Studio plugin UI
* Graph: Fixed view to not recenter on active node when clicking edge
* Fixed wrong errors displayed, wrong error counts and error view being wrongly shown
* Removed font zoom on CTRL + MouseWheel
* Graph: Display namespace information on left side of nodes, activate namespace by clicking label
* Improved file clearing performance
* Windows: Changed setup folder layout
* Added info screen for indexer crash investigation
* Code: Fixed display of links to missing symbols in project description
* Code: Fixed snippet size calculation causing a slowdown in certain cases
* Search: Skip intermediate autocompletions when typing fast
* Show dialog for last files inserted into .coatidb at end of indexing
* Integrated Visual Studio Plugin with Coati Project creation and removed previous Visual Studio Solution parser
* Improved C++ indexer coverage: using decls, using directives, auto keyword, lambda signatures, symbol references inside lambda captures, template argument related elements
* Reduced size of .coatidb file by up to 50% for large projects
* Fixed anonymouse symbol name conflicts (issue #241)
* Preferences: Added Indexer Logging option to print AST information during indexing
* Search: Added second line to search autocompletion list showing namespace, package or filepath
* Made cells readonly in errors table to prevent editing (issue #236)
* Renamed Log Window to Status window


#### 0.9.23
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
