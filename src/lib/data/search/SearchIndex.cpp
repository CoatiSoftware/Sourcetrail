#include "data/search/SearchIndex.h"

#include <algorithm>
#include <ctype.h>
#include <iterator>

#include "utility/utility.h"
#include "utility/utilityString.h"

SearchIndex::SearchIndex()
{
	clear();
}

SearchIndex::~SearchIndex()
{
}

void SearchIndex::addNode(Id id, const std::string& name, Node::NodeTypeMask type)
{
	SearchNode* currentNode = m_root;

	std::string remaining = name;
	while (remaining.size() > 0)
	{
		auto it = currentNode->edges.find(remaining[0]);
		if (it != currentNode->edges.end())
		{
			SearchEdge* currentEdge = it->second;
			const std::string& edgeString = currentEdge->s;

			size_t matchCount = 1;
			for (size_t j = 1; j < edgeString.size() && j < remaining.size(); j++)
			{
				if (edgeString[j] != remaining[j])
				{
					break;
				}
				matchCount++;
			}

			if (matchCount < edgeString.size())
			{
				// split current edge
				std::shared_ptr<SearchNode> n = std::make_shared<SearchNode>();
				m_nodes.push_back(n);
				std::shared_ptr<SearchEdge> e = std::make_shared<SearchEdge>();
				m_edges.push_back(e);

				e->s = edgeString.substr(matchCount);
				e->target = currentEdge->target;

				n->edges.emplace(e->s[0], e.get());

				currentEdge->s = edgeString.substr(0, matchCount);
				currentEdge->target = n.get();
			}

			remaining = remaining.substr(matchCount);
			currentNode = currentEdge->target;
		}
		else
		{
			std::shared_ptr<SearchNode> n = std::make_shared<SearchNode>();
			m_nodes.push_back(n);
			std::shared_ptr<SearchEdge> e = std::make_shared<SearchEdge>();
			m_edges.push_back(e);

			e->s = remaining;
			e->target = n.get();

			currentNode->edges.emplace(e->s[0], e.get());
			currentNode = n.get();

			remaining = "";
		}
	}

	currentNode->elementIds.insert(id);
	currentNode->mask |= type;
}

void SearchIndex::finishSetup()
{
	for (auto p : m_root->edges)
	{
		populateEdgeGate(p.second);
	}
}

void SearchIndex::clear()
{
	m_nodes.clear();
	m_edges.clear();

	std::shared_ptr<SearchNode> n = std::make_shared<SearchNode>();
	m_nodes.push_back(n);

	m_root = n.get();
}

std::vector<SearchResult> SearchIndex::search(
	const std::string& query, Node::NodeTypeMask filter, size_t maxResultCount, size_t maxBestScoredResultsLength) const
{
	// find paths containing query
	SearchPath startPath;
	startPath.node = m_root;

	std::vector<SearchPath> paths;
	searchRecursive(startPath, utility::toLowerCase(query), filter, &paths);

	// create scored search results
	std::multiset<SearchResult> searchResults = createScoredResults(paths, filter, maxResultCount * 3);

	// find best scores
	std::map<std::string, SearchResult> scoresCache;
	std::multiset<SearchResult> bestResults;
	for (const SearchResult& result : searchResults)
	{
		bestResults.insert(bestScoredResult(result, &scoresCache, maxBestScoredResultsLength));
	}

	// narrow down to max result count
	auto it = bestResults.end();
	if (maxResultCount && bestResults.size() > maxResultCount)
	{
		it = bestResults.begin();
		std::advance(it, maxResultCount);
	}

	return std::vector<SearchResult>(bestResults.begin(), it);
}

void SearchIndex::populateEdgeGate(SearchEdge* e)
{
	SearchNode* target = e->target;
	for (auto p : target->edges)
	{
		SearchEdge* targetEdge = p.second;
		populateEdgeGate(targetEdge);
		utility::append(e->gate, targetEdge->gate);
	}
	for (size_t i = 0; i < e->s.size(); i++)
	{
		e->gate.insert(tolower(e->s[i]));
	}
}

void SearchIndex::searchRecursive(
	const SearchPath& path, const std::string& remainingQuery, Node::NodeTypeMask filter,
	std::vector<SearchIndex::SearchPath>* results) const
{
	if (remainingQuery.size() == 0 && (!filter || (path.node->mask & filter)))
	{
		results->push_back(path);
		return;
	}

	for (auto p : path.node->edges)
	{
		const SearchEdge* currentEdge = p.second;

		// test if s passes the edge's gate.
		bool passesGate = true;
		for (const char& c : remainingQuery)
		{
			if (currentEdge->gate.find(c) == currentEdge->gate.end())
			{
				passesGate = false;
				break;
			}
		}

		if (passesGate)
		{
			// consume characters for edge
			const std::string& edgeString = currentEdge->s;
			std::vector<size_t> indices = path.indices;

			size_t j = 0;
			for (size_t i = 0; i < edgeString.size() && j < remainingQuery.size(); i++)
			{
				if (tolower(edgeString[i]) == remainingQuery[j])
				{
					indices.push_back(path.text.size() + i);
					j++;
				}
			}

			SearchPath currentPath;
			currentPath.node = currentEdge->target;
			currentPath.indices = indices;
			currentPath.text = path.text + edgeString;

			searchRecursive(currentPath, remainingQuery.substr(j), filter, results);
		}
	}
}

