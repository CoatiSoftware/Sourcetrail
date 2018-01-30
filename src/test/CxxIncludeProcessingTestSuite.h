#include "cxxtest/TestSuite.h"

#include "utility/text/TextAccess.h"
#include "utility/IncludeDirective.h"
#include "utility/IncludeProcessing.h"
#include "utility/utility.h"

class CxxIncludeProcessingTestSuite: public CxxTest::TestSuite
{
public:
	void test_include_detection_finds_include_with_quotes()
	{
		std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
			"#include \"foo.h\"",
			FilePath(L"foo.cpp")
		));

		TS_ASSERT(!includeDirectives.empty());

		if (!includeDirectives.empty())
		{
			TS_ASSERT_EQUALS("foo.h", includeDirectives.front().getIncludedFile().str());
			TS_ASSERT_EQUALS("foo.cpp", includeDirectives.front().getIncludingFile().str());
		}
	}

	void test_include_detection_finds_include_with_angle_brackets()
	{
		std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
			"#include <foo.h>",
			FilePath(L"foo.cpp")
		));

		TS_ASSERT(!includeDirectives.empty());

		if (!includeDirectives.empty())
		{
			TS_ASSERT_EQUALS("foo.h", includeDirectives.front().getIncludedFile().str());
			TS_ASSERT_EQUALS("foo.cpp", includeDirectives.front().getIncludingFile().str());
		}
	}
	void test_include_detection_finds_include_with_quotes_and_space_before_keyword()
	{
		std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
			"# include \"foo.h\"",
			FilePath(L"foo.cpp")
		));

		TS_ASSERT(!includeDirectives.empty());

		if (!includeDirectives.empty())
		{
			TS_ASSERT_EQUALS("foo.h", includeDirectives.front().getIncludedFile().str());
			TS_ASSERT_EQUALS("foo.cpp", includeDirectives.front().getIncludingFile().str());
		}
	}

	void test_include_detection_does_not_find_include_in_empty_file()
	{
		TS_ASSERT(IncludeProcessing::getIncludeDirectives(TextAccess::createFromString("")).empty());
	}

	void test_include_detection_does_not_find_include_in_file_without_preprocessor_directive()
	{
		TS_ASSERT(IncludeProcessing::getIncludeDirectives(TextAccess::createFromString("foo")).empty());
	}

	void test_include_detection_does_not_find_include_in_file_without_include_preprocessor_directive()
	{
		TS_ASSERT(IncludeProcessing::getIncludeDirectives(TextAccess::createFromString("#ifdef xx\n#endif")).empty());
	}

	void test_header_search_path_detection_does_not_find_path_relative_to_including_file()
	{
		std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
			{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_does_not_find_path_relative_to_including_file/a.cpp") },
			{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_does_not_find_path_relative_to_including_file") },
			{ },
			1, [](float) {}
		));

		TS_ASSERT(headerSearchDirectories.empty());
	}

	void test_header_search_path_detection_finds_path_inside_sub_directory()
	{
		std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
			{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory/a.cpp") },
			{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory") },
			{},
			1, [](float) {}
		));

		TS_ASSERT(utility::containsElement<FilePath>(
			headerSearchDirectories,
			FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory/include").makeAbsolute()
		));
	}

	void test_header_search_path_detection_finds_path_relative_to_sub_directory()
	{
		std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
			{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory/a.cpp") },
			{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory") },
			{},
			1, [](float) {}
		));

		TS_ASSERT(utility::containsElement<FilePath>(
			headerSearchDirectories,
			FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory/include").makeAbsolute()
		));
	}

	void test_header_search_path_detection_finds_path_included_in_header_search_path()
	{
		std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/a.cpp") },
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/include_b") },
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/include_a") },
			1, [](float) {}
		));


		TS_ASSERT(utility::containsElement<FilePath>(
			headerSearchDirectories,
			FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_header_search_path/include_b").makeAbsolute()
		));
	}

	void test_header_search_path_detection_finds_path_included_in_future_header_search_path()
	{
		std::vector<FilePath> headerSearchDirectories = utility::toVector(IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path/a.cpp") },
		{ FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path") },
		{  },
			1, [](float) {}
		));

		TS_ASSERT(utility::containsElement<FilePath>(
			headerSearchDirectories,
			FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path/include_a").makeAbsolute()
		));
		TS_ASSERT(utility::containsElement<FilePath>(
			headerSearchDirectories,
			FilePath(L"data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_included_in_future_header_search_path/include_b").makeAbsolute()
		));
	}
};
