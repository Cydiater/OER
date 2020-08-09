#include <queue>

#include "gen.h"

CapEdge::CapEdge(DisNode* _head, DisNode* _tail, int _cap, bool _walkable) : 
						Edge(_head, _tail), 
					    cap { _cap }, 
						rev { nullptr },
						walkable { _walkable } { }

DisNode::DisNode(int _x, int _y) : x { _x }, y { _y }, dis { -1 } { }

FlowGraph::FlowGraph() : source { nullptr }, sink { nullptr } { }

void FlowGraph::addUndirectedEdge(DisNode *head, DisNode *tail, int cap) {
	CapEdge *forward = new CapEdge(head, tail, cap, true);
	CapEdge *backward = new CapEdge(tail, head, cap, true);
	forward -> rev = backward;
	backward -> rev = forward;
}

void FlowGraph::addDirectedEdge(DisNode *head, DisNode *tail, int cap) {
	CapEdge *forward = new CapEdge(head, tail, cap, true);
	CapEdge *backward = new CapEdge(tail, head, 0, false);
	forward -> rev = backward;
	backward -> rev = forward;
}

void FlowGraph::addNode(DisNode &node) {
	nodes.push_back(&node);
}

void FlowGraph::setSource(DisNode &node) {
	source = &node;
}

void FlowGraph::setSink(DisNode &node) {
	sink = &node;
}

bool FlowGraph::bfs() {
	std::queue<DisNode*> q;
	for (auto node: nodes)	
		node -> dis = std::numeric_limits<int>::max();
	source -> dis = 0;
	q.push(source);

	while (!q.empty()) {
		DisNode *node = q.front();
		q.pop();
		for (auto it = node -> begin(); it != node -> end(); it++) {
			CapEdge *edge = static_cast<CapEdge*>(*it);
			if (edge -> cap > 0) {
				DisNode *next = static_cast<DisNode*>(edge -> getTail());
				if (node -> dis + 1 < next -> dis) {
					next -> dis = node -> dis + 1;
					q.push(next);
				}
			}
		}
	}

	return sink -> dis != std::numeric_limits<int>::max();
}

int FlowGraph::dfs(DisNode *node, int flow) {
	if (node == sink || flow == 0) return flow;

	int sumFlow = 0;

	for (auto it = node -> begin(); it != node -> end(); it++) {
		CapEdge *edge = static_cast<CapEdge*>(*it);
		DisNode *next = static_cast<DisNode*>(edge -> getTail());
		int nxtFlow = 0;

		if (edge -> cap > 0 && 
			node -> dis + 1 == next -> dis &&
		   (nxtFlow = dfs(next, std::min(flow - sumFlow, edge -> cap)) > 0) ) {
				edge -> cap -= flow;	
				edge -> rev -> cap += flow;

				sumFlow += nxtFlow;
			
		}
	}

	if (sumFlow == 0)
		node -> dis = -1;

	return sumFlow;
}

void FlowGraph::clear() {
	nodes.clear();
	source = sink = nullptr;
}

int FlowGraph::run() {
#ifdef DEBUG
	fprintf(stderr, "[INFO] FlowGraph::run() begin\n");
#endif 
	int sum = 0;
	while (bfs()) {
		int flow = 0;
#ifdef DEBUG
		fprintf(stderr, "[INFO] bfs pass\n");
#endif 
		while ( (flow = dfs(source, std::numeric_limits<int>::max())) > 0 ) {
			sum += flow;
		}
	}
#ifdef DEBUG
	fprintf(stderr, "[INFO] sum of flow = %d\n", sum);
#endif 
	return sum;	
}

OrderFinder::OrderFinder() : pInfo { nullptr }, 
							 pGraph { new FlowGraph() },
							 source(-1, -1),
							 sink(-2, -2) { }
OrderFinder::OrderFinder(GridInfo &grid) : pInfo { &grid }, 
										   pGraph { new FlowGraph() },
										   source(-1, -1),
										   sink(-2, -2) { }