std::multiset<SearchResult> SearchIndex::createScoredResults(
	const std::vector<SearchPath>& paths, Node::NodeTypeMask filter, size_t maxResultCount) const
{
	// score and order initial paths
	std::multimap<int, SearchPath, std::greater<int>> scoredPaths;
	for (const SearchPath& path : paths)
	{
		scoredPaths.emplace(scoreText(path.text, path.indices), path);
	}

	// score paths and subpaths
	std::multiset<SearchResult> searchResults;
	for (const std::pair<int, SearchPath>& p : scoredPaths)
	{
		std::vector<SearchPath> currentPaths;
		currentPaths.push_back(p.second);

		while (currentPaths.size())
		{
			std::vector<SearchPath> nextPaths;

			for (const SearchPath& path : currentPaths)
			{
				if (path.node->elementIds.size() && (!filter || (path.node->mask & filter)))
				{
					SearchResult result;
					result.text = path.text;
					result.elementIds = path.node->elementIds;
					result.indices = path.indices;
					result.score = scoreText(path.text, path.indices);
					searchResults.insert(result);

					if (maxResultCount && searchResults.size() >= maxResultCount)
					{
						return searchResults;
					}
				}

				for (auto p : path.node->edges)
				{
					const SearchEdge* edge = p.second;
					SearchPath nextPath;
					nextPath.indices = path.indices;
					nextPath.node = edge->target;
					nextPath.text = path.text + edge->s;
					nextPaths.push_back(nextPath);
				}
			}

			currentPaths = nextPaths;
		}
	}

	return searchResults;
}

SearchResult SearchIndex::bestScoredResult(
	SearchResult result, std::map<std::string, SearchResult>* scoresCache, size_t maxBestScoredResultsLength)
{
	std::string text = result.text;

	if (maxBestScoredResultsLength && result.text.size() > maxBestScoredResultsLength)
	{
		if (result.indices.back() >= maxBestScoredResultsLength)
		{
			return result;
		}

		result.text = result.text.substr(0, maxBestScoredResultsLength);
	}

	auto it = scoresCache->find(result.text);
	if (it != scoresCache->end())
	{
		// std::cout << "cached: " << it->first << " " << it->second.score << std::endl;
		SearchResult result = it->second;
		result.text = text;
		return result;
	}

	const std::vector<size_t> indices = result.indices;
	bestScoredResultRecursive(utility::toLowerCase(result.text), indices, indices.size() - 1, scoresCache, &result);

	// std::cout << "save: " << result.text << " " << result.score << std::endl;
	scoresCache->emplace(result.text, result);

	result.text = text;

	return result;
}

void SearchIndex::bestScoredResultRecursive(
	const std::string& lowerText, const std::vector<size_t>& indices, const size_t indicesPos,
	std::map<std::string, SearchResult>* scoresCache, SearchResult* result)
{
	// left for debugging
	// std::cout << lowerText << std::endl;
	// size_t idx = 0;
	// for (size_t i = 0; i < lowerText.size() && idx < indices.size(); i++)
	// {
	// 	if (i == indices[idx])
	// 	{
	// 		if (idx == indicesPos)
	// 			std::cout << "I";
	// 		else
	// 			std::cout << "^";
	// 		idx++;
	// 	}
	// 	else
	// 		std::cout << " ";
	// }
	// std::cout << "\n" << std::endl;

	size_t oldTextPos = indices[indicesPos];
	size_t nextTextPos = (indicesPos + 1 == indices.size() ? lowerText.size() : indices[indicesPos + 1]);
	bool foundMatch = false;

	for (size_t i = oldTextPos + 1; i < nextTextPos; i++)
	{
		if (lowerText[i] == lowerText[oldTextPos])
		{
			foundMatch = true;

			std::vector<size_t> newIndices = indices;
			newIndices[indicesPos] = i;

			int newScore = scoreText(result->text, newIndices);
			if (newScore > result->score)
			{
				result->score = newScore;
				result->indices = newIndices;
			}

			bestScoredResultRecursive(lowerText, newIndices, indicesPos, scoresCache, result);
			break;
		}
	}

	if (indicesPos + 1 == indices.size())
	{
		std::map<std::string, SearchResult>::const_iterator it = scoresCache->find(result->text.substr(0, indices.back() + 1));
		if (it != scoresCache->end())
		{
			// std::cout << "cached: " << it->first << " " << it->second.score << std::endl;
			result->score = it->second.score;
			result->indices = it->second.indices;
			return;
		}
	}

	size_t newIndicesPos = indicesPos;
	while (newIndicesPos > 0)
	{
		newIndicesPos--;

		if (indices[newIndicesPos + 1] - indices[newIndicesPos] > 1)
		{
			bestScoredResultRecursive(lowerText, indices, newIndicesPos, scoresCache, result);
			break;
		}
	}

	if (indicesPos == 0 && !foundMatch)
	{
		// std::cout << "save: " << result->text.substr(0, indices.back() + 1) << " " << result->score << std::endl;
		scoresCache->emplace(result->text.substr(0, indices.back() + 1), *result);
	}
}

