#include "QtHighlighter.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include "ColorScheme.h"
#include "FileSystem.h"
#include "ResourcePaths.h"
#include "TextAccess.h"
#include "tracing.h"
#include "utility.h"

std::map<std::wstring, std::vector<QtHighlighter::HighlightingRule>> QtHighlighter::s_highlightingRules;
QTextCharFormat QtHighlighter::s_textFormat;

std::string QtHighlighter::highlightTypeToString(QtHighlighter::HighlightType type)
{
	switch (type)
	{
		case HighlightType::COMMENT: return "comment";
		case HighlightType::DIRECTIVE: return "directive";
		case HighlightType::FUNCTION: return "function";
		case HighlightType::KEYWORD: return "keyword";
		case HighlightType::NUMBER: return "number";
		case HighlightType::QUOTATION: return "quotation";
		case HighlightType::TEXT: return "text";
		case HighlightType::TYPE: return "type";
	}
	return "text";
}

QtHighlighter::HighlightType QtHighlighter::highlightTypeFromString(const std::string typeStr)
{
	const std::array<HighlightType, 8> types = {
		HighlightType::COMMENT,
		HighlightType::DIRECTIVE,
		HighlightType::FUNCTION,
		HighlightType::KEYWORD,
		HighlightType::NUMBER,
		HighlightType::QUOTATION,
		HighlightType::TEXT,
		HighlightType::TYPE
	};

	for (HighlightType type : types)
	{
		if (typeStr == highlightTypeToString(type))
		{
			return type;
		}
	}

	return HighlightType::TEXT;
}

void QtHighlighter::loadHighlightingRules()
{
	ColorScheme* scheme = ColorScheme::getInstance().get();

	std::map<HighlightType, QColor> ruleTypeColors;
	const std::array<HighlightType, 8> types = {
		HighlightType::COMMENT,
		HighlightType::DIRECTIVE,
		HighlightType::FUNCTION,
		HighlightType::KEYWORD,
		HighlightType::NUMBER,
		HighlightType::QUOTATION,
		HighlightType::TEXT,
		HighlightType::TYPE
	};

	for (HighlightType type : types)
	{
		ruleTypeColors.emplace(type, QColor(scheme->getSyntaxColor(highlightTypeToString(type)).c_str()));
	}

	s_textFormat.setForeground(ruleTypeColors[HighlightType::TEXT]);

	for (const FilePath path :
			FileSystem::getFilePathsFromDirectory(ResourcePaths::getSyntaxHighlightingRulesPath(), { L".rules" }))
	{
		std::wstring language = path.withoutExtension().fileName();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(path);

		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(textAccess->getText()).toUtf8(), &error);
		if (doc.isNull() || !doc.isArray())
		{
			LOG_ERROR_STREAM(<< "Highlinghting rules in \"" << path.str() << "\" couldn't be parsed as JSON: "
				"offset " << error.offset << " - " << error.errorString().toStdString());
			continue;
		}

		std::vector<HighlightingRule> rules;

		for (QJsonValueRef value : doc.array())
		{
			if (!value.isObject())
			{
				continue;
			}

			QJsonObject ruleObj = value.toObject();

			HighlightType type = highlightTypeFromString(ruleObj.value("type").toString().toStdString());

			auto colorIt = ruleTypeColors.find(type);
			if (colorIt == ruleTypeColors.end())
			{
				continue;
			}

			bool priority = ruleObj.value("priority").toBool();

			QJsonArray patterns = ruleObj.value("patterns").toArray();
			for (QJsonValueRef pattern : patterns)
			{
				if (pattern.isString())
				{
					rules.push_back(HighlightingRule(type, colorIt->second, QRegExp(pattern.toString()), priority));
				}
			}

			QJsonObject range = ruleObj.value("range").toObject();
			if (!range.empty())
			{
				rules.push_back(HighlightingRule(type, colorIt->second, QRegExp(range.value("start").toString()), priority, true));
				rules.push_back(HighlightingRule(type, colorIt->second, QRegExp(range.value("end").toString()), priority, true));
			}
		}

		s_highlightingRules.emplace(language, rules);
	}
}

void QtHighlighter::clearHighlightingRules()
{
	s_highlightingRules.clear();
}

QtHighlighter::QtHighlighter(QTextDocument *document, const std::wstring& language)
	: m_document(document)
{
	if (!s_highlightingRules.size())
	{
		loadHighlightingRules();
	}

	const auto it = s_highlightingRules.find(language);
	if (it != s_highlightingRules.end())
	{
		m_highlightingRules = it->second;
	}
}

