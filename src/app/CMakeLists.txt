add_files(
	APP_FILES

	data/parser/cxx/TaskParseCxx.cpp

	utility/commandline/CommandLineParser.cpp
	utility/commandline/CommandLineParser.h

	utility/headerSearch/ClangDetector.cpp
	utility/headerSearch/ClangDetector.h
	utility/headerSearch/CompilerDetectorBase.cpp
	utility/headerSearch/CompilerDetectorBase.h
	utility/headerSearch/DetectorBase.cpp
	utility/headerSearch/DetectorBase.h
	utility/headerSearch/DetectorProxy.h
	utility/headerSearch/GCCDetector.cpp
	utility/headerSearch/GCCDetector.h
	utility/headerSearch/StandardHeaderDetection.cpp
	utility/headerSearch/StandardHeaderDetection.h
	utility/headerSearch/VisualStudioDetector.cpp
	utility/headerSearch/VisualStudioDetector.h

	utility/utilityApp.cpp
	utility/utilityApp.h

	isTrial.cpp
	LicenseChecker.h
	main.cpp
)
