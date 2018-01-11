#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 3) {
		kit::usage(argv[0], "");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_COLOR);
	img.save(argv[2]);

	return 0;
}