void QtHighlighter::highlightDocument()
{
	TRACE();

	QTextDocument* doc = document();

	size_t docStart = 0;
	size_t docEnd = 0;
	for (int i = 0; i < doc->blockCount(); i++)
	{
		docEnd += doc->findBlockByLineNumber(i).length();
	}

	if (docEnd > 0)
	{
		docEnd -= 1;
	}

	applyFormat(docStart, docEnd, s_textFormat);

	m_highlightedLines.clear();
	m_highlightedLines.resize(document()->blockCount(), false);

	if (m_highlightingRules.empty())
	{
		return;
	}

	std::vector<HighlightingRule> quotationRules;
	for (const HighlightingRule& rule : m_highlightingRules)
	{
		if (rule.priority && rule.type == HighlightType::QUOTATION)
		{
			quotationRules.emplace_back(rule);
		}
	}
	createRanges(doc, quotationRules);
}

void QtHighlighter::highlightRange(int startLine, int endLine)
{
	if (startLine < 0 || endLine < 0 || startLine > endLine || endLine > int(m_highlightedLines.size()))
	{
		return;
	}

	bool hasUnhighlightedLines = false;
	for (int i = startLine; i <= endLine; i++)
	{
		if (!m_highlightedLines[i])
		{
			hasUnhighlightedLines = true;
			break;
		}
	}

	if (!hasUnhighlightedLines)
	{
		return;
	}

	QTextDocument* doc = document();
	QTextBlock start = doc->findBlockByLineNumber(startLine);
	QTextBlock end = doc->findBlockByLineNumber(endLine + 1);

	const HighlightingRule* singleLineCommentRule = nullptr;
	const HighlightingRule* quotationRule = nullptr;

	for (const HighlightingRule& rule : m_highlightingRules)
	{
		if (rule.type == HighlightType::COMMENT && !rule.multiLine)
		{
			singleLineCommentRule = &rule;
		}
		else if (rule.type == HighlightType::QUOTATION)
		{
			quotationRule = &rule;
		}
	}

	int index = startLine;
	for (QTextBlock it = start; it != end; it = it.next())
	{
		if (!m_highlightedLines[index])
		{
			applyFormat(it.position(), it.position() + it.length() - 1, s_textFormat);

			for (const HighlightingRule &rule : m_highlightingRules)
			{
				if (!rule.priority && rule.type != HighlightType::COMMENT)
				{
					formatBlockForRule(it, rule);
				}
			}

			if (quotationRule)
			{
				formatBlockIfInRange(it, quotationRule->format, &m_quotationRanges);
			}

			if (singleLineCommentRule)
			{
				formatBlockForRule(it, *singleLineCommentRule, &m_quotationRanges);
				formatBlockIfInRange(it, singleLineCommentRule->format, &m_multiLineCommentRanges);
			}
		}
		index++;
	}

	for (int i = startLine; i <= endLine; i++)
	{
		m_highlightedLines[i] = true;
	}
}

void QtHighlighter::rehighlightLines(const std::vector<int>& lines)
{
	for (int line : lines)
	{
		if (line >= 0 && line < int(m_highlightedLines.size()))
		{
			m_highlightedLines[line] = false;
		}
	}
}

void QtHighlighter::applyFormat(int startPosition, int endPosition, const QTextCharFormat& format)
{
	QTextCursor cursor(document());
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
	cursor.setCharFormat(format);
}

QTextCharFormat QtHighlighter::getFormat(int startPosition, int endPosition) const
{
	QTextCursor cursor(document());
	cursor.setPosition(endPosition);
	return cursor.charFormat();
}

void QtHighlighter::createRanges(
	QTextDocument* doc, const std::vector<HighlightingRule>& quotationRules)
{
	m_quotationRanges.clear();
	m_multiLineCommentRanges.clear();

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		for (const HighlightingRule& rule : quotationRules)
		{
			utility::append(m_quotationRanges, getRangesForRule(it, rule));
		}
	}

	m_multiLineCommentRanges = createMultiLineCommentRanges(doc, &m_quotationRanges);
}

