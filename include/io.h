#ifndef IO_H
#define IO_H

#include <vector>
#include <cmath>
#include <iostream>

extern int px2tax(int x);
extern int py2tay(int y);
extern std::pair<int, int> pxy2taxy(std::pair<int, int> xy);
extern int tx2tax(int x);
extern int ty2tay(int y);

class BasicInfo {/*{{{*/

	public:

	int escapeCode;
	int m, n;

	BasicInfo(int paraM, int paraN, int paraEscapeCode);

	bool inBorder(int x, int y);
	bool hasExit(int side);
	int border_size();
};/*}}}*/

class GridInfo : public BasicInfo { /*{{{*/

	public:

	GridInfo(int paraM, int paraN, int paraEscapeCode);

	std::vector<std::pair<int, int>> pins;
	typedef std::vector<std::pair<int, int>>::iterator iterator;

	void setPin(int x, int y);
	iterator begin();
	iterator end();
	int size();

	friend std::ostream& operator<<(std::ostream &os, const GridInfo &info);

};/*}}}*/

class RouteResult : public BasicInfo {/*{{{*/

	typedef std::vector<std::pair<int, int>> route_t;
	
	std::vector<route_t> routes;
	int length;
	
	public:

	typedef std::vector<route_t>::iterator iterator;

	RouteResult(const BasicInfo& rhs, int _length);
	RouteResult (int paraM, int paraN, int paraEscapeCode, int _length);

	void startNewRoute(std::pair<int, int> start);
	void expandLastRoute(std::pair<int, int> node);
	iterator begin();
	iterator end();
	int getLength();

};/*}}}*/

#endif
