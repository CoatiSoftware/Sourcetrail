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

void SearchIndex::addNode(Id id, const NameHierarchy& nameHierarchy)
{
	Node* currentNode = m_root;

	// we don't use the signature here, so elements with the same signature share the same node in the search index.
	std::string remaining = nameHierarchy.getQualifiedName();

	while (remaining.size() > 0)
	{
		bool matchingEdgeFound = false;
		// has edge starting with c?
		for (size_t i = 0; i < currentNode->edges.size(); i++)
		{
			Edge* currentEdge = currentNode->edges[i];
			const std::string& edgeString = currentEdge->s;

			size_t matchCount = 0;
			for (size_t j = 0; j < edgeString.size() && j < remaining.size(); j++)
			{
				if (edgeString[j] != remaining[j])
				{
					break;
				}
				matchCount++;
			}

			if (matchCount != 0)
			{
				remaining = remaining.substr(matchCount);
				if (matchCount < edgeString.size())
				{
					// split current edge
					std::shared_ptr<Node> n = std::make_shared<Node>();
					m_nodes.push_back(n);
					std::shared_ptr<Edge> e = std::make_shared<Edge>();
					m_edges.push_back(e);

					n->edges.push_back(e.get());

					e->s = edgeString.substr(matchCount);
					e->target = currentEdge->target;

					currentEdge->s = edgeString.substr(0, matchCount);
					currentEdge->target = n.get();
				}
				currentNode = currentEdge->target;
				matchingEdgeFound = true;
				break;
			}
		}

		if (!matchingEdgeFound)
		{
			std::shared_ptr<Node> n = std::make_shared<Node>();
			m_nodes.push_back(n);
			std::shared_ptr<Edge> e = std::make_shared<Edge>();
			m_edges.push_back(e);

			e->s = remaining;
			e->target = n.get();

			currentNode->edges.push_back(e.get());
			currentNode = n.get();

			remaining = "";
		}
	}

	currentNode->elementIds.insert(id);
}

void SearchIndex::finishSetup()
{
	for (size_t i = 0; i < m_root->edges.size(); i++)
	{
		populateEdgeGate(m_root->edges[i]);
	}
}

void SearchIndex::clear()
{
	m_nodes.clear();
	m_edges.clear();

	std::shared_ptr<Node> n = std::make_shared<Node>();
	m_nodes.push_back(n);

	m_root = n.get();
}