std::vector<std::pair<int, int>> QtHighlighter::createMultiLineCommentRanges(
	QTextDocument* doc, std::vector<std::pair<int, int>>* ranges)
{
	const HighlightingRule* multiLineCommentStartRule = nullptr;
	const HighlightingRule* multiLineCommentEndRule = nullptr;
	const HighlightingRule* singleLineCommentRule = nullptr;

	for (const HighlightingRule& rule : m_highlightingRules)
	{
		if (rule.type == HighlightType::COMMENT)
		{
			if (rule.priority && rule.multiLine)
			{
				if (!multiLineCommentStartRule)
				{
					multiLineCommentStartRule = &rule;
				}
				else if (!multiLineCommentEndRule)
				{
					multiLineCommentEndRule = &rule;
				}
			}
			else if (!rule.multiLine)
			{
				singleLineCommentRule = &rule;
			}
		}
	}

	std::vector<std::pair<int, int>> multiLineCommentRanges;
	if (!multiLineCommentStartRule || !multiLineCommentEndRule)
	{
		return multiLineCommentRanges;
	}

	QTextCursor cursorStart(doc);
	QTextCursor cursorEnd(doc);

	while (true)
	{
		while (true)
		{
			cursorStart = document()->find(multiLineCommentStartRule->pattern, cursorStart);
			if (cursorStart.isNull())
			{
				break;
			}

			// ignore if within single line comment
			if (singleLineCommentRule)
			{
				QTextCursor inlineCommentStart = document()->find(singleLineCommentRule->pattern, QTextCursor(cursorStart.block()));
				if (!inlineCommentStart.isNull() &&
					inlineCommentStart.blockNumber() == cursorStart.blockNumber() &&
					inlineCommentStart.selectionStart() < cursorStart.selectionStart())
				{
					cursorStart = QTextCursor(inlineCommentStart.block().next());
					continue;
				}
			}

			if (!isInRange(cursorStart.selectionEnd(), *ranges))
			{
				cursorStart.setPosition(cursorStart.selectionEnd() - 2);
				break;
			}
			else
			{
				cursorStart.setPosition(cursorStart.selectionEnd() + 2);
			}
		}

		if (cursorStart.isNull())
		{
			break;
		}

		cursorEnd = document()->find(multiLineCommentEndRule->pattern, cursorStart);
		if (cursorEnd.isNull())
		{
			break;
		}

		multiLineCommentRanges.push_back(std::pair<int, int>(cursorStart.position(), cursorEnd.position()));
		cursorStart = cursorEnd;
	}

	return multiLineCommentRanges;
}

QtHighlighter::HighlightingRule::HighlightingRule()
{
}

QtHighlighter::HighlightingRule::HighlightingRule(
	HighlightType type, const QColor& color, const QRegExp& regExp, bool priority, bool multiLine
)
	: type(type)
	, pattern(regExp)
	, priority(priority)
	, multiLine(multiLine)
{
	format.setForeground(color);
}

bool QtHighlighter::isInRange(int pos, const std::vector<std::pair<int, int>>& ranges) const
{
	for (const std::pair<int, int> p : ranges)
	{
		if (pos >= p.first && pos <= p.second)
		{
			return true;
		}
	}

	return false;
}

std::vector<std::pair<int, int>> QtHighlighter::getRangesForRule(
	const QTextBlock& block, const HighlightingRule& rule) const
{
	QRegExp expression(rule.pattern);
	int pos = block.position();
	int index = expression.indexIn(block.text());

	std::vector<std::pair<int, int>> ranges;

	while (index >= 0)
	{
		int length = expression.matchedLength();

		ranges.push_back(std::pair<int, int>(pos + index, pos + index + length));

		index = expression.indexIn(block.text(), index + length);
	}

	return ranges;
}

void QtHighlighter::formatBlockForRule(
	const QTextBlock& block, const HighlightingRule& rule, std::vector<std::pair<int, int>>* ranges
){
	QRegExp expression(rule.pattern);
	int pos = block.position();
	int index = expression.indexIn(block.text());

	while (index >= 0)
	{
		int length = expression.matchedLength();

		if (!ranges || !isInRange(pos + index, *ranges))
		{
			applyFormat(pos + index, pos + index + length, rule.format);
		}

		index = expression.indexIn(block.text(), index + length);
	}
}

void QtHighlighter::formatBlockIfInRange(
	const QTextBlock& block, const QTextCharFormat& format, std::vector<std::pair<int, int>>* ranges
){
	int startPos = block.position();
	int endPos = startPos + block.length() - 1;

	for (auto range : *ranges)
	{
		int start = std::max(range.first, startPos);
		int end = std::min(range.second, endPos);

		if (start <= end)
		{
			applyFormat(start, end, format);
		}
	}
}

QTextDocument* QtHighlighter::document() const
{
	return m_document;
}