OrderFinder::~OrderFinder() {
	delete pGraph;
}

void OrderFinder::load(GridInfo &grid) {
	pInfo = &grid;
}

inline bool OrderFinder::inTable(int x, int y) {
	return 0 <= x && x < table_m &&
		   0 <= y && y < table_n;
}

const int vertical_dx[] = {0, 1, 0, -1};
const int vertical_dy[] = {1, 0, -1, 0};

const int incline_dx[] = {1, 1, -1, -1};
const int incline_dy[] = {1, -1, 1, -1};

void OrderFinder::buildTable() {
	table_m = pInfo -> m * 2 + 1;	
	table_n = pInfo -> n * 2 + 1;

	pGraph -> clear();

	for (int i = 0; i < 2; i++) {
		table[i].clear();
		table[i].resize( table_m );
		for (int x = 0; x < table_m; x++) {
			table[i][x].clear();
			table[i][x].reserve(table_n);
			for (int y = 0; y < table_n; y++) {
				table[i][x].emplace_back(x, y);
				pGraph -> addNode(table[i][x][y]);
			}
		}
	}

	for (int x = 0; x < table_m; x++)
		for (int y = 0; y < table_n; y++)
			pGraph -> addDirectedEdge(&table[0][x][y], &table[1][x][y], 1);
	
	pGraph -> addNode( source = DisNode(-1, -1) );
	pGraph -> addNode( sink = DisNode(-2, -2) );

	pGraph -> setSource( source );
	pGraph -> setSink( sink );

	for (auto it = pInfo -> begin(); it != pInfo -> end(); it++) {
		int x = it -> first * 2 + 1, y = it -> second * 2 + 1;

		if (pInfo -> inBorder(it -> first, it -> second)) {
#ifdef DEBUG
			fprintf(stderr, "[INFO] (%d, %d) is in border\n", it -> first, it -> second);
#endif 
			for (int k = 0; k < 4; k++) {
				int tx = x + vertical_dx[k],
					ty = y + vertical_dy[k];
				if (inTable(tx, ty) && (tx == 0 || tx == table_m - 1 || ty == 0 || ty == table_n - 1) ) {
					pGraph -> addDirectedEdge(&table[1][x][y], &table[0][tx][ty], 1);
#ifdef DEBUG
					fprintf(stderr, "[INFO] (%d, %d) - (%d, %d)\n", x, y, tx, ty);
#endif 
				}
			}
		}

		pGraph -> addDirectedEdge(&source, &table[0][x][y], 1);

		for (int k = 0; k < 4; k++) {
			int tx = x + incline_dx[k],
				ty = y + incline_dy[k];
			if (inTable(tx, ty)) {
				pGraph -> addDirectedEdge(&table[1][x][y], &table[0][tx][ty], 1);
			}
		}
	}

	if (pInfo -> hasExit(0)) 
		for (int x = 0, y = 1; y < table_n - 1; y++)
			pGraph -> addDirectedEdge(&table[1][x][y], &sink, 1);

	if (pInfo -> hasExit(1))
		for (int x = 1, y = table_n - 1; x < table_m - 1; x++)
			pGraph -> addDirectedEdge(&table[1][x][y], &sink, 1);

	if (pInfo -> hasExit(2))
		for (int x = table_m - 1, y = table_n - 2; y > 0; y--)
			pGraph -> addDirectedEdge(&table[1][x][y], &sink, 1);

	if (pInfo -> hasExit(3))
		for (int x = table_m - 2, y = 0; x > 0; x--)
			pGraph -> addDirectedEdge(&table[1][x][y], &sink, 1);

	for (int x = 1; x < table_m - 1; x++)
		for (int y = 1; y < table_n - 1; y++) {
			if (x % 2 == 1 && y % 2 == 1) continue;
			for (int k = 0; k < 2; k++) {
				int tx = x + vertical_dx[k],
					ty = y + vertical_dy[k];
				if (tx % 2 == 1 && ty % 2 == 1) continue;
				if (inTable(tx, ty)) {
					pGraph -> addUndirectedEdge(&table[1][x][y], 
							&table[0][tx][ty], 
							1);
				}
			}
		}
}

