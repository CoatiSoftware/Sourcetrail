#include "data/search/SearchIndex.h"

#include <algorithm>
#include <ctype.h>

#include "utility/utility.h"

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
	std::string lowerCaseQuery = "";
	for (size_t i = 0; i < query.size(); i++)
	{
		lowerCaseQuery += tolower(query[i]);
	}

	Path startPath;
	startPath.node = m_root;
	std::vector<Path> paths = search(startPath, lowerCaseQuery);

	// scoring paths
	std::vector<std::pair<int, Path>> scoredPaths;
	for (size_t i = 0; i < paths.size(); i++)
	{
		const std::vector<size_t>& currentIndices = paths[i].indices;

		const int unmatchedLetterBonus = -1;
		const int consecutiveLetterBonus = 5;
		const int camelCaseBonus = 10;
		const int delayedStartBonus = -3;
		const int minDelayedStartBonus = -9;

		int unmatchedLetterScore = 0;
		int consecutiveLetterScore = 0;
		for (size_t j = 1; j < currentIndices.size(); j++)
		{
			unmatchedLetterScore += (currentIndices[j] - currentIndices[j-1] - 1) * unmatchedLetterBonus;
			consecutiveLetterScore += (currentIndices[j] - currentIndices[j-1] == 1 ? consecutiveLetterBonus : 0);
		}

		int camelCaseScore = 0;
		for (size_t j = 0; j < currentIndices.size(); j++)
		{
			size_t index = currentIndices[j];
			if (index == 0 || islower(paths[i].text[index-1]))
			{
				camelCaseScore += (isupper(paths[i].text[index]) ? camelCaseBonus : 0);
			}
		}

		int leadingStartScore = 0;
		leadingStartScore += std::max(int(currentIndices[0]) * delayedStartBonus, minDelayedStartBonus);

		int score =
			unmatchedLetterScore +
			consecutiveLetterScore +
			camelCaseScore +
			leadingStartScore;

		scoredPaths.push_back(std::make_pair(score, paths[i]));
	}

	// sorting paths
	std::sort(scoredPaths.begin(), scoredPaths.end(), [](
		std::pair<int, Path> a,
		std::pair<int, Path> b)
		{
			return b.first < a.first;
		}
	);

	// preparing results
	std::vector<SearchResult> searchResults;
	for (size_t i = 0; i < scoredPaths.size() && (maxResultCount == 0 || searchResults.size() < maxResultCount); i++)
	{
		std::vector<Path> currentPaths;
		currentPaths.push_back(scoredPaths[i].second);

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

std::vector<SearchIndex::Path> SearchIndex::search(const Path& path, const std::string& remainingQuery) const
{
	std::vector<Path> results;

	if (remainingQuery.size() == 0)
	{
		results.push_back(path);
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

				utility::append(results, search(currentPath, currentRemainingQuery));
			}
		}
	}
	return results;
}
