#include "data/search/SearchIndex.h"

#include <algorithm>
#include <ctype.h>

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
	std::string lowerCaseQuery = utility::toLowerCase(query);
	Path startPath;
	startPath.node = m_root;
	std::vector<Path> paths;
	search(startPath, lowerCaseQuery, &paths);

	std::set<char> noLetters;
	noLetters.insert(' ');
	noLetters.insert('.');
	noLetters.insert(',');
	noLetters.insert('_');
	noLetters.insert(':');
	noLetters.insert('<');
	noLetters.insert('>');

	// scoring paths
	std::multiset<std::pair<int, Path>, bool(*)(const std::pair<int, Path>&, const std::pair<int, Path>&)> scoredPaths(
		[](const std::pair<int, Path>& a, const std::pair<int, Path>& b)
		{
			return a.first > b.first;
		}
	);

	for (size_t i = 0; i < paths.size(); i++)
	{
		const std::vector<size_t>& currentIndices = paths[i].indices;
		const std::string& currentText = paths[i].text;

		const int unmatchedLetterBonus = -1;
		const int consecutiveLetterBonus = 5;
		const int camelCaseBonus = 5;
		const int noLetterBonus = 3;
		const int delayedStartBonus = -3;
		const int minDelayedStartBonus = -15;

		int unmatchedLetterScore = 0;
		int consecutiveLetterScore = 0;
		int camelCaseScore = 0;
		int noLetterScore = 0;

		for (size_t j = 0; j < currentIndices.size(); j++)
		{
			// unmatched and consecutive
			if (j > 0)
			{
				unmatchedLetterScore += (currentIndices[j] - currentIndices[j-1] - 1) * unmatchedLetterBonus;
				consecutiveLetterScore += (currentIndices[j] - currentIndices[j-1] == 1 ? consecutiveLetterBonus : 0);
			}

			size_t index = currentIndices[j];

			// camel case
			if (isupper(currentText[index]))
			{
				bool prevIsLower = (index > 0 && islower(currentText[index-1]));
				bool nextIsLower = (index + 1 == currentText.size() || islower(currentText[index+1]));

				if (prevIsLower && nextIsLower)
				{
					camelCaseScore += camelCaseBonus;
				}
			}

			// after no letter
			bool prevIsNoLetter = (index > 0 && noLetters.find(currentText[index-1]) != noLetters.end());
			if (prevIsNoLetter)
			{
				noLetterScore += noLetterBonus;
			}
		}

		int leadingStartScore = std::max(int(currentIndices[0]) * delayedStartBonus, minDelayedStartBonus);

		int score =
			unmatchedLetterScore +
			consecutiveLetterScore +
			camelCaseScore +
			noLetterScore +
			leadingStartScore;

		scoredPaths.insert(std::make_pair(score, paths[i]));
	}

	// preparing results
	std::vector<SearchResult> searchResults;
	for (const std::pair<int, Path> currentResult : scoredPaths)
	{
		if (maxResultCount > 0 && searchResults.size() >= maxResultCount)
		{
			break;
		}

		int currentScore = currentResult.first;
		std::vector<Path> currentPaths;
		currentPaths.push_back(currentResult.second);

		while (currentPaths.size() > 0)
		{
			std::vector<Path> nextPaths;

			for (size_t j = 0; j < currentPaths.size(); j++)
			{
				Path& currentPath = currentPaths[j];
				if (currentPath.node->elementIds.size() > 0 && (maxResultCount == 0 || searchResults.size() < maxResultCount))
				{
					SearchResult result;
					result.elementIds = currentPath.node->elementIds;
					result.indices = currentPath.indices;
					result.text = currentPath.text;
					result.score = currentScore;
					searchResults.push_back(result);
				}

				for (size_t k = 0; k < currentPath.node->edges.size(); k++)
				{
					Path nextPath;
					nextPath.indices = currentPath.indices;
					nextPath.node = currentPath.node->edges[k]->target;
					nextPath.text = currentPath.text + currentPath.node->edges[k]->s;
					nextPaths.push_back(nextPath);
				}
			}

			currentPaths = nextPaths;

			if (!(maxResultCount == 0 || searchResults.size() < maxResultCount))
			{
				break;
			}
		}
	}

	return searchResults;
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

void SearchIndex::search(const Path& path, const std::string& remainingQuery, std::vector<SearchIndex::Path>* results) const
{
	if (remainingQuery.size() == 0)
	{
		results->push_back(path);
	}
	else
	{
		for (size_t i = 0; i < path.node->edges.size(); i++)
		{
			const Edge* currentEdge = path.node->edges[i];

			// test if s passes the edge's gate.
			bool passesGate = true;
			for (size_t j = 0; j < remainingQuery.size(); j++)
			{
				if (currentEdge->gate.find(tolower(remainingQuery[j])) == currentEdge->gate.end())
				{
					passesGate = false;
					break;
				}
			}

			if (passesGate)
			{
				// consume characters for edge
				const std::string& edgeString = currentEdge->s;

				std::vector<size_t> currentFoundIds = path.indices;
				std::string currentRemainingQuery = remainingQuery;

				for (size_t j = 0; j < edgeString.size() && currentRemainingQuery.size() > 0; j++)
				{
					if (currentRemainingQuery[0] == tolower(edgeString[j]))
					{
						currentFoundIds.push_back(path.text.size() + j);
						currentRemainingQuery = currentRemainingQuery.substr(1);
					}
				}

				Path currentPath;
				currentPath.node = currentEdge->target;
				currentPath.indices = currentFoundIds;
				currentPath.text = path.text + edgeString;

				search(currentPath, currentRemainingQuery, results);
			}
		}
	}
}
