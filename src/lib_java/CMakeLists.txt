
add_files(
	LIB_JAVA

	data/indexer/IndexerCommandJava.cpp
	data/indexer/IndexerCommandJava.h
	data/indexer/IndexerJava.cpp
	data/indexer/IndexerJava.h

	data/parser/java/JavaParser.cpp
	data/parser/java/JavaParser.h
	data/parser/java/JavaEnvironment.cpp
	data/parser/java/JavaEnvironment.h
	data/parser/java/JavaEnvironmentFactory.cpp
	data/parser/java/JavaEnvironmentFactory.h

	project/SourceGroupFactoryModuleJava.cpp
	project/SourceGroupFactoryModuleJava.h
	project/SourceGroupJava.cpp
	project/SourceGroupJava.h
	project/SourceGroupJavaEmpty.cpp
	project/SourceGroupJavaEmpty.h
	project/SourceGroupJavaGradle.cpp
	project/SourceGroupJavaGradle.h
	project/SourceGroupJavaMaven.cpp
	project/SourceGroupJavaMaven.h

	utility/utilityJava.cpp
	utility/utilityJava.h
	utility/utilityGradle.cpp
	utility/utilityGradle.h
	utility/utilityMaven.cpp
	utility/utilityMaven.h
	
	LanguagePackageJava.cpp
	LanguagePackageJava.h
)
