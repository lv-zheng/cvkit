#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 4) {
		kit::usage(argv[0], "<radius>");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);

	int radius = atoi(argv[3]);

	if (radius < 0 || radius > 16)
		throw std::runtime_error("radius out of range");

	img.blur_avg(radius);

	img.save(argv[2]);

	return 0;
}
