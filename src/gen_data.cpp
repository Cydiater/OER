#include "gen.h"

void Usage(char *s) {
	printf("Usage: %s <m> <n> <status> <size_of_pin>\n", s);
	exit(0);
}

int main(int argc, char **argv) {

	if (argc != 5) Usage(argv[0]);

	int m = std::stoi( std::string(argv[1]) ),
		n = std::stoi( std::string(argv[2]) ),
		size_of_pin = std::stoi( std::string(argv[4]) );

	assert(strlen(argv[3]) == 4);

	int escapeCode = 0;

	for (int i = 0; i < 4; i++) if (argv[3][i] == '1')
		escapeCode |= 1 << i;

	BasicInfo base(m, n, escapeCode);
	Generator gen(base);
	GridInfo info = gen.gen(size_of_pin);

	std::cout << info;

	return 0;
}