bool OrderFinder::find() {
	buildTable();
	int sum = pGraph -> run();
	if (sum == pInfo -> size()) {
		for (auto it = pInfo -> begin(); it != pInfo -> end(); it++) {
			int x = it -> first * 2 + 1,
				y = it -> second * 2 + 1;
#ifdef DEBUG
			fprintf(stderr, "###################\nx = %d y = %d\n", x, y);
#endif 
			for (auto node = &table[0][x][y]; node != &sink; ) {
#ifdef DEBUG
				fprintf(stderr, "%p <-> (%d, %d)\n", node, node -> x, node -> y);
#endif 
				node -> x = x;
				node -> y = y;
				bool changed = false;
				for (auto eIt = node -> begin(); eIt != node -> end(); eIt++) {
					CapEdge *edge = static_cast<CapEdge*>(*eIt);
					if (edge -> cap == 0 && edge -> walkable) {
						node = static_cast<DisNode*>(edge -> getTail());
						changed = true;
						break;
					}
				}
				assert(changed);
			}
		}
#ifdef DEBUG
		fprintf(stderr, "[INFO] label done\n");
#endif 
		int index = 0;
		if (pInfo -> hasExit(0)) 
			for (int x = 0, y = 1; y < table_n - 1; y++)
				if (table[1][x][y].x != x || table[1][x][y].y != y) 
					pInfo -> pins[index++] = std::make_pair((table[1][x][y].x - 1) / 2, (table[1][x][y].y - 1) / 2);

		if (pInfo -> hasExit(1))
			for (int x = 1, y = table_n - 1; x < table_m - 1; x++)
				if (table[1][x][y].x != x || table[1][x][y].y != y) 
					pInfo -> pins[index++] = std::make_pair((table[1][x][y].x - 1) / 2, (table[1][x][y].y - 1) / 2);

		if (pInfo -> hasExit(2))
			for (int x = table_m - 1, y = table_n - 2; y > 0; y--)
				if (table[1][x][y].x != x || table[1][x][y].y != y) 
					pInfo -> pins[index++] = std::make_pair((table[1][x][y].x - 1) / 2, (table[1][x][y].y - 1) / 2);

		if (pInfo -> hasExit(3))
			for (int x = table_m - 2, y = 0; x > 0; x--)
				if (table[1][x][y].x != x || table[1][x][y].y != y) 
					pInfo -> pins[index++] = std::make_pair((table[1][x][y].x - 1) / 2, (table[1][x][y].y - 1) / 2);
		return true;	
	}
	return false;
}

Generator::Generator() : pInfo { nullptr }, pOrderFinder { new OrderFinder() } { }
Generator::Generator(BasicInfo &info) : pInfo { &info }, pOrderFinder { new OrderFinder() } { }

Generator::~Generator() {
	delete pOrderFinder;
}

int Generator::lrand(int l, int r) {
	std::random_device rd;
	std::mt19937 re(rd());
	std::uniform_int_distribution<int> rand(l, r);
	return rand(re);
}

void Generator::load(BasicInfo &info) {
	pInfo = &info;
}

GridInfo Generator::gen(int cnt) {

	GridInfo grid( pInfo -> m,
				   pInfo -> n,
				   pInfo -> escapeCode );		

	std::set< std::pair<int, int> > indexSet;

	while (static_cast<int>(indexSet.size()) < cnt) {
		int x = lrand(0, pInfo -> m - 1),
			y = lrand(0, pInfo -> n - 1);
		auto xy = std::make_pair(x, y);
		auto ret = indexSet.insert(xy);
		if (ret.second)
			grid.setPin(x, y);
	}

	pOrderFinder -> load(grid);
	
	if (!pOrderFinder -> find())
		fprintf(stderr, "[ERROR] NO FEASIBLE DATA\n");

	return grid;
}
