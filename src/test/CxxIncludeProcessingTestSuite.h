#include "cxxtest/TestSuite.h"

#include "utility/text/TextAccess.h"
#include "utility/IncludeDirective.h"
#include "utility/IncludeProcessing.h"

class CxxIncludeProcessingTestSuite: public CxxTest::TestSuite
{
public:
	void test_include_detection_finds_include_with_quotes()
	{
		std::vector<IncludeDirective> includeDirectives = IncludeProcessing::getIncludeDirectives(TextAccess::createFromString(
			"#include \"foo.h\"",
			FilePath("foo.cpp")
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
			FilePath("foo.cpp")
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
			FilePath("foo.cpp")
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
		std::set<FilePath> headerSearchDirectoies = IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath("data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_does_not_find_path_relative_to_including_file/a.cpp") },
		{ FilePath("data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_does_not_find_path_relative_to_including_file") },
			1, [&](float) {}
		);

		TS_ASSERT(headerSearchDirectoies.empty());
	}

	void test_header_search_path_detection_finds_path_inside_sub_directory()
	{
		std::set<FilePath> headerSearchDirectoies = IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath("data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory/a.cpp") },
		{ FilePath("data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory") },
			1, [&](float) {}
		);

		TS_ASSERT(!headerSearchDirectoies.empty());
		if (!headerSearchDirectoies.empty())
		{
			TS_ASSERT_EQUALS(
				"CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_inside_sub_directory/include",
				headerSearchDirectoies.begin()->getRelativeTo(FilePath("data").getAbsolute()).str()
			);
		}
	}

	void test_header_search_path_detection_finds_path_relative_to_sub_directory()
	{
		std::set<FilePath> headerSearchDirectoies = IncludeProcessing::getHeaderSearchDirectories(
		{ FilePath("data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory/a.cpp") },
		{ FilePath("data/CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory") },
			1, [&](float) {}
		);

		TS_ASSERT(!headerSearchDirectoies.empty());
		if (!headerSearchDirectoies.empty())
		{
			TS_ASSERT_EQUALS(
				"CxxIncludeProcessingTestSuite/test_header_search_path_detection_finds_path_relative_to_sub_directory/include",
				headerSearchDirectoies.begin()->getRelativeTo(FilePath("data").getAbsolute()).str()
			);
		}
	}
};
