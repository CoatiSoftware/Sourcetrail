### Changelog

#### 2021.4.19
released 2021-11-30

* UI: Removed website links and e-mail addresses that are no longer valid ([#1230](https://github.com/CoatiSoftware/Sourcetrail/issues/1230))
* Docs: Converted documentation to Markdown file to be accessible via GitHub
* UI: Removed update check ([#1226](https://github.com/CoatiSoftware/Sourcetrail/issues/1226))
* UI: fix extensive wait for short-lived processes ([#1227](https://github.com/CoatiSoftware/Sourcetrail/issues/1227))
* Java: support Gradle 7 when copying (test) compile libs ([#1203](https://github.com/CoatiSoftware/Sourcetrail/issues/1203))
* Code: Fix C/C++ keywords highlighted like functions ([#1197](https://github.com/CoatiSoftware/Sourcetrail/issues/1197))
* UI: Added solarized color schemes ([#1195](https://github.com/CoatiSoftware/Sourcetrail/issues/1195))

#### 2021.1.30
released 2021-03-15

* Java: Fixed Java path detection failures on linux ([#1138](https://github.com/CoatiSoftware/Sourcetrail/issues/1138))
* Java: Updated Java dependencies to add support for Java 13, 14 and 15 ([#1152](https://github.com/CoatiSoftware/Sourcetrail/issues/1152))
* C/C++: Fixed crash due to nullpointer access in indexer
* Python: Updated SourcetrailPythonIndexer to version 1.25.6 to fix issue when storing file content to database ([#1159](https://github.com/CoatiSoftware/Sourcetrail/issues/1159))
* Graph: Fixed multi-level inheritance edges when there are diamond structures ([#1142](https://github.com/CoatiSoftware/Sourcetrail/issues/1142))
* UI: hide news widget if no news are available ([#1160](https://github.com/CoatiSoftware/Sourcetrail/issues/1160))
* Windows: Ignore files where path exceeds character limit to avoid crash (issue [#1149](https://github.com/CoatiSoftware/Sourcetrail/issues/1149)) ([#1151](https://github.com/CoatiSoftware/Sourcetrail/issues/1151))
* Linux: Simplified symbolic link resolution in Linux Sourcetrail launch script ([#1134](https://github.com/CoatiSoftware/Sourcetrail/issues/1134))
* Docs: correct lots of spelling errors ([#1162](https://github.com/CoatiSoftware/Sourcetrail/issues/1162))
* Build: Switched from Qt to boost implementation of executeProcess ([#1145](https://github.com/CoatiSoftware/Sourcetrail/issues/1145))
* Build: make Windows CI build and test Java language support ([#1156](https://github.com/CoatiSoftware/Sourcetrail/issues/1156))
* Build: make Java tests run on Linux CI ([#1127](https://github.com/CoatiSoftware/Sourcetrail/issues/1127))

#### 2020.4.35
released 2021-01-06

* Build: Add compatibility layer for Qt versions ([#1118](https://github.com/CoatiSoftware/Sourcetrail/issues/1118))
* C/C++: Updated to LLVM/Clang 11.0.0 (issue [#1088](https://github.com/CoatiSoftware/Sourcetrail/issues/1088)) ([#1116](https://github.com/CoatiSoftware/Sourcetrail/issues/1116))
* C/C++: fix crash when using codeblocks project (issue [#1109](https://github.com/CoatiSoftware/Sourcetrail/issues/1109)) ([#1110](https://github.com/CoatiSoftware/Sourcetrail/issues/1110))
* Graph: add menu action for graph 'save as image' (issue [#426](https://github.com/CoatiSoftware/Sourcetrail/issues/426)) ([#1104](https://github.com/CoatiSoftware/Sourcetrail/issues/1104))
* Graph: remember graph zoom level on restart (issue [#801](https://github.com/CoatiSoftware/Sourcetrail/issues/801)) ([#1099](https://github.com/CoatiSoftware/Sourcetrail/issues/1099))
* Build: fix compiler issue when CMAKE_CXX_COMPILER_LAUNCHER was set to ccache (issue [#1081](https://github.com/CoatiSoftware/Sourcetrail/issues/1081)) ([#1093](https://github.com/CoatiSoftware/Sourcetrail/issues/1093))
* Python: update python indexer to v1.db25.p5 ([#1087](https://github.com/CoatiSoftware/Sourcetrail/issues/1087))
* Linux: explicitly add u+w permission when copying initial user data ([#1072](https://github.com/CoatiSoftware/Sourcetrail/issues/1072))
* Linux: Fixed duplicating Sourcetrail icon on run ([#1071](https://github.com/CoatiSoftware/Sourcetrail/issues/1071))
* Build: Qt 5.12 deprecation fixes ([#1003](https://github.com/CoatiSoftware/Sourcetrail/issues/1003))
* Linux: Prevent word splitting within arguments passed to Sourcetrail.sh ([#1057](https://github.com/CoatiSoftware/Sourcetrail/issues/1057))
* C/C++: Support linking with LLVM/Clang Dylib instead of individual components ([#1044](https://github.com/CoatiSoftware/Sourcetrail/issues/1044))

#### 2020.2.43
released 2020-06-29

* Docs: Explain Linux Tarball uninstall ([#1047](https://github.com/CoatiSoftware/Sourcetrail/issues/1047))
* Graph: Renamed 'aggregation' edge to 'bundled edges' for more clarity ([#1051](https://github.com/CoatiSoftware/Sourcetrail/issues/1051))
* Fixed error recording for multi-threaded custom command indexing ([#1049](https://github.com/CoatiSoftware/Sourcetrail/issues/1049))
* UI: Fixed crash when changing tabs during animation with focus on a child node ([#1046](https://github.com/CoatiSoftware/Sourcetrail/issues/1046))
* C/C++: Improved help message for source file extensions in project settings ([#1039](https://github.com/CoatiSoftware/Sourcetrail/issues/1039))
* Python: Updated to SourcetrailPythonIndexer v1.db25.p4 - Record override edges ([#1037](https://github.com/CoatiSoftware/Sourcetrail/issues/1037))
* Linux: Migrated Linux '.tar.gz' package creation from CPack to linuxdeployqt ([#1034](https://github.com/CoatiSoftware/Sourcetrail/issues/1034))
* UI: Save last filepicker location ([#802](https://github.com/CoatiSoftware/Sourcetrail/issues/802))
* C/C++: Updated to LLVM/Clang 10.0.0 ([#965](https://github.com/CoatiSoftware/Sourcetrail/issues/965))
* Java: Updated macOS Java Path detector and docs to current JDK file structure ([#1031](https://github.com/CoatiSoftware/Sourcetrail/issues/1031))
* Improved performance of excluded files and directories filtering ([#1030](https://github.com/CoatiSoftware/Sourcetrail/issues/1030))
* Fix corrupted display of non-ascii characters in about window ([#1013](https://github.com/CoatiSoftware/Sourcetrail/issues/1013))
* C/C++: Copy Clang compiler headers from Clang build dir via CMake ([#993](https://github.com/CoatiSoftware/Sourcetrail/issues/993))
* Improved error logging if exception occurred while loading project ([#1004](https://github.com/CoatiSoftware/Sourcetrail/issues/1004))
* Graph: Add context menu action 'copy to clipboard' ([#999](https://github.com/CoatiSoftware/Sourcetrail/issues/999))
* C/C++: Extend Compiler Flags help message ([#974](https://github.com/CoatiSoftware/Sourcetrail/issues/974))
* macOS: Create .dmg release package using HFS+ filesystem to be mountable on older macOS versions ([#619](https://github.com/CoatiSoftware/Sourcetrail/issues/619))
* Python: Respect "super()" in post processing ([#964](https://github.com/CoatiSoftware/Sourcetrail/issues/964))
* Less restrictions for FilePath::isValid check on project location ([#959](https://github.com/CoatiSoftware/Sourcetrail/issues/959))
* Refactored recent projects menu ([#956](https://github.com/CoatiSoftware/Sourcetrail/issues/956))
* macOs: Updated Info.plist missing values and removed deprecated keys
* Python: Respect class qualifier in method call for post processing ([#951](https://github.com/CoatiSoftware/Sourcetrail/issues/951))

#### 2020.1.117
released 2020-03-31

* C/C++: Fixed multiple main declarations not shown as separate symbols when declaring file has same name ([#950](https://github.com/CoatiSoftware/Sourcetrail/issues/950))
* Linux: Provide AppImage package based on linuxdeployqt (issue [#279](https://github.com/CoatiSoftware/Sourcetrail/issues/279)) ([#945](https://github.com/CoatiSoftware/Sourcetrail/issues/945))
* Make content of help dialogs selectable (issue [#805](https://github.com/CoatiSoftware/Sourcetrail/issues/935))
* Added keyboard controls to move and activate focus in graph and code views using WASD/HJKL/Arrows + Enter/E ([#935](https://github.com/CoatiSoftware/Sourcetrail/issues/935)) (issues [#486](https://github.com/CoatiSoftware/Sourcetrail/issues/486), [#327](https://github.com/CoatiSoftware/Sourcetrail/issues/327), [#214](https://github.com/CoatiSoftware/Sourcetrail/issues/214), [#210](https://github.com/CoatiSoftware/Sourcetrail/issues/210))
* Fixed a crash in shared memory use ([#912](https://github.com/CoatiSoftware/Sourcetrail/issues/912))
* Discard non-existing paths in automatic path detection ([#930](https://github.com/CoatiSoftware/Sourcetrail/issues/930))
* Python: Update python indexer to use jedi 0.16.0 ([#929](https://github.com/CoatiSoftware/Sourcetrail/issues/929))
* Fixed frequent deadlock after loading project from Start Screen (issue [#924](https://github.com/CoatiSoftware/Sourcetrail/issues/924))
* C/C++: Updated LLVM/Clang config headers to release 9.0.0 ([#922](https://github.com/CoatiSoftware/Sourcetrail/issues/922))
* Fix crash in update check on premature Qt object deletion ([#920](https://github.com/CoatiSoftware/Sourcetrail/issues/920))
* C/C++: Fixed an indexer crash ([#911](https://github.com/CoatiSoftware/Sourcetrail/issues/911))
* Code: fixed font size not changed in single file view (issue [#916](https://github.com/CoatiSoftware/Sourcetrail/issues/916))
* Mark required contents with asterisk in Source Group setup (issue [#723](https://github.com/CoatiSoftware/Sourcetrail/issues/723)) ([#914](https://github.com/CoatiSoftware/Sourcetrail/issues/914))
* Removed multiple dialog steps in Source Group setup (issue [#723](https://github.com/CoatiSoftware/Sourcetrail/issues/723)) ([#913](https://github.com/CoatiSoftware/Sourcetrail/issues/913))
* Handled non-writeable project location (issue [#735](https://github.com/CoatiSoftware/Sourcetrail/issues/735)) ([#906](https://github.com/CoatiSoftware/Sourcetrail/issues/906))
* Fixed error help dialog not showing up during indexing (issue [#740](https://github.com/CoatiSoftware/Sourcetrail/issues/740)) ([#904](https://github.com/CoatiSoftware/Sourcetrail/issues/904))
* Allow changing the log file path in preferences (issue [#156](https://github.com/CoatiSoftware/Sourcetrail/issues/156)) ([#900](https://github.com/CoatiSoftware/Sourcetrail/issues/900))
* Fix crash when project directory contains non-latin character (issue [#899](https://github.com/CoatiSoftware/Sourcetrail/issues/899)) ([#901](https://github.com/CoatiSoftware/Sourcetrail/issues/901))
* Graph: Fixed endless recursion on cyclic inheritance edges
* Add 'close tabs to the right' context menu action to tab bar (issue [#822](https://github.com/CoatiSoftware/Sourcetrail/issues/822)) ([#875](https://github.com/CoatiSoftware/Sourcetrail/issues/875))
* C/C++: Record calls to cxx destructor on delete keyword (issue [#829](https://github.com/CoatiSoftware/Sourcetrail/issues/829)) ([#863](https://github.com/CoatiSoftware/Sourcetrail/issues/863))
* Retry pre-filling mandatory empty path settings on startup ([#864](https://github.com/CoatiSoftware/Sourcetrail/issues/864))
* Improved handling of unloadable source group ([#862](https://github.com/CoatiSoftware/Sourcetrail/issues/862))

#### 2019.4.102
released 2019-12-20

* C/C++: Update selectable C and C++ language standards and cross-compilation flags ([#835](https://github.com/CoatiSoftware/Sourcetrail/issues/835))
* Java: Allow to specify custom Maven settings.xml file in project settings ([#794](https://github.com/CoatiSoftware/Sourcetrail/issues/794))
* Python: Accept python.exe located in the root directory of the environment ([#771](https://github.com/CoatiSoftware/Sourcetrail/issues/771))
* Java: Extend language support to Java 12 ([#752](https://github.com/CoatiSoftware/Sourcetrail/issues/752))
* Graph: Show active symbol definition in code when clicking on active node ([#836](https://github.com/CoatiSoftware/Sourcetrail/issues/836))
* C++: Fix recording template parameter as local symbol instead of qualifier ([#824](https://github.com/CoatiSoftware/Sourcetrail/issues/826))
* Python: Always perform deep index as default and added '--shallow' command line option ([#823](https://github.com/CoatiSoftware/Sourcetrail/issues/823))
* Code: Drag and drop selected source code from code view into other applications ([#707](https://github.com/CoatiSoftware/Sourcetrail/issues/707))
* Python: Allow using "unsafe" Python environment if explicitly specified by user ([#697](https://github.com/CoatiSoftware/Sourcetrail/issues/697), [#747](https://github.com/CoatiSoftware/Sourcetrail/issues/747))
* C/C++: Updated to LLVM/Clang 9.0.0 ([#799](https://github.com/CoatiSoftware/Sourcetrail/issues/799))
* Fix crash on application exit ([#789](https://github.com/CoatiSoftware/Sourcetrail/issues/789))
* Code: Fixed view cleared in single file mode when ui refresh is triggered ([#772](https://github.com/CoatiSoftware/Sourcetrail/issues/772))

#### 2019.4.61
released 2019-11-18

* Added GitHub and Patreon buttons to start window
* Added GNU General Public License
* Removed End User License Agreement and accept dialog from UI and Windows installer
* Removed Sonargraph based project setup
* Code: Add all source locations indexed for a file when displaying errors
* C/C++: Use gnu C and C++ standards as default in source groups on Linux platforms
* Python: add fast 'shallow indexing' mode (issue [#725](https://github.com/CoatiSoftware/Sourcetrail/issues/725))
* Fixed nested layout broken after relayout for maximum access node width
* Removed license key check
* Python: Improved performance of python post processing (by about factor 2)
* Improved performance of clearing data from re-indexed files to be instant
* Fixed making canonical filepaths when working directory contains symlink on Windows (issue [#733](https://github.com/CoatiSoftware/Sourcetrail/issues/733))
* C/C++: Update indexed headers list when editing the compilation database text box (issue [#724](https://github.com/CoatiSoftware/Sourcetrail/issues/724))
* Skip pre-indexing steps of disabled source groups (issue [#737](https://github.com/CoatiSoftware/Sourcetrail/issues/737))
* Fixed refresh cleared files of disabled source groups
* C++: Merged nodes for all possible expansions of a template parameter pack
* C++: Replaced template member specialization edge by template specialization edge
* C++: Record template default args as type usage
* C++: Record template parameter as local symbol
* Java: Use caching to speed up gradle and maven based projects
* Windows: Fixed styles missing in portable packages
* Add menu entry to close the current project (issue [#722](https://github.com/CoatiSoftware/Sourcetrail/issues/722))

#### 2019.3.46
released 2019-08-27

* Fixed errors were sometimes not clickable
* Windows: sign installer and executables with Authenticode certificate (issue [#704](https://github.com/CoatiSoftware/Sourcetrail/issues/704))
* Show proper error messages and progress when indexing with CLI (issue [#718](https://github.com/CoatiSoftware/Sourcetrail/issues/718))
* C/C++: only use precompiled headers for compilation database commands that specify the "-include-pch" flag (issue [#719](https://github.com/CoatiSoftware/Sourcetrail/issues/719))
* Python: Updated to SourcetrailPythonIndexer v1.db24.p2
* Graph: Fixed bezier edges stayed highlighted after getting unhovered
* Windows: Use modern Windows style for default ui elements on Windows 10
* Change mouse cursor to pointer on graph and code view contents (issue [#715](https://github.com/CoatiSoftware/Sourcetrail/issues/311))
* Java: Remove JVM maximum memory setting because it causes issues (issue [#694](https://github.com/CoatiSoftware/Sourcetrail/issues/311))
* Show overview instead of empty error screen after all indexing errors are gone
* C/C++: Allow indexing multiple indexer commands with different options for one source file in Compilation Database source group (issue [#717](https://github.com/CoatiSoftware/Sourcetrail/issues/311))
* Graph: Added custom trail dialog to show trail graph with specific origin and/or target, using node/edge filters (issue [#249](https://github.com/CoatiSoftware/Sourcetrail/issues/249))
* C/C++: Fixed removing duplicate compiler header directory not working (issue [#710](https://github.com/CoatiSoftware/Sourcetrail/issues/710))
* Java: Fixed maven source groups broken when default console output contains colors (issue [#705](https://github.com/CoatiSoftware/Sourcetrail/issues/705))
* Code: Added option to preferences to show relative file directory in code view title (issue [#556](https://github.com/CoatiSoftware/Sourcetrail/issues/556))
* C++: Fixed lambdas created additional type nodes
* C/C++: Initialize all clang targets to reduce errors for cross-target compilation
* C/C++: Added Precompiled Header Support to Empty and Compilation Database Source Groups (issue [#311](https://github.com/CoatiSoftware/Sourcetrail/issues/311))
* Search: Turn search element editable when pressing DELETE with cursor in front
* Graph: increased the tolerated movement distance during click to still count as click action

#### 2019.2.39
released 2019-05-28

* Python: Added example for Python environment to Source Group help text (issue [#696](https://github.com/CoatiSoftware/Sourcetrail/issues/696))
* Python: Improved post processing speed and show progress dialog
* Python: Added validity check for Python environment when entered in Source Group
* Code: Improved performance of location hovering in snippet mode
* Graph: Fixed undo after single edge activation within aggregation caused missing edges
* Don't set project state outdated if only Source Group name changed (issue [#700](https://github.com/CoatiSoftware/Sourcetrail/issues/700))
* C/C++: Don't store path to compiler headers in preferences, instead always add it before indexing
* Python: write output of indexer processes to log file
* Fixed crashes on application close (issue [#698](https://github.com/CoatiSoftware/Sourcetrail/issues/698))

#### 2019.2.25
released 2019-05-22

* Enable console and file logging by default
* Python: Sort indexer commands by file size
* Linux: Add path to OpenSSL config in Sourcetrail.sh script (issue [#659](https://github.com/CoatiSoftware/Sourcetrail/issues/659))
* Search: Refocus already focused search box on 'find symbol' shortcut to select content
* Renamed 'main' function in tutorial to 'start_tour', due to multiple main fix
* Python: Fixed macOS app bundle not using correct Python default environment
* Python: Pass verbose indexer logging flag and add output to log
* Fixed EULA not displayed in Windows and macOS builds
* Python: Added start screen project icon
* Python: Added sample project "tictactoe_py"
* Code: Fixed symbol definition shown instead of most recent reference when using back action
* Python: Added post processing to add ambiguous edges for unsolved symbols with matching name
* Fixed old test license format not loaded (issue [#691](https://github.com/CoatiSoftware/Sourcetrail/issues/691))
* C/C++: Added intermediate fix to handle multiple definitions of main() in C/C++ (issue [#233](https://github.com/CoatiSoftware/Sourcetrail/issues/233))
* Graph: Added "Show Definition in IDE" action to context menu (issue [#687](https://github.com/CoatiSoftware/Sourcetrail/issues/687))
* Increased contrasts in color scheme 'bright'
* Automatically delete log files older than a month (issue [#676](https://github.com/CoatiSoftware/Sourcetrail/issues/676))
* Code: renamed 'relations' to 'references' in overview stats
* Fixed bug that caused the UI to be blocked after aborting indexing on "partial clearing not possible" warning
* Start automatic update checks on second day of use
* Graph: Show multi-level inheritance edge in graph legend (issue [#688](https://github.com/CoatiSoftware/Sourcetrail/issues/688))
* C/C++: Updated to LLVM/Clang 8.0.1
* Graph: Use multicolumn layout for nodes with many children if none of the children has edges
* Python: Added Python 2 & 3 support based on [SourcetrailPythonIndexer](https://github.com/CoatiSoftware/SourcetrailPythonIndexer) (issue [#51](https://github.com/CoatiSoftware/Sourcetrail/issues/51))
* Python: Added source group type "Empty Python" to project setup
* Search: Edit last search element on backspace instead of deleting (issue [#681](https://github.com/CoatiSoftware/Sourcetrail/issues/681))
* EULA: added support for lifelong licenses
* Improved text search performance (issue [#680](https://github.com/CoatiSoftware/Sourcetrail/issues/680))
* Windows: Fixed bug that caused texts within tooltips sometimes to be cut off
* Fixed crash when closing license window at first start
* Automatically replace invalid characters in project name during project setup with "_" (issue [#671](https://github.com/CoatiSoftware/Sourcetrail/issues/671))
* Windows: Added syntax highlighting rules to portable package (issue [#672](https://github.com/CoatiSoftware/Sourcetrail/issues/672))

#### 2019.1.11
released 2019-02-05

* Mention Compilation Database can be generated by QtCreator since version 4.8
* Code: Fixed qualifier locations were used in reference navigation
* Fixed error count in status bar not restored after discarding interrupted index
* Graph: Fixed layout for inheritance chains
* Added syntax highlighting rules for Python
* Rerun cxx global header path detection if skipped due to error in previous release (issue [#664](https://github.com/CoatiSoftware/Sourcetrail/issues/664))
* Fixed status error may not show up if custom command fails on Windows
* Graph: Fixed right click activates edge on Windows but should not
* C/C++: Always add compiler header path to global include paths and warn user about possible problems
* Added option to execute custom indexer commands in parallel
* Graph: Align layout in middle column at middle gap to avoid edges going through nodes
* Code: Fixed line numbers disappearing on hover on Linux
* Fixed initialization of header search paths on first start of Sourcetrail
* Project Setup: Highlight project setup from Compilation Database as recommended for C/C++ projects
* Project Setup: Double-click in project setup list boxes adds new line
* Graph: Only bundle as "importing files" when "import" edge is present
* Graph: Prevent flickering of edges when going back after multiple edge activations
* Fixed relative paths not working in Code::Blocks project files (issue [#663](https://github.com/CoatiSoftware/Sourcetrail/issues/663))
* Graph: Remove virtual nodes in depth-graph edges when moving nodes (issue [#639](https://github.com/CoatiSoftware/Sourcetrail/issues/639))
* Added exception handling for HTTP request exceptions in update checker (issue [#659](https://github.com/CoatiSoftware/Sourcetrail/issues/659))
* Clear screen-search matches when the active symbol in the view changes (issue [#660](https://github.com/CoatiSoftware/Sourcetrail/issues/660))
* Wrap lines in status and error table (issue [#658](https://github.com/CoatiSoftware/Sourcetrail/issues/658))
* Graph: Elide name of active node if it has more than 100 characters (issue [#652](https://github.com/CoatiSoftware/Sourcetrail/issues/652))
* Added link to changelog to help menu (issue [#653](https://github.com/CoatiSoftware/Sourcetrail/issues/653))

#### 2018.4.45
released 2018-12-18

* Code: Fixed active code view mode button can be unchecked and lose it's visual highlight
* C/C++: record Clang invocation errors as errors when nothing else is recorded
* Show Database Version in About and Custom Command Source Group description
* Graph: Show overview bundles for non-indexed or symbol types if no others available
* Code: Extract syntax highlighting rules to .rules files, to allow adding highlighting rules for new languages
* C/C++: Still log the first 20.000 characters of the Clang invocation if verbose indexer logging is disabled
* Added Source Group Custom Command to use with SourcetrailDB binaries
* Fixed indexed source files not updated when clicking "show files" after other changes in project setup
* Linux: Fixed issue in sourcetrail.sh script (issue [#638](https://github.com/CoatiSoftware/Sourcetrail/issues/638))
* Inspect errors during indexing (issue [#235](https://github.com/CoatiSoftware/Sourcetrail/issues/235))
* C/C++: Fixed header path selection in Source Group setup also preselects non existing paths (issue [#640](https://github.com/CoatiSoftware/Sourcetrail/issues/640))
* Graph: Fixed graph node centering shows right end when node has greater width than view
* Code: Fixed multiple multiline comments within one line not correctly highlighted
* Code: Changed snippet style, removing per file frame and minimize/snippet buttons
* C/C++: Fixed shrinking shared memory below initial size (issue [#636](https://github.com/CoatiSoftware/Sourcetrail/issues/636))
* Fixed indexers still waiting until less storages are queued, although indexing was already interrupted (issue [#634](https://github.com/CoatiSoftware/Sourcetrail/issues/634))
* Fixed interrupting indexing while dialog hidden
* Graph: Fixed nodes and edges stay highlighted after hovering in code
* C/C++: Reduced shared memory consumption during indexing when indexers are faster than db insertion

#### 2018.4.8
released 2018-11-13

* Fixed search activates wrong symbol if multiple potential matches share the same "long" symbol name
* C++: Fixed infinite iteration when solving parent name of C++ template arguments with multiple parent contexts
* C++: Only log Clang tool invocation info when enabling "verbose indexer logging"
* Running shrink on shared memory that is used to transfer data between processes during indexing from time to time
* Added tabs UI to top of main window (issue [#215](https://github.com/CoatiSoftware/Sourcetrail/issues/215))
* Linux: Fixed no write permission to ~/.config/sourcetrail after installation (issue [#625](https://github.com/CoatiSoftware/Sourcetrail/issues/625), [#626](https://github.com/CoatiSoftware/Sourcetrail/issues/626))
* C++: Added type usage edges from parent context for types that occur as template arguments
* C++: Skip recording of implicit local symbols
* Java: Show explicit error message if no Java path provided in preferences
* C/C++: Updated to LLVM/Clang 7.0.0
* Added news box to start screen, showing news returned with daily update check
* Reduced memory usage during project loading by iterating over database entries instead of retrieving in containers
* Code: Added copy action to context menu for code view
* Code: Removed accidental leading spaces in overview project description
* Fixed history menu to show global activation history chronologically
* Fixed handling of multiple active symbols broken when going back in history
* Removed std headers from tutorial
* Show hidden indexing dialog when clicking refresh while indexing is in progress
* Code: Fixed unhovering an active error in code view removed the "active-highlight"
* Fixed shared memory garbage collector mutex unlocking policy when stuck
* Code: Fixed crash on code view scope expansion
* Rewrote indexer data recording API to rely on IDs instead of strings for faster indexer performance
* Removed unnecessary SQLite indexes for smaller database size
* JAVA: display multi-line maven error messages (issue [#622](https://github.com/CoatiSoftware/Sourcetrail/issues/622))
* Improved indexer storing performance by splitting insertion into multiples of pre-compiled SQLite batch insert statements
* Reduced copies during indexer data recording
* Improved cache lookups in indexer data recording
* Windows: fixed line breaking issues with carriage returns
* C/C++: Reduced waiting time when interrupting indexer processes
* C/C++: Improved name handling and caching performance
* Fixed edge cases in determining files to update/clear when re-indexing
* C/C++: record and display indexer configuration issues (issue [#618](https://github.com/CoatiSoftware/Sourcetrail/issues/618))
* Windows: Log success of Visual Studio path detector instead of printing to console
* Generate and pass index file commands to indexers one by one to reduce memory consumption
* C/C++: Query files from compilation database one by one to reduce memory consumption
* Java: changed type_argument edges to originate in the generic type or method
* Optimized storage in-memory caches to reduce memory use during indexing

#### 2018.3.55
released 2018-08-31

* C/C++: Fixed crash on logging Clang invocation info with flags that throw errors (issue [#617](https://github.com/CoatiSoftware/Sourcetrail/issues/617))
* Plugin: Move window to the front when sending symbol via plugin (issue [#605](https://github.com/CoatiSoftware/Sourcetrail/issues/605))
* Java: Fixed crash on Windows if JVM tries to allocate too much memory
* Java: Fixed loading dialogs shown in main window instead of project setup dialog for setup with gradle
* Graph: Display bundle node count as pill instead of circle
* Fixed crash that had a chance to occur when showing source files of cdb/cbp/sonargraph project during setup
* Fixed source extensions not converted to lowercase (issue [#614](https://github.com/CoatiSoftware/Sourcetrail/issues/614))
* C/C++: fixed line of errors wrong when appearing after #line directives (issue [#610](https://github.com/CoatiSoftware/Sourcetrail/issues/610))
* Graph: Clear graph contents on overview (issue [#609](https://github.com/CoatiSoftware/Sourcetrail/issues/609))
* Windows: Fixed policy for finding Windows user data folder (issue [#612](https://github.com/CoatiSoftware/Sourcetrail/issues/612))
* C/C++: Fixed recording of multiple cxx anonymous namespaces within the same file
* C/C++: Fixed files within indexed directories always refreshed in empty C/C++ source groups
* Code: Don't show list of overridden methods when clicking overriding method
* Code: Fixed local reference navigation not shown for edges when file content was not cached
* Fixed crash related to update check when app was opened with a project
* Tooltip: Fixed repeated signature name qualification and line breaks
* Tooltip: Fixed app crash when signature location was wrongly recorded
* Java: always record 'java.lang' as package
* Java: Record all parent packages of an import declaration as packages
* Code: Fixed code annotations missing in snippet mode for files with /r/n line endings
* Java: Added support for annotations
* C/C++: log Clang invocation with implicit and derived arguments
* Indexer: Reduced copies in handling intermediate storage representation
* Indexer: Don't shuffle file order for indexing
* Indexer: Improved storing performance by optimizing insertion statements and indices

#### 2018.3.13
released 2018-08-06

* Code: Fixed text search locations sometimes disappearing when expanding scope of code snippets
* Graph: Added Expand/Collapse Node actions to graph context menu
* Indexer: Improved storing performance
* Warn user after indexing if project index is used in multiple processes and cannot be updated
* Search: Fixed text search sometimes showed wrong results for queries with non-letter characters
* Graph: Improved graph node color contrasts
* C/C++: Improved file path caching for faster indexing times
* C/C++: Fixed click on macro reference activated first of multiple symbols instead of showing tooltip list
* C/C++: Fixed file logging broken when indexing a compilation database project in single process
* Prefill system specific paths in application settings only once
* Added check if entered project name is a valid file name
* Windows: Fixed names of available header path detectors
* Graph: Show graph legend via 'legend' command or '?' button in the lower right corner (issue [#308](https://github.com/CoatiSoftware/Sourcetrail/issues/308), [#540](https://github.com/CoatiSoftware/Sourcetrail/issues/540))
* C/C++: Ship clang compiler headers also for Windows
* Code: Added on-demand local reference navigation to code view navigation bar (issue [#453](https://github.com/CoatiSoftware/Sourcetrail/issues/453), [#538](https://github.com/CoatiSoftware/Sourcetrail/issues/538))
* Improved button texts for "reindexing required" dialog
* Code: Scroll code horizontally to active source location if out of view
* Graph: Added show definition context menu action/shortcut to show definition of any node in the code (issue [#83](https://github.com/CoatiSoftware/Sourcetrail/issues/83))
* Improved project loading speed
* Create project directory during project setup if not existing
* Tooltip: Show function/method signatures exactly as declared in source file
* Search: Improved autocompletion scoring to process more symbols
* Search: Fixed autocompletions fail with wide characters (issue [#598](https://github.com/CoatiSoftware/Sourcetrail/issues/598))
* Windows: Show error text when opening external .dll fails
* C/C++: Highlight opening and closing braces in code when hovering either one (issue [#12](https://github.com/CoatiSoftware/Sourcetrail/issues/12))
* macOS: Fixed start screen not disappearing when launching Sourcetrail by opening a project
* Graph: Fixed restoring of scroll position in graph on undo broken
* Graph: Hide children with type use edges to parent if parent is active
* Bookmark: Fixed bookmark creator dialog header not visible
* Graph: Show parameters for functions/methods with same name (issue [#259](https://github.com/CoatiSoftware/Sourcetrail/issues/259))
* Graph: exporting graph as .svg (issue [#596](https://github.com/CoatiSoftware/Sourcetrail/issues/596))
* Show indexing progress bar in status bar
* Keep browsing old project state while indexing (issue [#175](https://github.com/CoatiSoftware/Sourcetrail/issues/175))
* Java: Added support for Java 9 & 10
* Added project setup from Sonargraph project for C, C++ and Java.
* Migrate old ".coatiproject" settings to new ".srctrlprj" extension when loading project
* Allow discarding unfinished index when aborting indexing
* Fixed partly indexed project can't be opened after closing while indexing (issue [#594](https://github.com/CoatiSoftware/Sourcetrail/issues/594))
* C/C++: Show translation unit of indexing errors
* Fixed activating history item from menu showed wrong symbol (issue [#572](https://github.com/CoatiSoftware/Sourcetrail/issues/572))
* Fixed all errors shown when only showing errors for certain file and changing error filter (issue [#577](https://github.com/CoatiSoftware/Sourcetrail/issues/577))
* Added pagination to project setup dialogs
* C/C++: Make complete file incomplete if it has errors in later translation units


#### 2018.2.77
released 2018-06-18

* C/C++: Fixed incomplete header file got changed to complete from correct translation unit, although it had errors
* Fixed show errors button click on incomplete file with no associated errors showed all errors
* C/C++: Ship clang compiler headers within package on macOS and Linux (issue [#99](https://github.com/CoatiSoftware/Sourcetrail/issues/99))
* Graph: Fixed undoing one of multiple edge activations caused multiple recenterings of the graph
* Fixed links not clickable in source group type selection
* Explicitly clear all errors when refreshing incomplete files
* Graph: Fixed splitting anonymous namespace bundle increased group nesting by 1 every time
* Graph: Fixed typedef nodes not grouped within file group
* Code: Fixed snippet minimize click expanded snippet again
* Fixed changing of selected source group not possible if validity check fails.
* C/C++: Added C/C++ project setup from Code::Blocks (.cdb), which is also created for cmake projects in QtCreator
* C/C++: fixed endless loop in include path auto detection (issue [#585](https://github.com/CoatiSoftware/Sourcetrail/issues/585), [#470](https://github.com/CoatiSoftware/Sourcetrail/issues/470))
* C/C++: fixed crash in include validation in source group setup
* Fixed typo in update check (issue [#586](https://github.com/CoatiSoftware/Sourcetrail/issues/586))
* C/C++: Added some compiler flags checks
* C/C++: Errors without location are now recorded for current main file

#### 2018.2.36
released 2018-05-04

* Graph: Fixed crash when clicking on namespace label
* Improved help dialog for compiler flags in project setup
* Fixed handling case insensitive filepath matches when activating a symbol via editor plugin
* Wrote cmake export compile command to description in source group setup
* Fixed macOS gatekeeper refusing to open app bundle (issue [#568](https://github.com/CoatiSoftware/Sourcetrail/issues/568))
* Use custom radio button style on start indexing dialog to fix radio indicator not visible on old Windows systems
* Fixed indexed header paths in compilation database project change the processing order of includes (issue [#571](https://github.com/CoatiSoftware/Sourcetrail/issues/571))
* Allow tooltips to be as wide as the window (issue [#570](https://github.com/CoatiSoftware/Sourcetrail/issues/570))
* Graph: Improved edge routing in graph layout
* Graph: Only use base/derived bundles for inheritance

#### 2018.2.23
released 2018-04-24

* C/C++: removed check if header was already indexed to fix missing data due to different preprocessor conditions.
* Fixed fulltext search missing results for queries with many matches in the same file.
* Fixed crash on converting relative paths to canonical paths in project setup
* Also clear errors in non-indexed files when refreshing "updated and incomplete files" (issue [#564](https://github.com/CoatiSoftware/Sourcetrail/issues/564))
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
* Graph: Group graph nodes by file or namespace (issues [#171](https://github.com/CoatiSoftware/Sourcetrail/issues/171), [#439](https://github.com/CoatiSoftware/Sourcetrail/issues/439), [#522](https://github.com/CoatiSoftware/Sourcetrail/issues/522))
* Graph: Layout template specializations vertically and show them in inheritance depth graph
* Graph: Show symbols defined within file when activating file node (issue [#268](https://github.com/CoatiSoftware/Sourcetrail/issues/268))
* UI: Updated to Qt 5.10.1
* C/C++: Updated to clang 6.0
* Graph: better contrast for non-indexed hatching colors
* Graph: group nodes with same base and derived nodes in inheritance depth graph (issue [#459](https://github.com/CoatiSoftware/Sourcetrail/issues/459))
* Code: Lazy load code annotations for shorter display time (issue [#389](https://github.com/CoatiSoftware/Sourcetrail/issues/389))
* Code: Improved snippet display time (issue [#389](https://github.com/CoatiSoftware/Sourcetrail/issues/389))
* C++: fixed infinite recursion while solving c++ template parameter names (issue [#553](https://github.com/CoatiSoftware/Sourcetrail/issues/544))
* Added close button to status view
* Improved style of tab bar in status view
* Added Show Errors button to title bar of incomplete files to show only errors related to certain file (issue [#246](https://github.com/CoatiSoftware/Sourcetrail/issues/246))
* Added sorting to error table columns (issue [#559](https://github.com/CoatiSoftware/Sourcetrail/issues/559))
* Added wildcards for excluded paths (issue [#475](https://github.com/CoatiSoftware/Sourcetrail/issues/475))
* Option to accept EULA via commandline API
* Changed seats to users for Commercial License in EULA
* Graph: Improved vertical layout for override and inheritance edges
* Speed up preferences dialog loading time, by delaying font face loading
* Allow non-ASCII characters in symbol, full-text and on-screen search
* Allow non-ASCII characters in bookmarks
* Allow non-ASCII characters in file paths

#### 2018.1.19
released 2018-01-30

* Fixed crash on filling DirectoryListBox with no selected items (issue [#544](https://github.com/CoatiSoftware/Sourcetrail/issues/544), [#545](https://github.com/CoatiSoftware/Sourcetrail/issues/545))
* Graph: Improved type node contrast in bright color scheme
* Code: Fixed going back in history sometimes showed the wrong file when an edge was activated last
* Code: Clicking on active location will activate it again and show it's definition
* Graph: Don't keep expanded nodes expanded after using symbol search
* Fixed file picker didn't start in recent directory when exporting graph as image (issue [#541](https://github.com/CoatiSoftware/Sourcetrail/issues/541))
* Code: Fixed file content shown multiple times when clicking title button of non-indexed file with errors
* Preferences: Only show latin fonts in the font face dropdown
* Code: Fixed gaps in line indicators for some font faces
* Windows: Show indexing progress in Windows task bar
* Linux: Added settings to change screen scaling to preferences (issue [#518](https://github.com/CoatiSoftware/Sourcetrail/issues/518), [#523](https://github.com/CoatiSoftware/Sourcetrail/issues/523))
* Allow for disabling certain Source Groups via Status checkbox in the "Edit Project" dialog
* C/C++: Fixed no indexed information saved for compilation database referencing certain compiler (issue [#531](https://github.com/CoatiSoftware/Sourcetrail/issues/531))
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
* Code: Show scrollbar at bottom of snippet list if bottom snippet is horizontally scrollable (issue [#432](https://github.com/CoatiSoftware/Sourcetrail/issues/432))
* Code: Keep snippet title bar at top when scrolling down (issue [#479](https://github.com/CoatiSoftware/Sourcetrail/issues/479))
* Fixed history dropdown opened again when clicking on button again
* Code: Fixed single file view cleared when refreshing
* Graph: Added "Exported from Sourcetrail" note to exported graph images
* C/C++: Don't store commandline string for each error to save memory
* Fixed delay when starting indexing until progress dialog shows up
* Group include validation results by file and add line breaks for readability
* Fixed location pickers used system root directory when empty
* Graph: Add "bidirectional" to tooltip of bidirectional aggregation edges
* Linux: Don't explicitly open terminal when running on Linux (issue [#525](https://github.com/CoatiSoftware/Sourcetrail/issues/525))
* Fixed files and directories dialog to offer selection of files (issue [#521](https://github.com/CoatiSoftware/Sourcetrail/issues/521))
* C++: Fixed cases where references to members were recorded as usage instead of call
* Fixed crash when Start Screen is closed while checking for update
* Search: Fixed selected text not deleted when pasting into search field (issue [#527](https://github.com/CoatiSoftware/Sourcetrail/issues/527))
* C++: Record template variable specialization edges
* Code: Use different background color in snippet list to improve visual layering
* Don't force full project refresh when project settings changed (issue [#493](https://github.com/CoatiSoftware/Sourcetrail/issues/493))
* Added indexing mode to reindex incomplete files (issue [#496](https://github.com/CoatiSoftware/Sourcetrail/issues/496))
* Java: Allow using .aar files as Java project dependencies
* Java: fixed some Java exceptions and write Java exceptions to log file
* Java: Note architecture of Java version in preferences dialog
* Code: Show line indicators for fulltext search results and fixed matches at line start
* Graph: Update visible depth graph when changing depth level slider (issue [#484](https://github.com/CoatiSoftware/Sourcetrail/issues/484))
* Always show all context menu actions, but disable unavailable ones
* Graph: Hide nodes and edges via context menu action or Alt + Click (issue [#472](https://github.com/CoatiSoftware/Sourcetrail/issues/472))
* Graph: Added option to preferences to show/hide builtin types, default is hidden (issue [#409](https://github.com/CoatiSoftware/Sourcetrail/issues/409))
* Improved style and color of scrollbars and main window separators
* Linux: Fixed icon sizes when scaling to highDPI fails
* Search: Fixed cursor disappearing when entering long string in the search field
* C++: Fixed parent node hierarchy for C++ template parameters of variable declarations
* Linux: Fixed syntax error in Sourcetrail.sh (issue [#519](https://github.com/CoatiSoftware/Sourcetrail/issues/519))
* Graph: Improved exported image quality
* Graph: Fixed exported graph image were too large (issue [#511](https://github.com/CoatiSoftware/Sourcetrail/issues/511))
* Fixed crashes due to double deleted Qt widgets
* Fixed crash when specifying an unknown text encoding in the preferences
* C++: Fixed retrieval of CXX template parameter declarations of parent
* C/C++: Fixed filepaths to always be canonical (issue [#516](https://github.com/CoatiSoftware/Sourcetrail/issues/516))
* Graph: Fixed implicit nodes not auto-expanded when activated
* C/C++: Add filename info to static functions (issue [#515](https://github.com/CoatiSoftware/Sourcetrail/issues/515))

#### 2017.4.46
released 2017-11-09

* C/C++: Add filename info to static global variables to avoid collision (issue [#514](https://github.com/CoatiSoftware/Sourcetrail/issues/514))
* Allow duplicate indexing of source files if the indexer command differs
* Fixed single header files within indexed paths were not indexed (issue [#513](https://github.com/CoatiSoftware/Sourcetrail/issues/513))
* Status: Explain differences of errors and how to fix them in help message dialog (issue [#501](https://github.com/CoatiSoftware/Sourcetrail/issues/501))
* C/C++: Add parent directories of source files in the compilation database automatically to indexed header paths
* C/C++: Give clang higher precedence in automatic global header search path detection
* C/C++: Added new clang 5.0.0 language standards to project setup dropdown box
* Graph: Show nodes with more than 20 children initially collapsed (issue [#509](https://github.com/CoatiSoftware/Sourcetrail/issues/509))
* Show message box informing about shared memory problems before indexing (issue [#508](https://github.com/CoatiSoftware/Sourcetrail/issues/508))
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
* Status: Fixed resize status view table columns were only resizeable in the title bar (issue [#506](https://github.com/CoatiSoftware/Sourcetrail/issues/506))
* Added setting for text encoding to preferences (issue [#500](https://github.com/CoatiSoftware/Sourcetrail/issues/500))
* Search: Fixed low contrast of search field cursor in dark schemes (issue [#487](https://github.com/CoatiSoftware/Sourcetrail/issues/487))

#### 2017.4.20
released 2017-10-25

* Code: Fixed file state of snippets wrong after iterating references in single file view
* Search: Improved selection on focus to allow editing of search query (issue [#483](https://github.com/CoatiSoftware/Sourcetrail/issues/483))
* Code: Fixed last line removed in full file view
* Fixed local symbol activation via plugin shows empty view
* Fixed handling of symbolic links when activated via plugin (issue [#489](https://github.com/CoatiSoftware/Sourcetrail/issues/489))
* Code: Fixed crash due to double deletion of elements (issue [#488](https://github.com/CoatiSoftware/Sourcetrail/issues/488))
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
* C/C++: Removed include check validation for compilation database source groups
* Graph: Bundle nested anonymous namespaces into anonymous namespace bundle in namespace overview (issue [#465](https://github.com/CoatiSoftware/Sourcetrail/issues/465))
* Search: Fixed vertical growth of search bar area when resizing window
* Visual Studio plugin: Lots off fixes, see changelog (https://github.com/CoatiSoftware/vs-sourcetrail/blob/master/CHANGELOG.md)
* Sublime Text plugin: Fixed plugin not working when installed via Package Control (issue #468)
* Windows: Added missing Qt dlls to save graph images as .jpg (issue [#474](https://github.com/CoatiSoftware/Sourcetrail/issues/474))
* C/C++: Updated to Clang 5.0.0
* Code: Fixed annotations missing for full file snippets
* Java: Added project setup from Gradle (issue [#379](https://github.com/CoatiSoftware/Sourcetrail/issues/379))
* Added "Find On-Screen" option with search bar at bottom of window for searching Graph and Code on-screen (issue [#79](https://github.com/CoatiSoftware/Sourcetrail/issues/79))
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
* Search: replace template arguments with <..> in non-indexed nodes to reduce clutter in search results
* Code: Fixed default text color not reset text was annotated in dark schemes
* Code: Fixed text color not correctly changed when annotation type changes
* Fixed history list activating wrong symbol after aggregation edge was active
* Added node type keywords to filter autocompletions or activate all nodes (issue [#78](https://github.com/CoatiSoftware/Sourcetrail/issues/78))
* File dialog shows home directory instead of working directory for first file dialog (issue [#448](https://github.com/CoatiSoftware/Sourcetrail/issues/448))
* Java: improved Maven timeout policy (issue [#449](https://github.com/CoatiSoftware/Sourcetrail/issues/449))
* Fixed indexing results insertion starving when indexers are really fast because of other UI updates

#### 2017.3.26
released 2017-08-24

* Java: fixed indexer to continue working when a dependency path does not exist
* Changed default indexer thread count to 0, which uses optimal thread count
* Java: Changed default value for maven dependencies directory
* Java: improved indexing speed to be 2.5 time faster
* Added commandline API for indexing projects and configuring indexing related preferences (issue [#383](https://github.com/CoatiSoftware/Sourcetrail/issues/383))
* Mention call/inheritance graphs in tutorial
* CXX: Save command line info of indexer command to errors and show within errors table (issue [#351](https://github.com/CoatiSoftware/Sourcetrail/issues/351))
* CXX: Added cross-compilation UI to project setup (issue [#370](https://github.com/CoatiSoftware/Sourcetrail/issues/370))
* Added update checker connecting to online API and checkbox to enable daily update checks on start screen.
* Java: Implemented interrupting AST visiting of Java indexer
* Java: Updated to JavaSymbolSolver 0.6.0.X and Javaparser 3.3.0
* CXX: Removed preprocessor only option, because of new include path validation
* CXX: Fixed uppercase CXX header file name and extensions saved in lowercase (issue [#437](https://github.com/CoatiSoftware/Sourcetrail/issues/396))
* Code: When clicking source location with multiple tokens or local symbols show a list to select one of these
* Sort indexer commands for increased performance
* Added custom tooltipping to Code and Graph with clickable types of variables and signatures (issue [#195](https://github.com/CoatiSoftware/Sourcetrail/issues/195), [#331](https://github.com/CoatiSoftware/Sourcetrail/issues/331))
* Java: fix issue where Maven project did not show any source files (issue [#428](https://github.com/CoatiSoftware/Sourcetrail/issues/428))
* CXX: clean indexed header path selection for project setup from Compilation Database
* Added progress dialog for "show source files" button in project setup UI
* CXX: Added include path validation to project settings
* Search: Select text elements when clicking into search bar
* Search: Fixed crash when fulltext search yields no results
* Updated to Qt 5.9.1
* CXX: Added node type for "union"
* CXX: Merge anonymous types and the respective typedef (issue [#189](https://github.com/CoatiSoftware/Sourcetrail/issues/189), [#292](https://github.com/CoatiSoftware/Sourcetrail/issues/292))
* CXX: Updated to clang 4.0.1
* Added Menu option to show Start Window
* Don't show start window when opening project with double-click
* Show license type label in title bar: "Sourcetrail [trial, test, non-commercial]"
* Graph: Fixed class expand toggle shown in overview and namespace lists
* Graph: Show inheritance edges between parents of active symbol and other visible symbols (issue [#167](https://github.com/CoatiSoftware/Sourcetrail/issues/167))
* Improved project loading performance
* Code: improved location retrieval and hover performance
* Show recent bookmarks in bookmarks menu (issue [#414](https://github.com/CoatiSoftware/Sourcetrail/issues/414))
* Code: Fixed references and buttons when activating file in snippet mode
* Java: added auto-detection for JRE System Library
* Java: Prefill JRE system library path on first Sourcetrail launch
* macOS: fixed qt.conf for case-sensitive file system (issue [#404](https://github.com/CoatiSoftware/Sourcetrail/issues/404))
* Improved database saving performance
* Improved database file clearing performance
* Improved contrasts of UI elements in bright color scheme
* Graph: Show zoom level as percent label next to zoom buttons
* Added Visual Studio Code plugin (issue [#397](https://github.com/CoatiSoftware/Sourcetrail/issues/397))
* Added "Edit Project" button below error table
* Java: Maven fixes regarding JAVA_HOME variable (issue [#405](https://github.com/CoatiSoftware/Sourcetrail/issues/405))

#### 2017.2
released 2017-06-21

* Fixed crash when using shortcuts containing letters with focus on the graph on macOS (issue [#390](https://github.com/CoatiSoftware/Sourcetrail/issues/390))
* Improved Visual Studio plugin performance
* Updated End User License Agreement and have users accept on first run on macOS and Linux
* Fixed memory issues when using large Compilation Database files (issue [#396](https://github.com/CoatiSoftware/Sourcetrail/issues/396))
* Updated to JavaParser and JavaSymbolSolver
* Fixed issue where black borders appeared around dialogs for some Linux window managers (issue [#376](https://github.com/CoatiSoftware/Sourcetrail/issues/376))
* Use platform default window decorations for UI dialogs
* Code: Made snippet minimize/maximize buttons more interactive by showing hover states
* Code: improved code view loading performance
* Java: Record more "import not found" errors
* Support Visual Studio 2017 in Visual Studio Plugin (issue [#381](https://github.com/CoatiSoftware/Sourcetrail/issues/381))
* Graph: increased creation speed by requesting less data
* macOS: Codesign .app bundle to avoid "from unidentified developer" warning
* Error: Use same error order in error table and code view
* Error: Only show first 1000 errors, click button in lower right of error table to show all (issue [#385](https://github.com/CoatiSoftware/Sourcetrail/issues/385))
* Graph: use bezier edges when activating aggregation edge
* Send ping after changing plugin ports
* Added menu action to display EULA and force accepting on macOS
* Fixed use of environment variables in project setup paths
* Multi-language project setup via Source Groups setup UI (issue [#230](https://github.com/CoatiSoftware/Sourcetrail/issues/230))
* C/C++: Fixed no files indexed when relative file paths are provided in Compilation Database (issue #388)
* C/C++: Display an error when loading a Compilation Database fails
* C/C++: Fixed source files within Compilation Database not indexed unless within Indexed Header Paths
* Fixed status message length limiting window resizing (issue [#372](https://github.com/CoatiSoftware/Sourcetrail/issues/372))
* Match source file extensions case insensitive (issue [#384](https://github.com/CoatiSoftware/Sourcetrail/issues/384))
* Fixed indexing progress file count in status bar and status view (issue [#387](https://github.com/CoatiSoftware/Sourcetrail/issues/387))
* Windows: remove setup.exe from windows installer with all necessary .dll files
* Added history list button between back and forward button
* Added history menu showing activated symbols in chronologic order

#### 0.12.25
released 2017-05-16

* Added QtCreator plugin
* Graph: Added context menu action to create bookmark for node under mouse cursor (issue [#373](https://github.com/CoatiSoftware/Sourcetrail/issues/373))
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
* Graph: Show call graphs, inheritance trees and include trees for active symbol. UI in the top left of the graph view for defining direction and depth. (issues [#249](https://github.com/CoatiSoftware/Sourcetrail/issues/249) [#337](https://github.com/CoatiSoftware/Sourcetrail/issues/337))
* Improved performance by running UI updates in parallel for each view
* Fixed project refresh not recognizing newly added files
* Show progress dialog when clicking refresh while computing files (issue [#341](https://github.com/CoatiSoftware/Sourcetrail/issues/341))
* Added show files button to summary of compilation database project setup (issue [#354](https://github.com/CoatiSoftware/Sourcetrail/issues/354))
* Show option to run only C/C++ preprocessor when indexing on indexing start dialog (issue [#297](https://github.com/CoatiSoftware/Sourcetrail/issues/297))
* Added bookmarking feature for nodes and edges. Bookmarks can have categories and comments. Data gets stored in .srctrlbm next to project file. Shortcuts similar to web browsers. (issue [#138](https://github.com/CoatiSoftware/Sourcetrail/issues/138))
* Mark files with errors and all files within a translation unit with fatal errors as incomplete (issue [#358](https://github.com/CoatiSoftware/Sourcetrail/issues/358))
* Fixed compilation database projects using C++ source extensions and standard (issue [#366](https://github.com/CoatiSoftware/Sourcetrail/issues/366))

#### 0.11.86
released 2017-04-12

* Terminating all running tasks before closing the application (issue [#343](https://github.com/CoatiSoftware/Sourcetrail/issues/343))
* Use all available cores when setting indexer threads to 0 (issue [#342](https://github.com/CoatiSoftware/Sourcetrail/issues/342))
* Added reset window layout option to view menu (issue [#289](https://github.com/CoatiSoftware/Sourcetrail/issues/289))
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

* Don't accept rich text in 'enter license' dialog field (issue [#207](https://github.com/CoatiSoftware/Sourcetrail/issues/207))
* Updated EULA to include Test License and better clarify other license types
* Fixed icons in project setup dialogs blurry on highDPI screens
* Added link to additional downloadable pre-indexed projects to trial start screen
* Fixed crash when saving preferences if they contain relative paths
* Convert all paths chosen by patch picker relative to project location in project setup
* Disabled name and location changing in project editing and removed project moving
* Split default file extensions for C++ and C to avoid wrong files getting indexed
* Deprecated 'Lazy Include Search', it's only visible to previously created projects (issue [#335](https://github.com/CoatiSoftware/Sourcetrail/issues/335))
* Support relative paths and paths containing environment variables for compilation database path (issue [#312](https://github.com/CoatiSoftware/Sourcetrail/issues/312))
* Removed 'Advanced Settings' section in project setup and show theses options in separate dialog (issue [#293](https://github.com/CoatiSoftware/Sourcetrail/issues/293))
* Support environment variables containing multiple files in project setup paths (issue [#283](https://github.com/CoatiSoftware/Sourcetrail/issues/283))
* Warn users when no 'Indexed Header Paths' were set in compilation database project (issue [#254](https://github.com/CoatiSoftware/Sourcetrail/issues/254))
* Renamed "Project Paths" to "Indexed Paths" in project setup
* Renamed "Project File Location" to "Coati Project Location" in project setup (issue [#234](https://github.com/CoatiSoftware/Sourcetrail/issues/234))
* Allow opening indexed projects in trial mode
* Record node type of non-indexed qualifiers (e.g. "std" in "std::string" as namespace)
* Log: Fixed error icon loaded for every row (issue [#287](https://github.com/CoatiSoftware/Sourcetrail/issues/287))
* Graph: Fixed lambda are placed within white area to better see call edges
* Graph: Show non-indexed nodes within their parents instead of using namespace labeling
* Code: Disabled syntax highlighting for project description
* Graph: Fixed different font-faces causing graph nodes to expand to bigger cell amount
* Mac: Fixed wrong READMEs at some plugins (issue [#315](https://github.com/CoatiSoftware/Sourcetrail/issues/315))
* Mac: Removed boost libraries and libLTO from Mac bundle
* Windows: Add Visual Studio compatibility flags to new project when Visual Studio include paths are present
* Renamed undefined nodes to non-indexed nodes for clarification
* Show progress in file clearing dialog
* Extended syntax highlighting for Java and distinguish by project type
* offer Windows 64bit build (issue [#300](https://github.com/CoatiSoftware/Sourcetrail/issues/300))
* Fixed legacy Java 6 install necessary on MacOS (issue [#280](https://github.com/CoatiSoftware/Sourcetrail/issues/280))
* Updated to Qt 5.8
* Updated to clang/llvm 3.9
* Added multithreading to Visual Studio plugin compilation database creation
* Graph: Use new list layouting for overview bundles and namespaces/packages
* Graph: Group nodes in list layout by starting character and allow jumping to group by pressing the key
* Graph: Use node specific styles for overview bundles
* Visual Studio plugin can now handle projects with non-native build tool for compilation database export
* Changed menu action 'Licenses' to '3rd Party Licenses'
* Added dialog with all shortcuts to Help menu
* Log: Show error in errors table when clicked in code view (issue [#323](https://github.com/CoatiSoftware/Sourcetrail/issues/323))
* Code: Switched default mouse drag behavior to selection, panning on Shift + drag, and show suitable cursors
* Updated company address in about dialog
* Graph: Added on-screen zoom buttons
* Graph: Split aggregation edge when expanding nodes if possible
* Uncommented lines using the std library in TicTacToe sample
* Fixed issue with lambda return type (issue [#318](https://github.com/CoatiSoftware/Sourcetrail/issues/318))
* Save builtin types as separate node type (issue [#2](https://github.com/CoatiSoftware/Sourcetrail/issues/2))
* Remove unused included files after refreshing
* Java: Fixed packages that only contain packages to show up as non-indexed
* Added context-menu actions to copy node names, file paths and show files in containing directory (issue [#320](https://github.com/CoatiSoftware/Sourcetrail/issues/320))
* Fixed crash when entering a single space in 'enter license' dialog (issue [#277](https://github.com/CoatiSoftware/Sourcetrail/issues/277))
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
* Infer some non-indexed node types by their edges


#### 0.10.0
released 2016-12-14

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
* Search: Fixed crashes related to empty searches (issue [#251](https://github.com/CoatiSoftware/Sourcetrail/issues/251))
* Search: Do fulltext search when no autocompletion match is available
* Added checkbox for full project refresh to start indexing dialog
* Disabled clang error limit by default to make sure all errors are shown
* Linux: Updated libs and added missing ones (issue [#245](https://github.com/CoatiSoftware/Sourcetrail/issues/245))
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
* Fixed anonymous symbol name conflicts (issue [#241](https://github.com/CoatiSoftware/Sourcetrail/issues/241))
* Preferences: Added Indexer Logging option to print AST information during indexing
* Search: Added second line to search autocompletion list showing namespace, package or filepath
* Made cells readonly in errors table to prevent editing (issue [#236](https://github.com/CoatiSoftware/Sourcetrail/issues/236))
* Renamed Log Window to Status window


#### 0.9.23
released 2016-10-27

* Windows: Added logging and exception handling to VS plugin
* Windows: Added VC++ runtime dlls for windows installer
* Windows: Added missing VC++ runtime dlls (issue [#231](https://github.com/CoatiSoftware/Sourcetrail/issues/231))
* Added setting for default graph zoom on mouse wheel to preferences (issue [#237](https://github.com/CoatiSoftware/Sourcetrail/issues/237))
* Scroll errors table to newest error during indexing
* Fixed best scoring for search autocompletions skipped some combinations
* Fixed flickering of errors table when indexing
* Don't show error snippets in overview screen anymore
* Fixed crash when Project Paths contain files and lazy include search in enabled (issue [#201](https://github.com/CoatiSoftware/Sourcetrail/issues/201))
* Fixed edge cases that caused deadlocks in indexing
* Added language specific project icons to start screen
* Fixed application couldn't be quit when start screen was showing (issue [#227](https://github.com/CoatiSoftware/Sourcetrail/issues/227))
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
* Properly quit application when window is closed while indexing (issue [#121](https://github.com/CoatiSoftware/Sourcetrail/issues/121))
* Graph: Count only top level nodes in bundle nodes of graph
* Search: Improved scoring of autocompletion matches to use best score for match
* App will go into trial mode without valid license
* Java: Auto detect java root source directories
* Fixed refresh crash (issue [#220](https://github.com/CoatiSoftware/Sourcetrail/issues/220))
* Java: improved indexer reliability a lot
* Java: Changed name of generic type to use name of declaration
* Fixed project refresh does not recognize removed source paths
* Java: Define maximum allocated jvm memory in preferences
* Follow symbolic directory links within source paths (issue [#205](https://github.com/CoatiSoftware/Sourcetrail/issues/205))
* Use white size grip in blue dialogs
* Graph: Use type node colors for undefined nodes
* Java: Show import not found as error
* Code: Select and copy source code in code view with SHIFT + mouse drag (issue [#7](https://github.com/CoatiSoftware/Sourcetrail/issues/7))
* Code: Added markers next to line numbers in code view showing active locations and scopes
* Merged app and trial to one build

#### 0.8.21
released 2016-09-16

* Added options for checking multiple paths in Indexed Header Paths dialog during project setup from Compilation Database [#190](https://github.com/CoatiSoftware/Sourcetrail/issues/190)
* Added all C/C++ standards supported by clang
* Fixed C language setting in empty project setup not propagated to indexer [#177](https://github.com/CoatiSoftware/Sourcetrail/issues/177)
* Added Sublime plugin to Sublime Text Package Control
* Improved indexer parallelization for faster indexing
* Changed all font-sizes to px for similar look on all platforms
* Added font face dropdown to Preferences showing all available monospace fonts [#172](https://github.com/CoatiSoftware/Sourcetrail/issues/172)
* Smoother font rendering on Windows by setting font engine to FreeType via qt.conf file [#137](https://github.com/CoatiSoftware/Sourcetrail/issues/137)
* Interrupt indexing while AST visiting for C/C++ for shorter waiting time
* Save graph as PNG image via context menu in graph view [#180](https://github.com/CoatiSoftware/Sourcetrail/issues/180)
* Added base folder to Linux package [#181](https://github.com/CoatiSoftware/Sourcetrail/issues/181)
* Use inverted golden ratio in indexing progress dialog

#### 0.8.0
released 2016-09-01

* Improved titles and help texts in project setup UI
* Fixed error locations not saved with absolute file paths in Compilation Database projects
* Added missing source files extensions setting to the Edit Project dialog [#132](https://github.com/CoatiSoftware/Sourcetrail/issues/132)
* Changed default text colors to black in project setup UI [#140](https://github.com/CoatiSoftware/Sourcetrail/issues/140)
* Added plugin ports settings to Preferences [#145](https://github.com/CoatiSoftware/Sourcetrail/issues/145)
* Improved project state handling on refreshing
* Added Java sample project JavaParser: http://javaparser.org/
* Fixed regex for system includes used wrongly in syntax highlighter
* Added setting to disable file and console logging, default is off [#157](https://github.com/CoatiSoftware/Sourcetrail/issues/157)
* Added migrations between different ApplicationSettings versions
* Added auto detection for Java library to Preferences
* Removed auto refresh option from Search view
* Added progress dialogs for indexing and block UI [#143](https://github.com/CoatiSoftware/Sourcetrail/issues/143)
* Hide name qualifiers for graph nodes in Java projects
* Added handling when java library not found
* Added plain text editing dialog to path list boxes
* Use language dependent symbol name delimiter in UI
* Added scroll speed setting to preferences [#112](https://github.com/CoatiSoftware/Sourcetrail/issues/112)
* Added Java project setup UI
* Record start and end locations of scopes in Java
* Display unresolved types names in Java as 'unresolved-type'
* Cache stats for overview to speed up display
* Fixed graph nodes to restore their expand state when going back [#118](https://github.com/CoatiSoftware/Sourcetrail/issues/118)
* Added shortcuts for navigating to next and previous reference in code view
* Pressing Tab in search completes up to next ::, Delete erases to last :: [#133](https://github.com/CoatiSoftware/Sourcetrail/issues/133)
* Bundle anonymous namespaces separately in graph overview [#126](https://github.com/CoatiSoftware/Sourcetrail/issues/126)
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
* Moved Preferences menu item to Edit menu on Windows/Linux [#113](https://github.com/CoatiSoftware/Sourcetrail/issues/113)
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
* Added separate step for defining project name, project location and compilation database to project setup
* Explain that the project stays up-to-date with the compilation database on refresh in project setup UI
* Disregard source extensions when loading from compilation database
* Removed error logs in name resolver
* Added plugin for VIM: https://github.com/CoatiSoftware/vim-coati

#### 0.6.0.0
released 2016-04-27

* Added support for temporary test licenses and show error message when it expired
* Increased search autocompletion performance to stay mostly below 100ms
* Increased project load performance
* Improved file clearing performance on project refresh
* Increased graph creation performance for overview screen
* Increased graph performance by reducing to fixed number of requests
* Increased graph edge drawing performance
* Improved error display performance by not expanding all of them at once
* Added analysis for local symbols and allow selecting them in the code view
* Added support for function pointers to analysis
* Smoother graph panning and zooming on keyboard using update loop
* Changed style of aggregation edge to cleaner look
* Allow certain edge types to enter and leave at top and bottom of nodes as well e.g. inheritance
* Changed namespace activation to show the namespace name in the search bar and the namespace definitions in the code view
* Differentiate between explicit and implicit nodes and only show implicit nodes in the graph view when connected
* Show deleted projects on the start screen and show messagebox to delete them
* Show analysis duration in hh:mm:ss
* Added bug tracker to the help menu
* Added home button next to search field for showing the project overview
* Added shortcut and menu item to show project overview
* Allow support for environment variables in project setup with syntax ${VARIABLE_NAME}
* Added welcome message to Coati trial
* Show quit button on license enter screen when no valid license was entered yet
* Allow color schemes to define text colors for code view highlights
* Defined new highlight colors for code view in all color schemes
* Changed style of out-of-date files to be followed by *
* Fixed Path boxes not sized correctly when switching back and forth in the project wizard
* Check analyzed paths for existence in project wizard
* Inform the user about project changes and ask before reparsing the project
* Removed "simple setup" step in project wizard and added "lazy include search"-checkbox
* Edit the project settings by clicking the project name in the overview stats
* Updated to Qt 5.6
* Improved high DPI support for windows
* Added Windows system dlls to installer on Windows so users don't not need to install the redistributable package
* Added vs-clang compatibility mode as compile flags when creating project from Visual Studio solution
* Added auto detection for system header paths to project wizard and preferences when using gcc, clang or vs compilers

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
* Improved graph view node bundling to bundle more
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
* Fixed number positions in graph
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
* Fixed constructors and destructors using return type 'void' without source location
* Parse errors will be displayed as they appear during analysis
* Redesigned the about window
* Installing prerequisites in Windows Installer
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
