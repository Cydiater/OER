#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

class Node;
class Edge;

class Node {
	std::vector<Edge*> adjList;

	public:

	Node() = default;
	virtual ~Node();

	typedef std::vector<Edge*>::iterator iterator;

	iterator begin();
	iterator end();
	void addEdge(Edge* edge);
};

class Edge {
	Node *head, *tail;

	public:

	Edge() = delete;
	Edge(Node* paraHead, Node* paraTail);
	virtual ~Edge() = default;

	Node* getHead() const;
	Node* getTail() const;
};

#endif
