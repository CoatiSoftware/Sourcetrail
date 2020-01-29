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
#include "logging.h"
#include "tracing.h"
#include "utility.h"

std::map<std::wstring, std::vector<QtHighlighter::HighlightingRule>> QtHighlighter::s_highlightingRules;
std::map<QtHighlighter::HighlightType, QTextCharFormat> QtHighlighter::s_charFormats;

std::string QtHighlighter::highlightTypeToString(QtHighlighter::HighlightType type)
{
	switch (type)
	{
	case HighlightType::COMMENT:
		return "comment";
	case HighlightType::DIRECTIVE:
		return "directive";
	case HighlightType::FUNCTION:
		return "function";
	case HighlightType::KEYWORD:
		return "keyword";
	case HighlightType::NUMBER:
		return "number";
	case HighlightType::QUOTATION:
		return "quotation";
	case HighlightType::TEXT:
		return "text";
	case HighlightType::TYPE:
		return "type";
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
		HighlightType::TYPE};

	for (HighlightType type: types)
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

	const std::array<HighlightType, 8> types = {
		HighlightType::COMMENT,
		HighlightType::DIRECTIVE,
		HighlightType::FUNCTION,
		HighlightType::KEYWORD,
		HighlightType::NUMBER,
		HighlightType::QUOTATION,
		HighlightType::TEXT,
		HighlightType::TYPE};

	s_charFormats.clear();
	for (HighlightType type: types)
	{
		QTextCharFormat format;
		format.setForeground(QColor(scheme->getSyntaxColor(highlightTypeToString(type)).c_str()));
		s_charFormats.emplace(type, format);
	}

	for (const FilePath path: FileSystem::getFilePathsFromDirectory(
			 ResourcePaths::getSyntaxHighlightingRulesPath(), {L".rules"}))
	{
		std::wstring language = path.withoutExtension().fileName();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(path);

		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(
			QString::fromStdString(textAccess->getText()).toUtf8(), &error);
		if (doc.isNull() || !doc.isArray())
		{
			LOG_ERROR_STREAM(
				<< "Highlinghting rules in \"" << path.str()
				<< "\" couldn't be parsed as JSON: "
				   "offset "
				<< error.offset << " - " << error.errorString().toStdString());
			continue;
		}

		std::vector<HighlightingRule> rules;

		for (QJsonValueRef value: doc.array())
		{
			if (!value.isObject())
			{
				continue;
			}

			QJsonObject ruleObj = value.toObject();

			HighlightType type = highlightTypeFromString(
				ruleObj.value(QStringLiteral("type")).toString().toStdString());

			bool priority = ruleObj.value(QStringLiteral("priority")).toBool();

			QJsonArray patterns = ruleObj.value(QStringLiteral("patterns")).toArray();
			for (QJsonValueRef pattern: patterns)
			{
				if (pattern.isString())
				{
					rules.push_back(HighlightingRule(type, QRegExp(pattern.toString()), priority));
				}
			}

			QJsonObject range = ruleObj.value(QStringLiteral("range")).toObject();
			if (!range.empty())
			{
				rules.push_back(HighlightingRule(
					type, QRegExp(range.value("start").toString()), priority, true));
				rules.push_back(
					HighlightingRule(type, QRegExp(range.value("end").toString()), priority, true));
			}
		}

		s_highlightingRules.emplace(language, rules);
	}
}

void QtHighlighter::clearHighlightingRules()
{
	s_highlightingRules.clear();
}

QtHighlighter::QtHighlighter(QTextDocument* document, const std::wstring& language)
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

	applyFormat(docStart, docEnd, s_charFormats[HighlightType::TEXT]);

	m_highlightedLines.clear();
	m_highlightedLines.resize(document()->blockCount(), false);

	if (m_highlightingRules.empty())
	{
		return;
	}

	std::vector<HighlightingRule> singleLineRules;
	for (const HighlightingRule& rule: m_highlightingRules)
	{
		if (rule.priority && !rule.multiLine)
		{
			singleLineRules.emplace_back(rule);
		}
	}
	createRanges(doc, singleLineRules);
}

