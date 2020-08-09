#include <fstream>

#include "router.h"
#include "ploter.h"

void Usage(char *s) {
	printf("Usage: %s <PDF|DOT filename>\n", s);
	exit(1);
}

int main(int argc, char** argv) {

	if (argc != 2) Usage(argv[0]);

	int m, n, escapeCode, pinSize;
	std::cin >> m >> n >> escapeCode >> pinSize;
	GridInfo data(m, n, escapeCode);
#ifdef DEBUG
	fprintf(stderr, "[INFO - read data] m = %d n = %d escapeCode = %d\n", m, n, escapeCode);
#endif
	while (pinSize--) {
		int x, y;
		std::cin >> x >> y;
		data.setPin(x, y);
	}
#ifdef DEBUG
	fprintf(stderr, "[INFO - read data] done\n");
#endif
	Router router(&data);
	RouteResult *pResult = router.run();

	if (pResult == nullptr) {
		fprintf(stderr, "[INFO] NO SOLUTION\n");
		return 0;
	}

	Ploter ploter(pResult);
	ploter.plot();

	std::string filename { argv[1] };
	if (filename.find(".dot") != std::string::npos) {
		ploter.saveAs(filename);
	} else if (filename.find(".pdf") != std::string::npos) {
		std::string dotname = filename;	
		dotname.replace(dotname.find(".pdf"), 4, ".dot");
#ifdef DEBUG
		printf("[INFO - plot] dotname = %s filename = %s\n", dotname.c_str(), filename.c_str());
#endif
		ploter.saveAs(dotname);
		std::system( ("dot -Tpdf -Kfdp " + dotname + " -o " + filename).c_str() );
	}

#ifdef DEBUG
	printf("[INFO - clear] begin destruct\n");
#endif

	return 0;
}
