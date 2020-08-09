#include "layout.h"


TileNode::TileNode(int paraX, int paraY) : Node(), x { paraX }, y { paraY } {
	for (int i = 0; i < 4; i++) {
		v[i] = new InterNode(x, y);
	}
}

bool TileNode::hasVnode(InterNode* vnode) {
	for (int i = 0; i < 4; i++) if (vnode == v[i])
		return true;
	return false;
}

GridEdge::GridEdge(TileNode* head, TileNode* tail, InterNode* paraSource, InterNode* paraSink) :
	Edge(head, tail),
	vhead { new InterNode() },
	vtail { new InterNode() },
	source { paraSource },
	sink { paraSink }
{ };

PinNode::PinNode(int paraX, int paraY) : 
	x { paraX }, y { paraY }, 
	node { new InterNode(x, y) } { }

void PinNode::addNewVnode(InterNode* node, bool isEdge) {
	vnodes.push_back(std::make_pair(node, isEdge));
} 

SlotNode::SlotNode(int paraX, int paraY, int paraK) 
	: x { paraX }, y {paraY}, k { paraK } {
		node = new InterNode(x, y);
	}

int SlotNode::getAdjX() { return ((x - dx[k] * 2) - 2) / 2; }
int SlotNode::getAdjY() { return ((y - dy[k] * 2) - 2) / 2; }

void Layout::pushBorderItem(int x, int y, int k) {
	border.emplace_back(x, y, k);
#ifdef DEBUG
	fprintf(stderr, "[INFO - build layout] border (%d, %d) detected\n", x, y);
#endif
}

bool Layout::inTable(int x, int y) {
	return 0 <= x && x < m - 1 &&
		0 <= y && y < n - 1;
}

bool Layout::isExit(int x, int y) {
	if ( std::min( 
				std::min( x , table_m - x - 1 ),
				std::min( y , table_n - y - 1 )
				) == 0) return true;
	return false;
}

bool Layout::isBorder(int x, int y) {
	if ( std::min( 
				std::min( x , table_m - x - 1 ),
				std::min( y , table_n - y - 1 )
				) == 1) return true;
	return false;
}

bool Layout::isCorner(int x, int y) {
	if (x == 1 && y == 1)			return true;
	if (x == 1 && y == table_n - 2)	return true;
	if (x == table_m - 2 && y == 1) return true;
	if (x == table_m - 2 &&
			y == table_n - 2)			return true;	
	return false;
}

