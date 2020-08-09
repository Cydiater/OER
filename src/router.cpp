#include "router.h"

void Router::handle_border() {/*{{{*/
#ifdef DEBUG
	fprintf(stderr, "[INFO - build router] handle border begin\n");
#endif
	std::vector<InterNode*> lastout;	
	for (auto it = layout -> border.begin(); it != layout -> border.end(); it++) {
		SlotNode cur = *it;	
		if (cur.k != 5) {
			addEdge(layout -> tiles[cur.getAdjX()][cur.getAdjY()].v[cur.k], 
					cur.node, 1, 1);
		} 
		lastout.push_back(cur.node);
	}	
#ifdef DEBUG
	fprintf(stderr, "[INFO - build router] first level of border build done\n");
#endif
	int insize = static_cast<int>(lastout.size()), 
		outsize = static_cast<int>(layout -> keys.size());
	for (int i = insize - 1; i >= outsize; i--) {
		for (int j = 0; j < i; j++) {
			InterNode *cur { new InterNode() };
			addEdge(lastout[j], cur, 1, 0);
			addEdge(lastout[j + 1], cur, 1, 0);
			lastout[j] = cur;
		}
	}
#ifdef DEBUG
	assert(insize >= outsize);
	fprintf(stderr, "[INFO - build router] fan structure build done with insize = %d & outsize = %d.\n", insize, outsize);
#endif

	for (int i = 0; i < outsize; i++)
		mmcf -> add_commodity(layout -> keys[i].node, lastout[i], 1);

#ifdef DEBUG
	fprintf(stderr, "[INFO - build router] handle border done\n");
#endif
}/*}}}*/

void Router::handle_table() {/*{{{*/

	for (auto it = layout -> edges.begin(); it != layout -> edges.end(); it++) {
		GridEdge* edge = *it;
		handle_undirected_edge(edge -> source, 
								 edge -> sink, 
								edge -> vhead, 
								edge -> vtail, 1, 1);
	}			

	for (int x = 0; x < layout -> m - 1; x++)
		for (int y = 0; y < layout -> n - 1; y++) {
			for (int k = 0; k < 4; k++) {
				handle_undirected_edge(layout -> tiles[x][y].v[k], layout -> tiles[x][y].v[(k + 1) % 4], 
									   new InterNode(), new InterNode(), 1, 0);
			}
		}
#ifdef DEBUG
	fprintf(stderr, "[INFO - build router] handler table done\n");
#endif
}/*}}}*/

void Router::handle_pin() {/*{{{*/
	for (auto it = layout -> keys.begin(); it != layout -> keys.end(); it++) {
		auto node = *it;
		for (auto vnode: node.vnodes)
			addEdge(node.node, vnode.first, 1, vnode.second ? 0 : 1);
	}
#ifdef DEBUG
	fprintf(stderr, "[INFO - build router] handler pin done\n");
#endif
}/*}}}*/

void Router::handle_undirected_edge(InterNode* source, InterNode *sink, InterNode *vhead, InterNode *vtail, int cap, int cost) {/*{{{*/
	addEdge(vhead, vtail, cap, cost);
	addEdge(source, vhead, cap, 0);
	addEdge(sink, vhead, cap, 0);
	addEdge(vtail, source, cap, 0);
	addEdge(vtail, sink, cap, 0);
}/*}}}*/

Router::Router(GridInfo* pData) {/*{{{*/
	layout = new Layout(pData);
	mmcf = new MMCF();

#ifdef DEBUG
	fprintf(stderr, "[INFO - build router] mmcf & layout initialize done\n");
#endif

	handle_border();
	handle_table();
	handle_pin();
}/*}}}*/

Router::~Router() {/*{{{*/
	delete mmcf;
	delete layout;
#ifdef DEBUG
	printf("[INFO - destruct Router] done\n");
#endif
}/*}}}*/

RouteResult* Router::createResult(int length) {/*{{{*/
#ifdef DEBUG
	fprintf(stderr, "[INFO - create result] traverse net flow begin\n");
#endif
	int index = 0;
	RouteResult *result = new RouteResult(*static_cast<BasicInfo*>(layout), length);
	for (auto it = layout -> keys.begin(); it != layout -> keys.end(); it++) {
		InterNode* cur = (*it).node;
		result -> startNewRoute(std::make_pair((*it).x, (*it).y));
#ifdef DEBUG
		fprintf(stderr, "[INFO - create result] start from pin (%d, %d)\n", (*it).x, (*it).y);
#endif
		auto xy = mmcf -> walk_along_flow(cur, index);
		while (xy != std::make_pair(-1, -1)) {
#ifdef DEBUG
			fprintf(stderr, "[INFO - create result] walk to (%d, %d)\n", xy.first, xy.second);
#endif
			result -> expandLastRoute(xy);
			if (layout -> isExit(xy.first, xy.second)) break;
			xy = mmcf -> walk_along_flow(cur, index);
		}
		index++;
	}	
	return result;
}/*}}}*/

RouteResult* Router::run() {/*{{{*/
#ifdef DEBUG
	printf("[INFO - MMCF] run begin\n");
#endif
	int ret = mmcf -> run();
	if (ret == -1) 
		return nullptr;

	return createResult(ret);
}/*}}}*/
