#include "cxxtest/TestSuite.h"

#include "utility/file/FilePath.h"

class FilePathTestSuite : public CxxTest::TestSuite
{
public:
	void test_file_path_gets_created_empty()
	{
		const FilePath path;

		TS_ASSERT_EQUALS(path.str(), "");
	}

	void test_file_path_gets_created_with_char_array()
	{
		const FilePath path("data/FilePathTestSuite/main.cpp");

		TS_ASSERT_EQUALS(path.str(), "data/FilePathTestSuite/main.cpp");
	}

	void test_file_path_gets_created_with_string()
	{
		const std::string str("data/FilePathTestSuite/main.cpp");
		const FilePath path(str);

		TS_ASSERT_EQUALS(path.str(), str);
	}

	void test_file_path_gets_created_other_file_path()
	{
		const FilePath path("data/FilePathTestSuite/main.cpp");
		const FilePath path2(path);

		TS_ASSERT_EQUALS(path, path2);
	}

	void test_file_path_empty()
	{
		const FilePath path1("data/FilePathTestSuite/a.cpp");
		const FilePath path2;

		TS_ASSERT(!path1.empty());
		TS_ASSERT(path2.empty());
	}

	void test_file_path_exists()
	{
		const FilePath path("data/FilePathTestSuite/a.cpp");

		TS_ASSERT(path.exists());
	}

	void test_file_path_not_exists()
	{
		const FilePath path("data/FilePathTestSuite/a.h");

		TS_ASSERT(!path.exists());
	}

	void test_file_path_is_directory()
	{
		const FilePath path("data/FilePathTestSuite/a.cpp");

		TS_ASSERT(!path.isDirectory());
		TS_ASSERT(path.getParentDirectory().isDirectory());
	}

	void test_empty_file_path_has_empty_parent_directory()
	{
		const FilePath path;

		TS_ASSERT(path.empty());
		TS_ASSERT(path.getParentDirectory().empty());
	}

	void test_file_path_is_absolute()
	{
		const FilePath path("data/FilePathTestSuite/a.cpp");

		TS_ASSERT(!path.isAbsolute());
		TS_ASSERT(path.getAbsolute().isAbsolute());
	}

	void test_file_path_parent_directory()
	{
		const FilePath path("data/FilePathTestSuite/a.cpp");

		TS_ASSERT(path.getParentDirectory().str() == "data/FilePathTestSuite");
		TS_ASSERT(path.getParentDirectory().getParentDirectory().str() == "data");
	}

	void test_file_path_relative_to_other_path()
	{
		const FilePath pathA("data/FilePathTestSuite/a.cpp");
		const FilePath pathB("data/FilePathTestSuite/test/c.h");

		TS_ASSERT_EQUALS(pathA.getRelativeTo(pathB).str(), "../a.cpp");
		TS_ASSERT_EQUALS(pathB.getRelativeTo(pathA).str(), "test/c.h");
	}

	void test_file_path_relative_to_other_directory()
	{
		const FilePath pathA("data/FilePathTestSuite/a.cpp");
		const FilePath pathB("data/FilePathTestSuite/test");

		TS_ASSERT_EQUALS(pathA.getRelativeTo(pathB).str(), "../a.cpp");
	}

	void test_file_path_relative_to_same_directory()
	{
		const FilePath pathA("data/FilePathTestSuite/test");

		TS_ASSERT_EQUALS(pathA.getRelativeTo(pathA).str(), "./");
	}

	void test_file_path_file_name()
	{
		const FilePath path("data/FilePathTestSuite/abc.h");

		TS_ASSERT_EQUALS(path.fileName(), "abc.h");
	}

	void test_file_path_extension()
	{
		const FilePath path("data/FilePathTestSuite/a.h");

		TS_ASSERT_EQUALS(path.extension(), ".h");
	}

	void test_file_path_without_extension()
	{
		const FilePath path("data/FilePathTestSuite/a.h");

		TS_ASSERT_EQUALS(path.withoutExtension(), FilePath("data/FilePathTestSuite/a"));
	}

	void test_file_path_has_extension()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".cpp");
		extensions.push_back(".cc");

		TS_ASSERT(FilePath("data/FilePathTestSuite/a.h").hasExtension(extensions));
		TS_ASSERT(FilePath("data/FilePathTestSuite/b.cpp").hasExtension(extensions));
		TS_ASSERT(!FilePath("data/FilePathTestSuite/a.m").hasExtension(extensions));
	}

	void test_file_path_equals_file_with_different_relative_paths()
	{
		const FilePath pathA("data/FilePathTestSuite/a.cpp");
		const FilePath pathA2("data/../data/FilePathTestSuite/./a.cpp");

		TS_ASSERT_EQUALS(pathA, pathA2);
	}

	void test_file_path_equals_relative_and_absolute_paths()
	{
		const FilePath pathA("data/FilePathTestSuite/a.cpp");
		const FilePath pathA2 = pathA.getAbsolute();

		TS_ASSERT_EQUALS(pathA, pathA2);
	}

	void test_file_path_equals_absolute_and_canonical_paths()
	{
		const FilePath path("data/../data/FilePathTestSuite/./a.cpp");

		TS_ASSERT_EQUALS(path.getAbsolute(), path.getCanonical());
	}

	void test_file_path_canonical_removes_symlinks()
	{
#ifndef _WIN32
		const FilePath pathA("data/FilePathTestSuite/parent/target/d.cpp");
		const FilePath pathB("data/FilePathTestSuite/target/d.cpp");

		TS_ASSERT_EQUALS(pathB.getAbsolute(), pathA.getCanonical());
#endif
	}

	void test_file_path_compares_paths_with_posix_and_windows_format()
	{
#ifdef _WIN32
		const FilePath pathB("data/FilePathTestSuite/b.cc");
		const FilePath pathB2("data\\FilePathTestSuite\\b.cc");

		TS_ASSERT_EQUALS(pathB, pathB2);
#endif
	}

	void test_file_path_differs_for_different_existing_files()
	{
		const FilePath pathA("data/FilePathTestSuite/a.cpp");
		const FilePath pathB("data/FilePathTestSuite/b.cc");

		TS_ASSERT_DIFFERS(pathA, pathB);
	}

	void test_file_path_differs_for_different_nonexisting_files()
	{
		const FilePath pathA("data/FilePathTestSuite/a.h");
		const FilePath pathB("data/FilePathTestSuite/b.c");

		TS_ASSERT_DIFFERS(pathA, pathB);
	}

	void test_file_path_differs_for_existing_and_nonexisting_files()
	{
		const FilePath pathA("data/FilePathTestSuite/a.h");
		const FilePath pathB("data/FilePathTestSuite/b.cc");

		TS_ASSERT_DIFFERS(pathA, pathB);
	}
};
