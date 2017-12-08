#ifndef TREE_H
#define TREE_H

template <typename T>
struct Tree
{
	Tree() {}
	Tree(T data) : data(data) {}
	T data;
	std::vector<Tree<T>> children;
};

#endif // TREE_H
