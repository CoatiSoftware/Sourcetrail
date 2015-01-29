#include "QtGraphPostprocessor.h"

void QtGraphPostprocessor::doPostprocessing(std::list<std::shared_ptr<QtGraphNode>>& nodes)
{
	unsigned int atomarGridSize = 20;

	resizeNodes(nodes, atomarGridSize);

	if(nodes.size() < 2)
	{
		LOG_WARNING_STREAM(<< "Skipping postprocessing, need at least 2 nodes but got " << nodes.size());
		return;
	}

	// determine center of mass (CoD) which is used to get outliers closer to the rest of the graph
	int divisor = 999999;
	int maxNodeSize = 0;
	Vec2i centerOfMass(0, 0);
	float totalMass = 0.0f;
	std::list<std::shared_ptr<QtGraphNode>>::iterator it = nodes.begin();
	for(it; it != nodes.end(); it++)
	{
		if((*it)->getSize().x < divisor)
		{
			divisor = (*it)->getSize().x;
		}

		if((*it)->getSize().y < divisor)
		{
			divisor = (*it)->getSize().y;
		}

		if((*it)->getSize().x > maxNodeSize)
		{
			maxNodeSize = (*it)->getSize().x;
		}
		else if((*it)->getSize().y > maxNodeSize)
		{
			maxNodeSize = (*it)->getSize().y;
		}

		float nodeMass = (*it)->getSize().getLengthSquared();
		centerOfMass += (*it)->getPosition() * nodeMass;
		totalMass += nodeMass;
	}

	centerOfMass /= totalMass;

	divisor = std::min(divisor, (int)atomarGridSize);

	resolveOutliers(nodes, centerOfMass);

	MatrixDynamicBase<unsigned int> heatMap = buildHeatMap(nodes, divisor, maxNodeSize);
	resolveOverlap(nodes, heatMap, divisor);
}

void QtGraphPostprocessor::resolveOutliers(std::list<std::shared_ptr<QtGraphNode>>& nodes, const Vec2i& centerPoint)
{
	float maxDist = 0.0f;
	std::list<std::shared_ptr<QtGraphNode>>::iterator it = nodes.begin();
	for(it; it != nodes.end(); it++)
	{
		Vec2i pos = (*it)->getPosition();
		Vec2i toCenterOfMass = centerPoint - pos;
		if(toCenterOfMass.getLength() > maxDist)
		{
			maxDist = toCenterOfMass.getLength();
		}
	}

	it = nodes.begin();
	for(it; it != nodes.end(); it++)
	{
		Vec2i pos = (*it)->getPosition();
		Vec2i toCenterOfMass = centerPoint - pos;
		float dist = toCenterOfMass.getLength();

		float distFactor = std::sqrt(dist/maxDist); // causes far away nodes to be effected stronger than nodes that are already close to the center

		(*it)->setPosition(pos + toCenterOfMass * distFactor);
	}
}

MatrixDynamicBase<unsigned int> QtGraphPostprocessor::buildHeatMap(const std::list<std::shared_ptr<QtGraphNode>>& nodes, const int atomarNodeSize, const int maxNodeSize)
{
	int heatMapWidth = maxNodeSize * nodes.size() / atomarNodeSize;
	int heatMapHeight = heatMapWidth;

	MatrixDynamicBase<unsigned int> heatMap(heatMapWidth, heatMapHeight);

	std::list<std::shared_ptr<QtGraphNode>>::const_iterator it = nodes.cbegin();
	for(it; it != nodes.end(); it++)
	{
		int left = (*it)->getPosition().x / atomarNodeSize + heatMapWidth/2;
		int up = (*it)->getPosition().y / atomarNodeSize + heatMapHeight/2;
		int width = (*it)->getSize().x / atomarNodeSize;
		int height = (*it)->getSize().y / atomarNodeSize;

		if(left + width > heatMapWidth || left < 0)
			continue;

		if(up + height > heatMapHeight || up < 0)
			continue;

		for(unsigned int i = 0; i < width; i++)
		{
			for(unsigned int j = 0; j < height; j++)
			{
				unsigned int x = left + i;
				unsigned int y = up + j;
				unsigned int value = heatMap.getValue(x, y);
				heatMap.setValue(x, y, value+1);
			}
		}
	}

	return heatMap;
}