Layout::Layout(GridInfo* pData) : BasicInfo( *static_cast<BasicInfo*>(pData) ),
								  table_m { pData -> m * 2 + 1 },
								  table_n { pData -> n * 2 + 1 } {

	tiles.reserve(m - 1);
	for (int i = 0; i < m - 1; i++)
		tiles[i].reserve(n);
	for (int x = 0; x < m - 1; x++) {
		tiles.emplace_back();
		for (int y = 0; y < n - 1; y++)
			tiles[x].emplace_back(tx2tax(x), ty2tay(y));
	}
	
#ifdef DEBUG
	printf("[INFO - build layout] tiles section build done\n");
#endif

	int border_size = pData -> border_size();
	std::map<std::pair<int, int>, PinNode*> PinMap;
	keys.reserve(pData -> size());
	for (auto it = pData -> begin(); it != pData -> end(); it++) {
		auto xy = pxy2taxy(*it);
		keys.emplace_back(xy.first, xy.second);
		PinMap[xy] = &keys.back();
		if (isBorder(xy.first, xy.second)) border_size++;
		if (isCorner(xy.first, xy.second)) border_size++;
	}	

#ifdef DEBUG
	printf("[INFO - build layout] pin section build done\n");
#endif

	border.reserve(border_size);

	if (pData -> hasExit(0)) {/*{{{*/
		auto xy = std::make_pair(1, 1);
		if (PinMap.count(xy)) {
			pushBorderItem(xy.first - 1, xy.second, 5);
			PinMap[xy] -> addNewVnode(border.back().node, false);
		}
		for (int x = 0, y = 0; y < n - 1; y++) {
			pushBorderItem(tx2tax(x) - 2, ty2tay(y), 3);
			xy = std::make_pair(tx2tax(x) - 1, ty2tay(y) + 1);
			if (PinMap.count(xy)) {
				pushBorderItem(xy.first - 1, xy.second, 5);
				PinMap[xy] -> addNewVnode(border.back().node, false);
			}
		}
	}/*}}}*/

	if (pData -> hasExit(1)) {/*{{{*/
		auto xy = std::make_pair(1, table_n - 2);
		if (PinMap.count(xy)) {
			pushBorderItem(xy.first, xy.second + 1, 5);
			PinMap[xy] -> addNewVnode(border.back().node, false);
		}
		for (int x = 0, y = n - 2; x < m - 1; x++) {
			pushBorderItem(tx2tax(x), ty2tay(y) + 2, 0);
			xy = std::make_pair(tx2tax(x) + 1, ty2tay(y) + 1);
			if (PinMap.count(xy)) {
				pushBorderItem(xy.first, xy.second + 1, 5);
				PinMap[xy] -> addNewVnode(border.back().node, false);
			}
		}
	}/*}}}*/

	if (pData -> hasExit(2)) {/*{{{*/
		auto xy = std::make_pair(table_m - 2, table_n - 2);
		if (PinMap.count(xy)) {
			pushBorderItem(xy.first + 1, xy.second, 5);
			PinMap[xy] -> addNewVnode(border.back().node, false);
		}
		for (int x = m - 2, y = n - 2; y >= 0; y--) {
			pushBorderItem(tx2tax(x) + 2, ty2tay(y), 1);
			xy = std::make_pair(tx2tax(x) + 1, ty2tay(y) - 1);
			if (PinMap.count(xy)) {
				pushBorderItem(xy.first + 1, xy.second, 5);
				PinMap[xy] -> addNewVnode(border.back().node, false);
			}
		}
	}/*}}}*/

	if (pData -> hasExit(3)) {/*{{{*/
		auto xy = std::make_pair(table_m - 2, 1);
		if (PinMap.count(xy)) {
			pushBorderItem(xy.first, xy.second - 1, 5);
			PinMap[xy] -> addNewVnode(border.back().node, false);
		}
		for (int x = m - 2, y = 0; x >= 0; x--) {
			pushBorderItem(tx2tax(x), ty2tay(y) - 2, 2);
			xy = std::make_pair(tx2tax(x) - 1, ty2tay(y) - 1);
			if (PinMap.count(xy)) {
				pushBorderItem(xy.first, xy.second - 1, 5);
				PinMap[xy] -> addNewVnode(border.back().node, false);
			}
		}
	}/*}}}*/

#ifdef DEBUG
	printf("[INFO - build layout] border section build done\n");
#endif

	for (int x = 0; x < m - 1; x++)
		for (int y = 0; y < n - 1; y++)
			for (int k = 0; k < 2; k++) {
				int tx = x + dx[k], ty = y + dy[k];
				if (inTable(tx, ty)) {
					edges.push_back(new GridEdge(&tiles[x][y], &tiles[tx][ty],
												 tiles[x][y].v[k], 
												 tiles[tx][ty].v[(k + 2) % 4]));
#ifdef DEBUG
					printf("[INFO - build layout] find edge (%d, %d) - (%d, %d)\n", tx2tax(x), ty2tay(y), tx2tax(tx), ty2tay(ty));
#endif
					if (x == tx) {

						auto xy = std::make_pair(tx2tax(x) + 1, ty2tay(y) + 1);/*{{{*/
						if (PinMap.count(xy)) {
							PinMap[xy] -> addNewVnode(edges.back() -> vhead);
#ifdef DEBUG
							printf("[INFO - build layout] pin (%d, %d) near edge\n", xy.first, xy.second);
#endif
						}

						xy.first -= 2;
						if (PinMap.count(xy)) {
							PinMap[xy] -> addNewVnode(edges.back() -> vhead);
#ifdef DEBUG
							printf("[INFO - build layout] pin (%d, %d) near edge\n", xy.first, xy.second);
#endif
						}/*}}}*/

					} else if (y == ty) {
						
						auto xy = std::make_pair(tx2tax(x) + 1, ty2tay(y) + 1);/*{{{*/
						if (PinMap.count(xy)) {
							PinMap[xy] -> addNewVnode(edges.back() -> vhead);
#ifdef DEBUG
							printf("[INFO - build layout] pin (%d, %d) near edge\n", xy.first, xy.second);
#endif
						}

						xy.second -= 2;
						if (PinMap.count(xy)) {
							PinMap[xy] -> addNewVnode(edges.back() -> vhead);
#ifdef DEBUG
							printf("[INFO - build layout] pin (%d, %d) near edge\n", xy.first, xy.second);
#endif
						}/*}}}*/

					}
				}
			}
#ifdef DEBUG
	printf("[INFO - build layout] link pin around edge done\n");
#endif
}	

