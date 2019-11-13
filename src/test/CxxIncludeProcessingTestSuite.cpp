#include "catch.hpp"

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "TextAccess.h"
#include "IncludeDirective.h"
#include "IncludeProcessing.h"
#include "utility.h"

TEST_CASE("include detection finds include with quotes")
{
	std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
		"#include \"foo.h\"",
		FilePath(L"foo.cpp")
	));

	REQUIRE(!includeDirectives.empty());

	if (!includeDirectives.empty())
	{
		REQUIRE(L"foo.h" == includeDirectives.front().getIncludedFile().wstr());
		REQUIRE(L"foo.cpp" == includeDirectives.front().getIncludingFile().wstr());
	}
}

TEST_CASE("include detection finds include with angle brackets")
{
	std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
		"#include <foo.h>",
		FilePath(L"foo.cpp")
	));

	REQUIRE(!includeDirectives.empty());

	if (!includeDirectives.empty())
	{
		REQUIRE(L"foo.h" == includeDirectives.front().getIncludedFile().wstr());
		REQUIRE(L"foo.cpp" == includeDirectives.front().getIncludingFile().wstr());
	}
}
TEST_CASE("include detection finds include with quotes and space before keyword")
{
	std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
		"# include \"foo.h\"",
		FilePath(L"foo.cpp")
	));

	REQUIRE(!includeDirectives.empty());

	if (!includeDirectives.empty())
	{
		REQUIRE(L"foo.h" == includeDirectives.front().getIncludedFile().wstr());
		REQUIRE(L"foo.cpp" == includeDirectives.front().getIncludingFile().wstr());
	}
}

TEST_CASE("include detection does not find include in empty file")
{
	REQUIRE(IncludeProcessing::getIncludeDirectives(TextAccess::createFromString("")).empty());
}

TEST_CASE("include detection does not find include in file without preprocessor directive")
{
	REQUIRE(IncludeProcessing::getIncludeDirectives(TextAccess::createFromString("foo")).empty());
}

TEST_CASE("include detection does not find include in file without include preprocessor directive")
{
	REQUIRE(IncludeProcessing::getIncludeDirectives(TextAccess::createFromString("#ifdef xx\n#endif")).empty());
}

TEST_CASE("header search path detection does not find path relative to including file")
{
	std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_does_not_find_path_relative_to_including_file/a.cpp") },
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_does_not_find_path_relative_to_including_file") },
		{ },
		1, [](float) {}
	));

	REQUIRE(headerSearchDirectories.empty());
}

TEST_CASE("header search path detection finds path inside sub directory")
{
	std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory/a.cpp") },
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory") },
		{},
		1, [](float) {}
	));

	REQUIRE(utility::containsElement<FilePath>(
		headerSearchDirectories,
		FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory/include").makeAbsolute()
	));
}

TEST_CASE("header search path detection finds path relative to sub directory")
{
	std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory/a.cpp") },
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory") },
		{},
		1, [](float) {}
	));

	REQUIRE(utility::containsElement<FilePath>(
		headerSearchDirectories,
		FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory/include").makeAbsolute()
	));
}

TEST_CASE("header search path detection finds path included in header search path")
{
	std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
	{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/a.cpp") },
	{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/include_b") },
	{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/include_a") },
		1, [](float) {}
	));


	REQUIRE(utility::containsElement<FilePath>(
		headerSearchDirectories,
		FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/include_b").makeAbsolute()
	));
}

TEST_CASE("header search path detection finds path included in future header search path")
{
	std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
	{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path/a.cpp") },
	{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path") },
	{  },
		1, [](float) {}
	));

	REQUIRE(utility::containsElement<FilePath>(
		headerSearchDirectories,
		FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path/include_a").makeAbsolute()
	));
	REQUIRE(utility::containsElement<FilePath>(
		headerSearchDirectories,
		FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path/include_b").makeAbsolute()
	));
}

#endif // BUILD_CXX_LANGUAGE_PACKAGE
