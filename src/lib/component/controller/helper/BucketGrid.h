#ifndef BUCKET_GRID_H
#define BUCKET_GRID_H

#include <map>

#include "utility/math/Vector2.h"
#include "utility/types.h"

#include "data/graph/Edge.h"

struct DummyEdge;
struct DummyNode;

class Bucket
{
public:
	Bucket();
	Bucket(int i, int j);

	int getWidth() const;
	int getHeight() const;

	bool hasNode(DummyNode* node) const;
	void addNode(DummyNode* node);

	void sort();

	void preLayout(Vec2i viewSize);
	void layout(int x, int y, int width, int height);

	int i;
	int j;

private:
	int m_width;
	int m_height;

	std::vector<DummyNode*> m_nodes;
};


class BucketGrid
{
public:
	BucketGrid(Vec2i viewSize);
	void createBuckets(std::vector<DummyNode>& nodes, const std::vector<DummyEdge>& edges);
	void sortBuckets();
	void layoutBuckets();

private:
	DummyNode* findTopMostDummyNodeRecursive(std::vector<DummyNode>& nodes, Id tokenId, DummyNode* top = nullptr);

	void addNode(DummyNode* node);
	bool addNode(DummyNode* owner, DummyNode* target, bool horizontal);

	Bucket* getBucket(int i, int j);
	Bucket* getBucket(DummyNode* node);

	Vec2i m_viewSize;
	std::map<int, std::map<int, Bucket>> m_buckets;

	int m_i1;
	int m_j1;
	int m_i2;
	int m_j2;
};

#endif // BUCKET_GRID_H
