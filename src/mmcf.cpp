#include "mmcf.h"

InterNode::InterNode(int paraX, int paraY) : /*{{{*/
	Exprs { nullptr }, 
	x {paraX}, 
	y {paraY} { };/*}}}*/

void InterNode::genExprs(int count) {/*{{{*/
	Exprs = new GRBLinExpr[count];
}/*}}}*/

InterEdge::InterEdge( InterNode* paraHead, /*{{{*/
					  InterNode* paraTail, 
					  int paraCap, 
					  int paraCost) :
	Edge(paraHead, paraTail),
	vars { nullptr },
	cap  { paraCap },
	cost { paraCost } { }/*}}}*/

void InterEdge::genVars(GRBModel *pModel, int count) {/*{{{*/
	vars = pModel -> addVars(count, GRB_BINARY)	;
}/*}}}*/

GRBVar InterEdge::getVar(int pos) {/*{{{*/
	return vars[pos];
}/*}}}*/

InterGraph::~InterGraph() {/*{{{*/
#ifdef DEBUG
	fprintf(stderr, "[INFO - clear] destruct InterGraph begin\n");
#endif
	for (auto node: nodedb) 
		delete node;
#ifdef DEBUG
	fprintf(stderr, "[INFO - clear] destruct InterGraph done\n");
#endif
}/*}}}*/

InterGraph::InterGraph() {/*{{{*/
#ifdef DEBUG
	fprintf(stderr, "[INFO - InterGraph] InterGraph initialize done\n");
#endif
}/*}}}*/

MMCF::MMCF() : InterGraph(),/*{{{*/
			   pEnv { new GRBEnv(true) } { 

	pEnv -> set(GRB_IntParam_OutputFlag, 0);
	pEnv -> start();

	pModel = new GRBModel(*pEnv);

#ifdef DEBUG
	 fprintf(stderr, "[INFO - MMCF] environment prepare done\n");
#endif

};/*}}}*/

InterEdge* InterGraph::addEdge(InterNode* head, InterNode* tail, int cap, int cost) {/*{{{*/
#ifdef DEBUG
	assert(cap > 0);
	assert(head != nullptr);
	assert(tail != nullptr);
#endif
	addNode(head);
	addNode(tail);
	return new InterEdge(head, tail, cap, cost);
}/*}}}*/

InterGraph::iterator InterGraph::begin() {/*{{{*/
	return nodedb.begin();
}/*}}}*/

InterGraph::iterator InterGraph::end() {/*{{{*/
	return nodedb.end();
}/*}}}*/

std::size_t InterGraph::getNodeSize() {/*{{{*/
	return nodedb.size();
}/*}}}*/

void InterGraph::addNode(InterNode* node) {/*{{{*/
	nodedb.insert(node);	
}	/*}}}*/

Commodity::Commodity(InterNode* paraSource, InterNode* paraSink, int paraTraffic) :/*{{{*/
	source { paraSource },
	sink { paraSink },
	traffic { paraTraffic } { };/*}}}*/

void MMCF::add_commodity(InterNode* source, InterNode *sink, int traffic) {/*{{{*/
	coms.push_back( Commodity(source, sink, traffic) );
}/*}}}*/

int MMCF::run() {/*{{{*/

	int coms_size = static_cast<int>(coms.size());

	GRBLinExpr objExpr {};
	for (auto it = begin(); it != end(); it++) {
		InterNode *pNode = *it;
		pNode -> genExprs(coms_size);
		GRBLinExpr out_edges {};
		for (auto eIt = pNode -> begin(); eIt != pNode -> end(); eIt++) {
			InterEdge *pEdge = static_cast<InterEdge*>(*eIt);
			pEdge -> genVars(pModel, coms_size);
			GRBLinExpr tmp {};		
			for (int i = 0; i < coms_size; i++) {
				tmp += pEdge -> getVar(i);
				out_edges += pEdge -> getVar(i);
				objExpr += pEdge -> getVar(i) * pEdge -> cost;
			}
			pModel -> addConstr(tmp <= pEdge -> cap);
		}
		pModel -> addConstr(out_edges <= 1);
	}	

	pModel -> setObjective(objExpr, GRB_MINIMIZE);

	int index = 0;
	for (auto commodity: coms) {
		InterNode *source = commodity.source, *sink = commodity.sink;	
		for (auto it = begin(); it != end(); it++) {
			InterNode *pNode = *it;
			for (auto eIt = pNode -> begin(); eIt != pNode -> end(); eIt++) {
				InterEdge *pEdge = static_cast<InterEdge*>(*eIt);
				InterNode *pHead = static_cast<InterNode*>(pEdge -> getHead());
				InterNode *pTail = static_cast<InterNode*>(pEdge -> getTail());
				pHead -> Exprs[index] -= pEdge -> getVar(index);		
				pTail -> Exprs[index] += pEdge -> getVar(index);
			} 
		}
		for (auto it = begin(); it != end(); it++) {
			InterNode *pNode = *it;
			if (pNode == source) pModel -> addConstr(pNode -> Exprs[index] == -1);
			else if (pNode == sink) pModel -> addConstr(pNode -> Exprs[index] == 1);
			else pModel -> addConstr(pNode -> Exprs[index] == 0);
		}
		index++;
	}
	
	pModel -> optimize();

	if (pModel -> get(GRB_IntAttr_Status) == 2 || pModel -> get(GRB_IntAttr_Status) == 10) {
		return static_cast<int>(pModel -> get(GRB_DoubleAttr_ObjVal));
	} else 
		return -1;
}/*}}}*/

std::pair<int, int> MMCF::walk_along_flow(InterNode* &pNode, int index) {/*{{{*/
	while (true) {
#ifdef DEBUG
		int cnt = 0;
#endif
		InterNode* tmp = nullptr;
		for (auto it = pNode -> begin(); it != pNode -> end(); it++) {
			InterEdge* pEdge = static_cast<InterEdge*>(*it);
			if (static_cast<bool>(pEdge -> getVar(index).get(GRB_DoubleAttr_X))) {
				tmp = static_cast<InterNode*>(pEdge -> getTail());
#ifdef DEBUG
				cnt++;
#endif
			}
		}
		pNode = tmp;
#ifdef DEBUG
		assert(cnt == 1);
		fprintf(stderr, "[INFO - walk_along_flow] new address %p with index = %d\n", pNode, index);
#endif
		if (pNode -> x != -1 && pNode -> y != -1) 
			return std::make_pair(pNode -> x, pNode -> y);
	} 
}/*}}}*/
