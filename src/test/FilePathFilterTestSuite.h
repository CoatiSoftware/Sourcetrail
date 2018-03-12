#include "cxxtest/TestSuite.h"

#include "utility/file/FilePathFilter.h"

class FilePathFilterTestSuite: public CxxTest::TestSuite
{
public:
	void test_file_path_filter_finds_exact_match()
	{
		FilePathFilter filter(L"test.h");

		TS_ASSERT(filter.isMatching(FilePath(L"test.h")));
	}

	void test_file_path_filter_finds_match_with_single_asterisk_in_same_level()
	{
		FilePathFilter filter(L"*test.*");

		TS_ASSERT(filter.isMatching(FilePath(L"this_is_a_test.h")));
	}

	void test_file_path_filter_finds_match_with_single_asterisk_in_different_level()
	{
		FilePathFilter filter(L"*/this_is_a_test.h");

		TS_ASSERT(filter.isMatching(FilePath(L"folder/this_is_a_test.h")));
	}

	void test_file_path_filter_does_not_find_match_with_single_asterisk_in_different_level()
	{
		FilePathFilter filter(L"*/test.h");

		TS_ASSERT(!filter.isMatching(FilePath(L"test.h")));
	}

	void test_file_path_filter_finds_match_with_multiple_asterisk_in_same_level()
	{
		FilePathFilter filter(L"**test.h");

		TS_ASSERT(filter.isMatching(FilePath(L"folder/this_is_a_test.h")));
	}

	void test_file_path_filter_finds_match_with_multiple_asterisk_in_different_level()
	{
		FilePathFilter filter(L"root/**/test.h");

		TS_ASSERT(filter.isMatching(FilePath(L"root/folder1/folder2/test.h")));
	}

	void test_file_path_filter_does_not_find_match_with_multiple_asterisk_in_different_level()
	{
		FilePathFilter filter(L"**/test.h");

		TS_ASSERT(!filter.isMatching(FilePath(L"folder/this_is_a_test.h")));
	}

	void test_file_path_filter_escapes_dot_character()
	{
		FilePathFilter filter(L"test.h");

		TS_ASSERT(!filter.isMatching(FilePath(L"testyh")));
	}

	void test_file_path_filter_escapes_plus_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test+.h").isMatching(FilePath(L"folder/test+.h")));
	}

	void test_file_path_filter_escapes_minus_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test[-].h").isMatching(FilePath(L"folder/test[-].h")));
	}

	void test_file_path_filter_escapes_dollar_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test$.h").isMatching(FilePath(L"folder/test$.h")));
	}

	void test_file_path_filter_escapes_circumflex_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test^.h").isMatching(FilePath(L"folder/test^.h")));
	}

	void test_file_path_filter_escapes_open_round_brace_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test(.h").isMatching(FilePath(L"folder/test(.h")));
	}

	void test_file_path_filter_escapes_close_round_brace_character()
	{
		TS_ASSERT(FilePathFilter(L"folder\\test).h").isMatching(FilePath(L"folder/test).h")));
	}

	void test_file_path_filter_escapes_open_curly_brace_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test{.h").isMatching(FilePath(L"folder/test{.h")));
	}

	void test_file_path_filter_escapes_close_curly_brace_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test}.h").isMatching(FilePath(L"folder/test}.h")));
	}

	void test_file_path_filter_escapes_open_squared_brace_character()
	{
		TS_ASSERT(FilePathFilter(L"folder/test[.h").isMatching(FilePath(L"folder/test[.h")));
	}

	void test_file_path_filter_escapes_close_squared_brace_character()
	{
		TS_ASSERT(FilePathFilter(L"folder\\test].h").isMatching(FilePath(L"folder/test].h")));
	}

	void test_file_path_filter_finds_backslash_if_slash_was_provided()
	{
		FilePathFilter filter(L"folder/test.h");

		TS_ASSERT(filter.isMatching(FilePath(L"folder\\test.h")));
	}

	void test_file_path_filter_finds_slash_if_backslash_was_provided()
	{
		FilePathFilter filter(L"folder\\test.h");

		TS_ASSERT(filter.isMatching(FilePath(L"folder/test.h")));
	}
};
