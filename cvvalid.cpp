#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 2) {
		kit::usage(argv[0], "");
		return 1;
	}

	try {
		kit::handle img(argv[1], cv::IMREAD_COLOR);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
