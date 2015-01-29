#include "component/controller/GraphLayouter.h"

#include <iostream>

#include <cmath>
#include <map>
#include <queue>

#include "component/view/graphElements/GraphEdge.h"
#include "component/view/graphElements/GraphNode.h"

#include "utility/math/MatrixDynamicBase.h"

// for prototyping, remove when done
#include "Eigen/Dense"
#include "Eigen/Eigenvalues"
#include "unsupported/Eigen/MatrixFunctions"

bool compareEigenvaluePairs(const std::pair<int, double>& p0, const std::pair<int, double>& p1)
{
	return p0.second > p1.second;
}

void GraphLayouter::layoutSimpleRaster(std::vector<DummyNode>& nodes)
{
	int x = 0;
	int y = 0;
	int offset = 150;

	int w = ceil(sqrt(nodes.size()));

	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		if (i > 0 && i % w == 0)
		{
			y += offset;
			x = 0;
		}

		nodes[i].position = Vec2i(x, y);

		x += offset;
	}
}

void GraphLayouter::layoutSimpleRing(std::vector<DummyNode>& nodes)
{
	if (nodes.size() >= 1)
	{
		nodes[0].position = Vec2i(0, 0);

		if (nodes.size() > 1)
		{
			float offset = 200.0f;

			for (unsigned int i = 1; i < nodes.size(); i++)
			{
				float rad = 2.0f * 3.14159265359f / float(nodes.size() - 1) * i - 1;

				int x = offset * std::cos(rad);
				int y = offset * std::sin(rad);

				nodes[i].position = Vec2i(x, y);
			}
		}
	}
}

void GraphLayouter::layoutSpectralPrototype(std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges)
{
	if(nodes.size() < 2)
	{
		LOG_WARNING("Not enough nodes for layouting");
		return;
	}

	MatrixDynamicBase<int> laplacian = buildLaplacianMatrix(nodes, edges);

	Eigen::MatrixXd degreeMatrix(laplacian.getColumnsCount(), laplacian.getRowsCount());
	Eigen::MatrixXd eigenMatrix(laplacian.getColumnsCount(), laplacian.getRowsCount());

	for(unsigned int x = 0; x < laplacian.getColumnsCount(); x++)
	{
		for(unsigned int y = 0; y < laplacian.getRowsCount(); y++)
		{
			eigenMatrix(x, y) = laplacian.getValue(x, y);

			if(x == y)
			{
				degreeMatrix(x, y) = laplacian.getValue(x, y);
			}
		}
	}

	degreeMatrix = degreeMatrix.inverse();
	Eigen::MatrixPower<Eigen::MatrixXd> dPow(degreeMatrix);
	degreeMatrix = dPow(0.5);

	eigenMatrix = degreeMatrix * eigenMatrix * degreeMatrix;

	eigenMatrix.normalize();
	Eigen::EigenSolver<Eigen::MatrixXd> solver(eigenMatrix);

	std::vector<std::vector<double>> eigenVectors;
	for(unsigned int i = 0; i < solver.eigenvectors().cols(); i++)
	{
		eigenVectors.push_back(std::vector<double>());

		for(unsigned int j = 0; j < solver.eigenvectors().rows(); j++)
		{
			eigenVectors[i].push_back(solver.eigenvectors()(i*solver.eigenvectors().rows() + j).real());
		}
	}

	std::vector<std::pair<int, double>> eigenValues;
	for(unsigned int i = 0; i < solver.eigenvalues().size(); i++)
	{
		eigenValues.push_back(std::pair<int, double>(i, solver.eigenvalues()(i).real()));
	}

	std::sort(eigenValues.begin(), eigenValues.end(), compareEigenvaluePairs);

	if(eigenVectors.size() > 0 && eigenVectors[0].size() >= 3)
	{
		unsigned int xIdx = eigenValues[eigenValues.size()-2].first;
		unsigned int yIdx = eigenValues[eigenValues.size()-3].first;

		/*double xEigenValue = std::sqrt(solver.eigenvalues()(xIdx).real());
		double yEigenValue = std::sqrt(solver.eigenvalues()(yIdx).real());*/

		for(unsigned int i = 0; i < nodes.size(); i++)
		{
			float xPos = eigenVectors[xIdx][i];
			float yPos = eigenVectors[yIdx][i];

			Vec2f newPos(xPos, yPos);

			newPos.normalize();
			newPos *= 600.0f;

			nodes[i].position.x = newPos.x;
			nodes[i].position.y = newPos.y;

			//std::cout << newPos << std::endl;
		}
		//std::cout << "=================" << std::endl;
	}
}

MatrixDynamicBase<int> GraphLayouter::buildLaplacianMatrix(const std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges)
{
	MatrixDynamicBase<int> matrix(nodes.size(), nodes.size());

	std::map<Id, DummyNode> nodesMap;
	std::queue<DummyNode> remainingNodes;
	for(unsigned int i = 0; i < nodes.size(); i++)
	{
		remainingNodes.push(nodes[i]);
	}

	while(remainingNodes.size() > 0)
	{
		if(remainingNodes.front().subNodes.size() > 0)
		{
			for(unsigned int i = 0; i < remainingNodes.front().subNodes.size(); i++)
			{
				remainingNodes.push(remainingNodes.front().subNodes[i]);
			}
		}

		nodesMap[remainingNodes.front().tokenId] = remainingNodes.front();
		remainingNodes.pop();
	}

	std::map<std::pair<Id, Id>, int> weightsMap;
	for(unsigned int i = 0; i < edges.size(); i++)
	{
		DummyNode ownerNode = nodesMap[edges[i].ownerId];
		DummyNode targetNode = nodesMap[edges[i].targetId];

		if(ownerNode.topLevelAncestorId != targetNode.topLevelAncestorId)
		{
			int weightIncrement = 1;

			Id ownerId = ownerNode.topLevelAncestorId;
			Id targetId = targetNode.topLevelAncestorId;
			std::pair<Id, Id> key(ownerId, targetId);
			std::pair<Id, Id> inverseKey(targetId, ownerId);
			std::pair<Id, Id> keyOwner(ownerId, ownerId);
			std::pair<Id, Id> keyTarget(targetId, targetId);

			std::map<std::pair<Id, Id>, int>::iterator it = weightsMap.find(key);
			if(it == weightsMap.end())
			{
				weightsMap[key] = 0;
			}

			weightsMap[key] += weightIncrement;

			it = weightsMap.find(inverseKey);
			if(it == weightsMap.end())
			{
				weightsMap[inverseKey] = 0;
			}

			weightsMap[inverseKey] += weightIncrement;

			it = weightsMap.find(keyOwner);
			if(it == weightsMap.end())
			{
				weightsMap[keyOwner] = 0;
			}

			weightsMap[keyOwner] += weightIncrement;

			it = weightsMap.find(keyTarget);
			if(it == weightsMap.end())
			{
				weightsMap[keyTarget] = 0;
			}

			weightsMap[keyTarget] += weightIncrement;
		}
	}

	for(unsigned int x = 0; x < nodes.size(); x++)
	{
		for(unsigned int y = x; y < nodes.size(); y++)
		{
			unsigned int xNodeId = nodes[x].tokenId;
			unsigned int yNodeId = nodes[y].tokenId;

			std::pair<Id, Id> key(xNodeId, yNodeId);

			if(x == y)
			{
				matrix.setValue(x, y, weightsMap[key]);
			}
			else
			{
				matrix.setValue(x, y, -weightsMap[key]);
				matrix.setValue(y, x, -weightsMap[key]);
			}
		}
	}

	return matrix;
}
