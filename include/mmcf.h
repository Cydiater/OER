#ifndef MMCF_H
#define MMCF_H
#include <unordered_set>
#include "gurobi_c++.h"

#include "graph.h"
#include "io.h"

class InterNode;
class InterEdge;
class InterGraph;

class InterNode : public Node {/*{{{*/
	public:

	GRBLinExpr* Exprs;
	int x, y;

	InterNode(int paraX = -1, int paraY = -1);

	void genExprs(int count);
};/*}}}*/

class InterEdge : public Edge {/*{{{*/
	GRBVar* vars;

	public:

	int cap, cost;

	InterEdge(	InterNode* paraHead, 
				InterNode* paraTail, 
				int paraCap, 
				int paraCost);

	~InterEdge() = default;

	void genVars(GRBModel *pModel, int count);
	GRBVar getVar(int pos);
};/*}}}*/

class InterGraph {/*{{{*/

	std::unordered_set<InterNode*> nodedb;	

	public:

	typedef std::unordered_set<InterNode*>::iterator iterator;
		
	InterGraph();

	virtual ~InterGraph();

	void addNode(InterNode* node);

	InterEdge* addEdge(InterNode* head, InterNode* tail, int cap, int cost); 

	iterator begin();

	iterator end(); 

	std::size_t getNodeSize(); 

};/*}}}*/

struct Commodity {/*{{{*/
	InterNode *source, *sink;	
	int traffic;

	Commodity(InterNode* paraSource, InterNode* paraSink, int paraTraffic);
};/*}}}*/

class MMCF : public InterGraph {/*{{{*/

	std::vector<Commodity> coms;
	
	GRBEnv* pEnv;
	GRBModel* pModel;

	public:

	MMCF();
	~MMCF() = default;

	std::pair<int, int> walk_along_flow(InterNode* &pNode, int index);
	void add_commodity(InterNode* source, InterNode* sink, int traffic);	
	int run();

};/*}}}*/

#endif
