#include "ploter.h"

void DotContent::expandLastRoute(int cur_x, int cur_y) {/*{{{*/
	raw << "\t\"(" << x << "," << y << ")\""
		<< "--" 
		<< "\"(" << cur_x << "," << cur_y << ")\" "
		<< "[ penwidth = 2 "
		<< " color = " << colors[current_color]
		<< "]\n";
	x = cur_x;
	y = cur_y;
}/*}}}*/

void DotContent::addCircle(int x, int y, int k) {/*{{{*/
	raw << "\t\"(" << x << "," << y << ")\" "	
		<< "[pos=\""<< y << "," << m - x - 1 << "!\" "
		<< "shape = \"circle\" penwidth = \"" << penwidth << "\" "
		<< "height = " << height << " "
		<< "width = " << width << " "
		<< (k == -1 ? "label=\"\"" : ("label=\"" + std::to_string(k + 1) + "\""))
		<< "]\n";
}/*}}}*/

void DotContent::addPoint(int x, int y) {/*{{{*/
	raw << "\t\"(" << x << "," << y << ")\" "	
		<< "[pos=\""<< y << "," << m - x - 1 << "!\" "
		<< "shape = \"point\" penwidth = \"" << penwidth << "\" "
		<< "label=\"\"" 
		<< "]\n";
}/*}}}*/

DotContent::DotContent(int para_m, int para_n) : current_color { 0 },/*{{{*/
											     m { para_m }, 
												 n { para_n } { };/*}}}*/

DotContent* DotContent::addColor(const std::string& color) {/*{{{*/
	colors.push_back(color);
	return this;
}/*}}}*/

DotContent* DotContent::setPenwidth(double value) {/*{{{*/
	penwidth = value;
	return this;
}/*}}}*/

DotContent* DotContent::setHeight(double value) {/*{{{*/
	height = value;
	return this;
}/*}}}*/

DotContent* DotContent::setWidth(double value) {/*{{{*/
	width = value;
	return this;
}/*}}}*/

DotContent* DotContent::setFontsize(int value) {/*{{{*/
	raw << "\tnode [fontsize=" << value << "]\n";
	return this;
}/*}}}*/

void DotContent::startNewRoute(int para_x, int para_y) {/*{{{*/
	current_color++;	
	current_color %= static_cast<int>(colors.size());
	x = para_x; y = para_y;
}/*}}}*/

std::ostream& operator<<(std::ostream &os, const DotContent& content) {/*{{{*/
	std::string tmp = content.raw.str();
	os << "Graph board {\n" << tmp << "}";
	return os;
}/*}}}*/

Ploter::Ploter(RouteResult *result) : pResult { result }, /*{{{*/
									  pContent { new DotContent(pResult -> m * 2 + 1, pResult -> n * 2 + 1) } 
									  { };/*}}}*/

void Ploter::plot() {/*{{{*/

	pContent -> addColor("red")
			 -> addColor("blue")
			 -> addColor("purple")
			 -> addColor("green")
			 -> addColor("pink")
			 -> addColor("aqua")
			 -> addColor("orange")
			 -> addColor("peru")
			 -> addColor("brown")
			 -> setPenwidth(2)
			 -> setHeight(1.2)
			 -> setWidth(1.2)
			 -> setFontsize(30);

	int m = pContent -> m, n = pContent -> n;
	std::map< std::pair<int, int>, bool > check_pin;
	int index = 0;
	for (auto it = pResult -> begin(); it != pResult -> end(); it++) {
		auto xy = (*it).front();
		check_pin[xy] = true;
		pContent -> addCircle(xy.first, xy.second, index++);
	}
	for (int x = 1; x < m; x += 2)
		for (int y = 1; y < n; y += 2)
			if (!check_pin[std::make_pair(x, y)])
				pContent -> addCircle(x, y, -1);
	for (int x = 0; x < m; x++)
		for (int y = 0; y < n; y++)
			if (x % 2 == 0 || y % 2 == 0)
				pContent -> addPoint(x, y);
	for (auto it = pResult -> begin(); it != pResult -> end(); it++) {
		auto route = *it;
		bool isFront = true;
		for (auto cur: route) {
			if (isFront) {
				pContent -> startNewRoute(cur.first, cur.second);
				isFront = false;
			} else pContent -> expandLastRoute(cur.first, cur.second);
		}
	}
}/*}}}*/

Ploter::~Ploter() {/*{{{*/
	delete pContent;
}/*}}}*/

void Ploter::saveAs(std::string filename) {/*{{{*/
	std::ofstream out(filename);
	out << *pContent;
}/*}}}*/

