#include <catch2/catch.hpp>

#include "TextAccess.h"

namespace
{
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
}	 // namespace

TEST_CASE("textAccessString constructor")
{
	std::string text = getTestText();

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);

	REQUIRE(textAccess.get() != nullptr);
}

TEST_CASE("textAccessString lines count")
{
	std::string text = getTestText();
	unsigned int lineCount = 8;

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);

	REQUIRE(textAccess->getLineCount() == lineCount);
}

TEST_CASE("textAccessString lines content")
{
	std::string text = getTestText();

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
	std::vector<std::string> lines = textAccess->getLines(1, 4);

	REQUIRE(lines.size() == 4);
	REQUIRE(lines[0] == "\"But the plans were on display . . .\"\n");
	REQUIRE(lines[1] == "\"On display? I eventually had to go down to the cellar to find them.\"\n");
	REQUIRE(lines[2] == "\"That's the display department.\"\n");
	REQUIRE(lines[3] == "\"With a torch.\"\n");
}

TEST_CASE("textAccessString lines content error handling")
{
	std::string text = getTestText();

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
	std::vector<std::string> lines = textAccess->getLines(3, 2);

	REQUIRE(lines.size() == 0);

	lines = textAccess->getLines(10, 3);

	REQUIRE(lines.size() == 0);

	lines = textAccess->getLines(1, 10);

	REQUIRE(lines.size() == 0);

	std::string line = textAccess->getLine(0);

	REQUIRE(line == "");

	lines = textAccess->getLines(0, 2);

	REQUIRE(line == "");
}

TEST_CASE("textAccessString single line content")
{
	std::string text = getTestText();

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
	std::string line = textAccess->getLine(6);

	REQUIRE(line == "\"So had the stairs.\"\n");
}

TEST_CASE("textAccessString all lines")
{
	std::string text = getTestText();
	unsigned int lineCount = 8;

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(text);
	std::vector<std::string> lines = textAccess->getAllLines();

	REQUIRE(lines.size() == lineCount);
}

TEST_CASE("textAccessFile constructor")
{
	FilePath filePath(L"data/TextAccessTestSuite/text.txt");

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

	REQUIRE(textAccess.get() != nullptr);
}

TEST_CASE("textAccessFile lines count")
{
	FilePath filePath(L"data/TextAccessTestSuite/text.txt");
	unsigned int lineCount = 7;

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

	REQUIRE(textAccess->getLineCount() == lineCount);
}

TEST_CASE("textAccessFile lines content")
{
	FilePath filePath(L"data/TextAccessTestSuite/text.txt");

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);
	std::vector<std::string> lines = textAccess->getLines(1, 4);

	REQUIRE(lines.size() == 4);
	REQUIRE(
		lines[0] ==
		"\"If you're a researcher on this book thing and you were on Earth, you must have been "
		"gathering material on it.\"\n");
	REQUIRE(lines[1] == "\"Well, I was able to extend the original entry a bit, yes.\"\n");
	REQUIRE(lines[2] == "\"Let me see what it says in this edition, then. I've got to see it.\"\n");
	REQUIRE(
		lines[3] ==
		"... \"What? Harmless! Is that all it's got to say? Harmless! One word! ... Well, for "
		"God's sake I hope you managed to recitify that a bit.\"\n");
}

TEST_CASE("textAccessFile get filePath")
{
	FilePath filePath(L"data/TextAccessTestSuite/text.txt");
	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

	REQUIRE(textAccess->getFilePath() == filePath);
}