std::vector<SearchResult> SearchIndex::search(const std::string& query, size_t maxResultCount) const
{
	// find paths containing query
	Path startPath;
	startPath.node = m_root;

	std::vector<Path> paths;
	searchRecursive(startPath, utility::toLowerCase(query), &paths);

	// create scored search results
	std::multiset<SearchResult> searchResults = createScoredResults(paths, maxResultCount * 3);

	// find best scores
	std::map<std::string, SearchResult> scoresCache;
	std::multiset<SearchResult> bestResults;
	for (const SearchResult& result : searchResults)
	{
		bestResults.insert(bestScoredResult(result, &scoresCache));
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

void SearchIndex::populateEdgeGate(Edge* e)
{
	Node* target = e->target;
	for (size_t i = 0; i < target->edges.size(); i++)
	{
		Edge* targetEdge = target->edges[i];
		populateEdgeGate(targetEdge);
		utility::append(e->gate, targetEdge->gate);
	}
	for (size_t i = 0; i < e->s.size(); i++)
	{
		e->gate.insert(tolower(e->s[i]));
	}
}

void SearchIndex::searchRecursive(
	const Path& path, const std::string& remainingQuery, std::vector<SearchIndex::Path>* results) const
{
	if (remainingQuery.size() == 0)
	{
		results->push_back(path);
		return;
	}

	for (const Edge* currentEdge : path.node->edges)
	{
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

			Path currentPath;
			currentPath.node = currentEdge->target;
			currentPath.indices = indices;
			currentPath.text = path.text + edgeString;

			searchRecursive(currentPath, remainingQuery.substr(j), results);
		}
	}
}

std::multiset<SearchResult> SearchIndex::createScoredResults(const std::vector<Path>& paths, size_t maxResultCount) const
{
	// score and order initial paths
	std::multiset<std::pair<int, Path>, bool(*)(const std::pair<int, Path>&, const std::pair<int, Path>&)> scoredPaths(
		[](const std::pair<int, Path>& a, const std::pair<int, Path>& b)
		{
			return a.first > b.first;
		}
	);

	for (const Path& path : paths)
	{
		scoredPaths.insert(std::make_pair(score(path.text, path.indices), path));
	}

	// score paths and subpaths
	std::multiset<SearchResult> searchResults;
	for (const std::pair<int, Path>& p : scoredPaths)
	{
		std::vector<Path> currentPaths;
		currentPaths.push_back(p.second);

		while (currentPaths.size())
		{
			std::vector<Path> nextPaths;

			for (const Path& path : currentPaths)
			{
				if (path.node->elementIds.size())
				{
					SearchResult result;
					result.text = path.text;
					result.elementIds = path.node->elementIds;
					result.indices = path.indices;
					result.score = score(path.text, path.indices);
					searchResults.insert(result);

					if (maxResultCount && searchResults.size() >= maxResultCount)
					{
						return searchResults;
					}
				}

				for (const Edge* edge : path.node->edges)
				{
					Path nextPath;
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

SearchResult SearchIndex::bestScoredResult(SearchResult result, std::map<std::string, SearchResult>* scoresCache) const
{
	if (result.text.size() > 100)
	{
		return result;
	}

	const std::vector<size_t>& indices = result.indices;

	std::map<std::string, SearchResult>::const_iterator it = scoresCache->find(result.text.substr(0, indices.back() + 1));
	if (it != scoresCache->end())
	{
		result.score = it->second.score;
		result.indices = it->second.indices;
	}

	int oldScore = result.score;

	bool consecutive = (indices.size() == 1);
	for (size_t i = 0; i < indices.size() - 1; i++)
	{
		if (indices[i + 1] - indices[i] != 1)
		{
			consecutive = false;
			break;
		}
	}

	if (!consecutive)
	{
		bestScoredResultRecursive(utility::toLowerCase(result.text), indices, indices.size() - 1, scoresCache, &result);
	}

	if (result.score != oldScore || it == scoresCache->end())
	{
		scoresCache->emplace(result.text.substr(0, result.indices.back() + 1), result);
	}

	return result;
}

void SearchIndex::bestScoredResultRecursive(
	const std::string& lowerText, const std::vector<size_t>& indices, size_t indicesPos,
	std::map<std::string, SearchResult>* scoresCache, SearchResult* result) const
{
	size_t oldTextPos = indices[indicesPos];
	size_t nextTextPos = (indicesPos + 1 == indices.size() ? result->text.size() : indices[indicesPos + 1]);
	for (size_t i = oldTextPos + 1; i < nextTextPos; i++)
	{
		if (lowerText[i] == lowerText[oldTextPos])
		{
			std::vector<size_t> newIndices = indices;
			newIndices[indicesPos] = i;

			int newScore = score(result->text, newIndices);
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
			result->score = it->second.score;
			result->indices = it->second.indices;
			return;
		}
	}

	if (indicesPos > 0)
	{
		size_t newIndicesPos = indicesPos - 1;
		while (newIndicesPos > 0 && indices[newIndicesPos + 1] - indices[newIndicesPos] == 1)
		{
			newIndicesPos--;
		}

		if (indices[newIndicesPos + 1] - indices[newIndicesPos] > 1)
		{
			bestScoredResultRecursive(lowerText, indices, newIndicesPos, scoresCache, result);
		}
	}
}

int SearchIndex::score(const std::string& text, const std::vector<size_t>& indices) const
{
	const int unmatchedLetterBonus = -1;
	const int consecutiveLetterBonus = 5;
	const int camelCaseBonus = 4;
	const int noLetterBonus = 3;
	const int delayedStartBonus = -1;
	const int minDelayedStartBonus = -20;

	int unmatchedLetterScore = 0;
	int consecutiveLetterScore = 0;
	int camelCaseScore = 0;
	int noLetterScore = 0;

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
	}

	for (size_t i = 0; i < indices.size(); i++)
	{
		// unmatched and consecutive
		if (i > 0)
		{
			unmatchedLetterScore += (indices[i] - indices[i - 1] - 1) * unmatchedLetterBonus;
			consecutiveLetterScore += (indices[i] - indices[i - 1] == 1 ? consecutiveLetterBonus : 0);
		}

		size_t index = indices[i];

		// camel case
		if (isupper(text[index]))
		{
			bool prevIsLower = (index > 0 && islower(text[index - 1]));
			bool nextIsLower = (index + 1 == text.size() || islower(text[index + 1]));

			if (prevIsLower || nextIsLower)
			{
				camelCaseScore += camelCaseBonus;
			}
		}

		// after no letter
		bool prevIsNoLetter = (index == 0 || noLetters.find(text[index - 1]) != noLetters.end());
		if (prevIsNoLetter)
		{
			noLetterScore += noLetterBonus;
		}
	}

	int leadingStartScore = std::max(int(indices[0]) * delayedStartBonus, minDelayedStartBonus);

	int score =
		unmatchedLetterScore +
		consecutiveLetterScore +
		camelCaseScore +
		noLetterScore +
		leadingStartScore;

	return score;
}
