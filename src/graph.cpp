#include "graph.h"

Node::iterator Node::begin() {
	return adjList.begin();
}

Node::iterator Node::end() {
	return adjList.end();
}

void Node::addEdge(Edge* edge) {
	adjList.push_back(edge);
}

Node::~Node() {
	for (Edge* edge: adjList)
		delete edge;
}

Edge::Edge(Node* paraHead, Node* paraTail) : head { paraHead }, tail { paraTail } {
	head -> addEdge(this);	
#ifdef DEBUG
	assert(head != tail);
#endif
};

Node* Edge::getHead() const {
	return head;
}

Node* Edge::getTail() const {
	return tail;
}
