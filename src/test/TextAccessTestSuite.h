#include "cxxtest/TestSuite.h"

#include "utility/text/TextAccess.h"

class TextAccessTestSuite : public CxxTest::TestSuite
{
public:
	void test_textAccessString_constructor()
	{
		std::string text = getTestText();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);

		TS_ASSERT(textAccess.get() != NULL);
	}

	void test_textAccessString_lines_count()
	{
		std::string text = getTestText();
		unsigned int lineCount = 8;

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);

		TS_ASSERT_EQUALS(textAccess->getLineCount(), lineCount);
	}

	void test_textAccessString_lines_content()
	{
		std::string text = getTestText();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
		std::vector<std::string> lines = textAccess->getLines(1, 4);

		TS_ASSERT_EQUALS(lines.size(), 4);
		TS_ASSERT_EQUALS(lines[0], "\"But the plans were on display . . .\"\n");
		TS_ASSERT_EQUALS(lines[1], "\"On display? I eventually had to go down to the cellar to find them.\"\n");
		TS_ASSERT_EQUALS(lines[2], "\"That's the display department.\"\n");
		TS_ASSERT_EQUALS(lines[3], "\"With a torch.\"\n");
	}

	void test_textAccessString_lines_content_error_handling()
	{
		std::string text = getTestText();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
		std::vector<std::string> lines = textAccess->getLines(3, 2);

		TS_ASSERT_EQUALS(lines.size(), 0);

		lines = textAccess->getLines(10, 3);

		TS_ASSERT_EQUALS(lines.size(), 0);

		lines = textAccess->getLines(1, 10);

		TS_ASSERT_EQUALS(lines.size(), 0);

		std::string line = textAccess->getLine(0);

		TS_ASSERT_EQUALS(line, "");

		lines = textAccess->getLines(0, 2);

		TS_ASSERT_EQUALS(line, "");
	}

	void test_textAccessString_single_line_content()
	{
		std::string text = getTestText();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
		std::string line = textAccess->getLine(6);

		TS_ASSERT_EQUALS(line, "\"So had the stairs.\"\n");
	}

	void test_textAccessString_all_lines()
	{
		std::string text = getTestText();
		unsigned int lineCount = 8;

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
		std::vector<std::string> lines = textAccess->getAllLines();

		TS_ASSERT_EQUALS(lines.size(), lineCount);
	}

	void test_textAccessFile_constructor()
	{
		FilePath filePath("data/TextAccessTestSuite/text.txt");

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

		TS_ASSERT(textAccess.get() != NULL);
	}

	void test_textAccessFile_lines_count()
	{
		FilePath filePath("data/TextAccessTestSuite/text.txt");
		unsigned int lineCount = 7;

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

		TS_ASSERT_EQUALS(textAccess->getLineCount(), lineCount);
	}

	void test_textAccessFile_lines_content()
	{
		FilePath filePath("data/TextAccessTestSuite/text.txt");

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);
		std::vector<std::string> lines = textAccess->getLines(1, 4);

		TS_ASSERT_EQUALS(lines.size(), 4);
		TS_ASSERT_EQUALS(lines[0], "\"If you're a researcher on this book thing and you were on Earth, you must have been gathering material on it.\"\n");
		TS_ASSERT_EQUALS(lines[1], "\"Well, I was able to extend the original entry a bit, yes.\"\n");
		TS_ASSERT_EQUALS(lines[2], "\"Let me see what it says in this edition, then. I've got to see it.\"\n");
		TS_ASSERT_EQUALS(lines[3], "... \"What? Harmless! Is that all it's got to say? Harmless! One word! ... Well, for God's sake I hope you managed to recitify that a bit.\"\n");
	}

	void test_textAccessFile_get_filePath()
	{
		FilePath filePath("data/TextAccessTestSuite/text.txt");
		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

		TS_ASSERT_EQUALS(textAccess->getFilePath(), filePath);
	}

private:
	std::string getTestText()
	{
		std::string text =
			"\"But the plans were on display . . .\"\n"
			"\"On display? I eventually had to go down to the cellar to find them.\"\n"
			"\"That's the display department.\"\n"
			"\"With a torch.\"\n"
			"\"Ah, well the lights had probably gone.\"\n"
			"\"So had the stairs.\"\n"
			"\"But look, you found the notice, didn't you?\"\n"
			"\"Yes,\" said Arthur, \"yes I did. It was on display in the bottom of a locked"
			" filing cabinet stuck in a disused lavatory with a sign on the door saying"
			" Beware of the Leopard.\"\n";

		return text;
	}
};