void QtHighlighter::highlightRange(int startLine, int endLine)
{
	if (startLine < 0 || endLine < 0 || startLine > endLine ||
		endLine > int(m_highlightedLines.size()))
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

	for (const HighlightingRule& rule: m_highlightingRules)
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
			applyFormat(
				it.position(), it.position() + it.length() - 1, s_charFormats[HighlightType::TEXT]);

			for (const HighlightingRule& rule: m_highlightingRules)
			{
				if (rule.multiLine)
				{
					continue;
				}

				if (rule.priority)
				{
					formatBlockIfInRange(it, rule.type, &m_singleLineRanges);
				}
				else
				{
					formatBlockForRule(it, rule, &m_singleLineRanges);
				}
			}

			if (m_multiLineRanges.size())
			{
				formatBlockIfInRange(it, &m_multiLineRanges);
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
	for (int line: lines)
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

void QtHighlighter::createRanges(QTextDocument* doc, const std::vector<HighlightingRule>& singleLineRules)
{
	m_singleLineRanges.clear();
	m_multiLineRanges.clear();

	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
	{
		for (const HighlightingRule& rule: singleLineRules)
		{
			utility::append(m_singleLineRanges, getRangesForRule(it, rule));
		}
	}

	// remove ranges starting inside others
	{
		std::map<std::pair<int, int>, size_t> sortedRangesToIndex;
		for (size_t i = 0; i < m_singleLineRanges.size(); i++)
		{
			const std::tuple<HighlightType, int, int>& range = m_singleLineRanges[i];
			sortedRangesToIndex.emplace(std::make_pair(std::get<1>(range), std::get<2>(range)), i);
		}

		std::set<size_t> indicesToErase;
		const std::pair<int, int>* topRange = nullptr;
		for (const auto& p: sortedRangesToIndex)
		{
			if (topRange && p.first.first <= topRange->second)
			{
				indicesToErase.insert(p.second);
			}
			else
			{
				topRange = &p.first;
			}
		}

		for (std::set<size_t>::const_reverse_iterator it = indicesToErase.rbegin();
			 it != indicesToErase.rend();
			 it++)
		{
			m_singleLineRanges.erase(m_singleLineRanges.begin() + *it);
		}
	}

	m_multiLineRanges = createMultiLineRanges(doc, &m_singleLineRanges);
}

std::vector<std::tuple<QtHighlighter::HighlightType, int, int>> QtHighlighter::createMultiLineRanges(
	QTextDocument* doc, std::vector<std::tuple<HighlightType, int, int>>* ranges)
{
	std::vector<std::tuple<HighlightType, int, int>> multiLineRanges;

	const HighlightingRule* startRule = nullptr;

	for (const HighlightingRule& rule: m_highlightingRules)
	{
		if (rule.priority && rule.multiLine)
		{
			if (!startRule)
			{
				startRule = &rule;
			}
			else if (rule.type == startRule->type)
			{
				utility::append(
					multiLineRanges, createMultiLineRangesForRules(doc, ranges, startRule, &rule));
				startRule = nullptr;
			}
		}
	}

	return multiLineRanges;
}

std::vector<std::tuple<QtHighlighter::HighlightType, int, int>> QtHighlighter::createMultiLineRangesForRules(
	QTextDocument* doc,
	std::vector<std::tuple<HighlightType, int, int>>* ranges,
	const HighlightingRule* startRule,
	const HighlightingRule* endRule)
{
	std::vector<std::tuple<HighlightType, int, int>> multiLineRanges;

	QTextCursor cursorStart(doc);
	QTextCursor cursorEnd(doc);

	while (true)
	{
		while (true)
		{
			cursorStart = document()->find(startRule->pattern, cursorStart);
			if (cursorStart.isNull())
			{
				break;
			}

			if (!isInRange(cursorStart.selectionEnd() - 1, *ranges))
			{
				break;
			}
			else
			{
				cursorStart.setPosition(cursorStart.selectionEnd() + 1);
			}
		}

		if (cursorStart.isNull())
		{
			break;
		}

		cursorEnd = document()->find(endRule->pattern, cursorStart);
		if (cursorEnd.isNull())
		{
			break;
		}

		multiLineRanges.emplace_back(
			std::make_tuple(startRule->type, cursorStart.selectionStart(), cursorEnd.position()));

		cursorStart = cursorEnd;
	}

	return multiLineRanges;
}

QtHighlighter::HighlightingRule::HighlightingRule() {}

QtHighlighter::HighlightingRule::HighlightingRule(
	HighlightType type, const QRegExp& regExp, bool priority, bool multiLine)
	: type(type), pattern(regExp), priority(priority), multiLine(multiLine)
{
}

bool QtHighlighter::isInRange(int pos, const std::vector<std::tuple<HighlightType, int, int>>& ranges) const
{
	for (const std::tuple<HighlightType, int, int>& range: ranges)
	{
		if (pos >= std::get<1>(range) && pos <= std::get<2>(range))
		{
			return true;
		}
	}

	return false;
}

std::vector<std::tuple<QtHighlighter::HighlightType, int, int>> QtHighlighter::getRangesForRule(
	const QTextBlock& block, const HighlightingRule& rule) const
{
	const int pos = block.position();
	const QString text = block.text();
	QRegExp expression(rule.pattern);
	int index = expression.indexIn(text);

	std::vector<std::tuple<HighlightType, int, int>> ranges;

	while (index >= 0)
	{
		const int length = expression.matchedLength();
		if (expression.capturedTexts().size() > 1)
		{
			const QString cap = expression.capturedTexts()[1];
			const int start = text.indexOf(cap, index);
			ranges.push_back(std::make_tuple(rule.type, pos + start, pos + start + cap.length()));
		}
		else
		{
			ranges.push_back(std::make_tuple(rule.type, pos + index, pos + index + length));
		}
		index = expression.indexIn(block.text(), index + length);
	}

	return ranges;
}

void QtHighlighter::formatBlockForRule(
	const QTextBlock& block,
	const HighlightingRule& rule,
	std::vector<std::tuple<HighlightType, int, int>>* ranges)
{
	if (s_charFormats.find(rule.type) == s_charFormats.end())
	{
		return;
	}

	const QTextCharFormat& format = s_charFormats.find(rule.type)->second;

	QRegExp expression(rule.pattern);
	int pos = block.position();
	int index = expression.indexIn(block.text());

	while (index >= 0)
	{
		int length = expression.matchedLength();

		if (!isInRange(pos + index, *ranges))
		{
			applyFormat(pos + index, pos + index + length, format);
		}

		index = expression.indexIn(block.text(), index + length);
	}
}

void QtHighlighter::formatBlockIfInRange(
	const QTextBlock& block,
	HighlightType type,
	std::vector<std::tuple<HighlightType, int, int>>* ranges)
{
	int startPos = block.position();
	int endPos = startPos + block.length() - 1;

	if (s_charFormats.find(type) == s_charFormats.end())
	{
		return;
	}

	const QTextCharFormat& format = s_charFormats.find(type)->second;

	for (auto range: *ranges)
	{
		if (type == std::get<0>(range))
		{
			int start = std::max(std::get<1>(range), startPos);
			int end = std::min(std::get<2>(range), endPos);

			if (start <= end)
			{
				applyFormat(start, end, format);
			}
		}
	}
}

void QtHighlighter::formatBlockIfInRange(
	const QTextBlock& block, std::vector<std::tuple<HighlightType, int, int>>* ranges)
{
	int startPos = block.position();
	int endPos = startPos + block.length() - 1;

	for (auto range: *ranges)
	{
		HighlightType type = std::get<0>(range);
		if (s_charFormats.find(type) == s_charFormats.end())
		{
			continue;
		}

		const QTextCharFormat& format = s_charFormats.find(type)->second;

		int start = std::max(std::get<1>(range), startPos);
		int end = std::min(std::get<2>(range), endPos);

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
