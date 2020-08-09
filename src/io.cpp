#include "io.h"

int px2tax(int x) { return x * 2 + 1; }
int py2tay(int y) { return y * 2 + 1; }
std::pair<int, int> pxy2taxy(std::pair<int, int> xy) { 
	return std::make_pair(px2tax(xy.first), py2tay(xy.second)); 
}
int tx2tax(int x) { return x * 2 + 2; }
int ty2tay(int y) { return y * 2 + 2; }

BasicInfo::BasicInfo(int paraM, int paraN, int paraEscapeCode) :
	escapeCode { paraEscapeCode }, m { paraM }, n { paraN } { };

bool BasicInfo::hasExit(int side) {
	return (escapeCode >> side) & 1;
}

bool BasicInfo::inBorder(int x, int y) {
	return x == 0 || x == m - 1 || y == 0 || y == n - 1;
}

int BasicInfo::border_size() {
	int res = 0;
	if (hasExit(0)) res += n;
	if (hasExit(1)) res += m;
	if (hasExit(2)) res += n;
	if (hasExit(3)) res += m;
	return res;
}

GridInfo::GridInfo(int paraM, int paraN, int paraEscapeCode) :
	BasicInfo(paraM, paraN, paraEscapeCode) { };

void GridInfo::setPin(int x, int y) {
	pins.push_back(std::make_pair(x, y));
#ifdef DEBUG
	fprintf(stderr, "[INFO - read data] set (%d, %d) as PIN\n", x, y);
#endif
}

GridInfo::iterator GridInfo::end() {
	return pins.end();
}

int GridInfo::size() {
	return static_cast<int>(pins.size());
}

GridInfo::iterator GridInfo::begin() {
	return pins.begin();
}

std::ostream& operator<<(std::ostream &os, const GridInfo &info) {
	os << info.m << " " << info.n << " " << info.escapeCode << " " << info.pins.size() << std::endl;
	for (auto xy: info.pins) 
		os << xy.first << " " << xy.second << std::endl;
	return os;
}

RouteResult::RouteResult(const BasicInfo& rhs, int _length) : BasicInfo(rhs), length { _length } { };
RouteResult::RouteResult (int paraM, int paraN, int paraEscapeCode, int _length) :
	BasicInfo(paraM, paraN, paraEscapeCode), length { _length } { };

void RouteResult::startNewRoute(std::pair<int, int> start) {
	routes.push_back( { start } );
}

void RouteResult::expandLastRoute(std::pair<int, int> node) {
	if (!routes.back().empty() && node == routes.back().back()) return;
	routes.back().push_back(node);
}

RouteResult::iterator RouteResult::begin() {
	return routes.begin();
}

RouteResult::iterator RouteResult::end() {
	return routes.end();
}

int RouteResult::getLength() {
	return length;
}
