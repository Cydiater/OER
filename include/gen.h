#ifndef GEN_H
#define GEN_H

#include <random>
#include <set>

#include "io.h"
#include "graph.h"

class CapEdge;
class DisNode;

class CapEdge : public Edge {
	
	public:
		int cap;
		CapEdge *rev;
		bool walkable;

		CapEdge(DisNode* _head, DisNode* _tail, int _cap, bool _walkable);
		virtual ~CapEdge() = default;
};

class DisNode : public Node {

	public:
		DisNode() = delete;
		DisNode(int _x, int _y);
		virtual ~DisNode() = default;

		int x, y, dis;
};

class FlowGraph {

	std::vector<DisNode*> nodes;

	bool bfs();
	int dfs(DisNode *node, int flow);

	DisNode *source, *sink;

	public:
		FlowGraph();
		~FlowGraph() = default;

		void addNode(DisNode &node);
		void addUndirectedEdge(DisNode *head, DisNode *tail, int cap);
		void addDirectedEdge(DisNode *head, DisNode *tail, int cap);
		void setSource(DisNode &node);
		void setSink(DisNode &node);

		void clear();
		int run();
};

class OrderFinder {
	GridInfo *pInfo;
	FlowGraph *pGraph;

	inline bool inTable(int x, int y);

	int table_m, table_n;

	std::vector< std::vector<DisNode> > table[2];
	DisNode source, sink;

	void buildTable();

	public:
		OrderFinder(GridInfo &grid);
		OrderFinder();

		~OrderFinder();

		void load(GridInfo &grid);
		bool find();
};

class Generator {

	int lrand(int l, int r);

	BasicInfo *pInfo;	
	OrderFinder *pOrderFinder;

	public:	
		Generator();
		Generator(BasicInfo &info);

		~Generator();

		void load(BasicInfo &info);
		GridInfo gen(int cnt);
};

#endif
