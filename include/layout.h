#ifndef LAYOUT_H
#define LAYOUT_H

#include <vector>
#include <map>

#include "io.h"
#include "graph.h"
#include "mmcf.h"

const int dx[5] = {0, 1, 0, -1, 0};
const int dy[5] = {1, 0, -1, 0, 0};

struct TileNode : public Node {/*{{{*/
	int x, y;
	InterNode* v[4];

	TileNode(int paraX, int paraY);
	bool hasVnode(InterNode* vnode);
};/*}}}*/

struct GridEdge : public Edge {/*{{{*/
	InterNode *vhead, *vtail, *source, *sink;
	
	GridEdge(TileNode* head, TileNode* tail, InterNode* paraSource, InterNode* paraSink);
};/*}}}*/

struct PinNode {/*{{{*/
	int x, y;
	InterNode* node;
	std::vector<std::pair<InterNode*, bool>> vnodes;

	PinNode(int paraX, int paraY);
	void addNewVnode(InterNode* node, bool isEdge = true);
};/*}}}*/

struct SlotNode {/*{{{*/

	int x, y, k;
	InterNode *node;

	SlotNode(int paraX, int paraY, int paraK) ;
	int getAdjX();
	int getAdjY();
};/*}}}*/

class Layout : public BasicInfo {/*{{{*/
	void pushBorderItem(int x, int y, int k);

	public:

	std::vector< std::vector<TileNode> > tiles;
	std::vector<SlotNode> border;
	std::vector<PinNode> keys; 
	std::vector<GridEdge*> edges;

	int table_m, table_n;

	bool inTable(int x, int y);
	bool isExit(int x, int y);
	bool isBorder(int x, int y);
	bool isCorner(int x, int y);

	Layout(GridInfo* pData);
	~Layout() = default;
};/*}}}*/

#endif