int SearchIndex::scoreText(const std::string& text, const std::vector<size_t>& indices)
{
	const int unmatchedLetterBonus = -1;
	const int consecutiveLetterBonus = 4;
	const int camelCaseBonus = 3;
	const int noLetterBonus = 4;
	const int firstLetterBonus = 4;
	const int delayedStartBonus = -1;
	const int minDelayedStartBonus = -20;

	int unmatchedLetterScore = 0;
	int consecutiveLetterScore = 0;
	int camelCaseScore = 0;
	int noLetterScore = 0;
	int firstLetterScore = 0;

	static std::set<char> noLetters;
	if (!noLetters.size())
	{
		noLetters.insert(' ');
		noLetters.insert('.');
		noLetters.insert(',');
		noLetters.insert('_');
		noLetters.insert(':');
		noLetters.insert('<');
		noLetters.insert('>');
		noLetters.insert('/');
		noLetters.insert('\\');
	}

	for (size_t i = 0; i < indices.size(); i++)
	{
		// unmatched and consecutive
		if (i > 0)
		{
			unmatchedLetterScore += (indices[i] - indices[i - 1] - 1) * unmatchedLetterBonus;
			consecutiveLetterScore += (indices[i] - indices[i - 1] == 1) ? consecutiveLetterBonus : 0;
		}

		size_t index = indices[i];

		// first letter
		if (index == 0)
		{
			firstLetterScore += firstLetterBonus;
		}
		// after no letter
		else if ((index != 0 && noLetters.find(text[index - 1]) != noLetters.end()))
		{
			noLetterScore += noLetterBonus;
		}
		// camel case
		else if (isupper(text[index]))
		{
			bool prevIsLower = (index > 0 && islower(text[index - 1]));
			bool nextIsLower = (index + 1 < text.size() && islower(text[index + 1]));

			if (prevIsLower || nextIsLower)
			{
				camelCaseScore += camelCaseBonus;
			}
		}
	}

	int leadingStartScore = std::max(int(indices[0]) * delayedStartBonus, minDelayedStartBonus);

	int score =
		unmatchedLetterScore +
		consecutiveLetterScore +
		camelCaseScore +
		noLetterScore +
		firstLetterScore +
		leadingStartScore;

	// left for debugging
	// std::cout << unmatchedLetterScore << " " << consecutiveLetterScore << " " << camelCaseScore << " " << noLetterScore;
	// std::cout << " " << firstLetterScore << " " << leadingStartScore << " - " << score << " " << text << std::endl;

	return score;
}

SearchResult SearchIndex::rescoreText(
	const std::string& fulltext,
	const std::string& text,
	const std::vector<size_t>& indices,
	int score,
	size_t maxBestScoredResultsLength)
{
	SearchResult result;
	result.text = text;
	result.score = score;
	result.indices = indices;

	std::vector<size_t> textIndices;

	// match is already within text
	int newIdx = indices[0] - (fulltext.size() - text.size());
	if (newIdx >= 0)
	{
		for (size_t idx : indices)
		{
			textIndices.push_back(idx - (fulltext.size() - text.size()));
		}
	}
	// try if match is within text
	else
	{
		size_t idx = 0;
		size_t textSize = text.size();
		if (maxBestScoredResultsLength && textSize > maxBestScoredResultsLength)
		{
			textSize = maxBestScoredResultsLength;
		}

		for (size_t i = 0; i < textSize && idx < indices.size(); i++)
		{
			if (tolower(text[i]) == tolower(fulltext[indices[idx]]))
			{
				textIndices.push_back(i);
				idx++;
			}
		}

		// match was not found
		if (idx != indices.size())
		{
			result.score -= 1;
			return result;
		}
	}

	result.score = scoreText(text, textIndices);
	result.indices = textIndices;

	std::map<std::string, SearchResult> scoresCache;
	result = bestScoredResult(result, &scoresCache, maxBestScoredResultsLength);

	for (size_t i = 0; i < result.indices.size(); i++)
	{
		result.indices[i] += fulltext.size() - text.size();
	}

	return result;
}
