#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 5) {
		kit::usage(argv[0], "<lo> <hi>");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);

	int lo = atoi(argv[3]);
	int hi = atoi(argv[4]);
	if (lo == hi)
		throw std::runtime_error("lo should be different from hi");
	img.equalize(lo, hi);

	img.save(argv[2]);

	return 0;
}