void QtGraphPostprocessor::resolveOverlap(std::list<std::shared_ptr<QtGraphNode>>& nodes, MatrixDynamicBase<unsigned int>& heatMap, const int divisor)
{
	int heatMapWidth = heatMap.getColumnsCount();
	int heatMapHeight = heatMap.getRowsCount();

	bool overlap = true;
	int iterationCount = 0;
	int maxIterations = 10;

	while(overlap && iterationCount < maxIterations)
	{
		overlap = false;
		iterationCount++;

		std::list<std::shared_ptr<QtGraphNode>>::iterator it = nodes.begin();
		for(it; it != nodes.end(); it++)
		{
			Vec2i nodePos((*it)->getPosition().x / divisor + heatMapWidth/2,
				(*it)->getPosition().y / divisor + heatMapHeight/2);
			Vec2i nodeSize((*it)->getSize().x / divisor,
				(*it)->getSize().y / divisor);

			if(nodePos.x + nodeSize.x > heatMapWidth || nodePos.x < 0)
				continue;

			if(nodePos.y + nodeSize.y > heatMapHeight || nodePos.y < 0)
				continue;

			Vec2f grad(0.0f, 0.0f);
			if(getHeatmapGradient(grad, heatMap, nodePos, nodeSize))
			{
				overlap = true;
			}

			// handle overlap with no gradient
			// e.g. when a node lies completely on top of another
			float gradLength = grad.getLength();

			if(grad.getLengthSquared() <= 0.000001f)
			{
				int val = heatMap.getValue(nodePos.x, nodePos.y);
				if(val > 1)
				{
					grad = (*it)->getPosition();
					grad.normalize();
					grad *= -1.0f;
				}
			}

			// remove node temporarily from heat map, it will be re-added at the new position later on
			modifyHeatmapArea(heatMap, nodePos, nodeSize, -1);

			// move node to new position
			int xOffset = grad.x * divisor;
			int yOffset = grad.y * divisor;

			// prevent the graph from "exploding" again...
			if(xOffset > divisor)
				xOffset = divisor;
			else if(xOffset < -divisor)
				xOffset = -divisor;

			if(yOffset > divisor)
				yOffset = divisor;
			else if(yOffset < -divisor)
				yOffset = -divisor;

			Vec2i pos = (*it)->getPosition();
			pos += Vec2i(xOffset, yOffset);
			// grid allignment
			pos.x = (pos.x / divisor) * divisor;
			pos.y = (pos.y / divisor) * divisor;
			(*it)->setPosition(pos);

			// re-add node to heat map at new position
			nodePos.x = (*it)->getPosition().x / divisor + heatMapWidth/2;
			nodePos.y = (*it)->getPosition().y / divisor + heatMapHeight/2;
			modifyHeatmapArea(heatMap, nodePos, nodeSize, 1);
		}
	}
}

void QtGraphPostprocessor::modifyHeatmapArea(MatrixDynamicBase<unsigned int>& heatMap, const Vec2i& leftUpperCorner, const Vec2i& size, const int modifier)
{
	bool wentOutOfRange = false;

	for(unsigned int i = 0; i < size.x; i++)
	{
		for(unsigned int j = 0; j < size.y; j++)
		{
			int x = leftUpperCorner.x + i;
			int y = leftUpperCorner.y + j;

			if(x < 0 || x > heatMap.getColumnsCount()-1)
			{
				wentOutOfRange = true;
				continue;
			}
			if(y < 0 || y > heatMap.getRowsCount()-1)
			{
				wentOutOfRange = true;
				continue;
			}

			unsigned int value = heatMap.getValue(x, y);
			heatMap.setValue(x, y, value+modifier);

			if(wentOutOfRange == true)
			{
				LOG_WARNING("Left matrix range while trying to modify values.");
			}
		}
	}
}

bool QtGraphPostprocessor::getHeatmapGradient(Vec2f& outGradient, const MatrixDynamicBase<unsigned int>& heatMap, const Vec2i& leftUpperCorner, const Vec2i& size)
{
	bool overlap = false;

	for(unsigned int i = 0; i < size.x; i++)
	{
		for(unsigned int j = 0; j < size.y; j++)
		{
			int x = leftUpperCorner.x + i;
			int y = leftUpperCorner.y + j;

			// if x and y lie directly at the border not all 4 neighbours can be checked
			if(x < 1 || x > heatMap.getColumnsCount()-2)
				continue;
			if(y < 1 || y > heatMap.getRowsCount()-2)
				continue;

			float val = heatMap.getValue(x, y);
			float xP1 = heatMap.getValue(x+1, y);
			float xM1 = heatMap.getValue(x-1, y);
			float yP1 = heatMap.getValue(x, y+1);
			float yM1 = heatMap.getValue(x, y-1);

			xP1 = std::sqrtf(xP1);
			xM1 = std::sqrtf(xM1);
			yP1 = std::sqrtf(yP1);
			yM1 = std::sqrtf(yM1);

			float xOffset = (xM1 - val) + (val - xP1);
			float yOffset = (yM1 - val) + (val - yP1);

			outGradient += Vec2f(xOffset, yOffset);

			if(val > 1)
			{
				overlap = true;
			}
		}
	}

	return overlap;
}

void QtGraphPostprocessor::resizeNodes(std::list<std::shared_ptr<QtGraphNode>>& nodes, const unsigned int atomarSize)
{
	std::list<std::shared_ptr<QtGraphNode>>::iterator it = nodes.begin();
	for(it; it != nodes.end(); it++)
	{
		Vec2i size = (*it)->getSize();
		if(size.x % atomarSize != 0)
		{
			int multiplier = size.x / atomarSize;
			++multiplier;

			size.x = atomarSize * multiplier;
		}

		if(size.y % atomarSize != 0)
		{
			int multiplier = size.y / atomarSize;
			++multiplier;

			size.y = atomarSize * multiplier;
		}

		(*it)->setSize(size);
	}
}
