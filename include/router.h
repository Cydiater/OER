#ifndef ROUTER_H
#define ROUTER_H

#include "mmcf.h"
#include "layout.h"

class Router {

	RouteResult* createResult(int length);

	void addEdge(InterNode* head, InterNode* tail, int cap, int cost) {
		mmcf -> addEdge(head, tail, cap, cost);
	}

	void handle_undirected_edge(InterNode* source, InterNode *sink, InterNode *vhead, InterNode *vtail, int cap, int cost);

	void handle_border();
	void handle_pin();
	void handle_table();

	public:
	
	Layout* layout;
	MMCF* mmcf;

	Router(GridInfo* pData);
	~Router();

	RouteResult* run();	
};

#endif
