#ifndef PLOTER_H
#define PLOTER_H

#include <sstream>
#include <fstream>
#include <map>

#include "io.h"

class DotContent {

	std::vector<std::string> colors;
	std::ostringstream raw;
	double penwidth, height, width;
	int current_color, x, y;

	public:

	int m, n;

	DotContent(int para_m, int para_n);
	~DotContent() = default;

	DotContent* addColor(const std::string& color);
	DotContent* setPenwidth(double value);
	DotContent* setHeight(double value);
	DotContent* setWidth(double value);
	DotContent* setFontsize(int value);
	void addCircle(int x, int y, int id);
	void addPoint(int x, int y);
	void startNewRoute(int para_x, int para_y);
	void expandLastRoute(int x, int y);

	friend std::ostream& operator<<(std::ostream &os, const DotContent& content);
};

class Ploter {
	
	RouteResult *pResult;	
	DotContent *pContent;

	public:

	Ploter(RouteResult *result);
	~Ploter();

	void plot();
	void saveAs(std::string filename);

};

#endif
