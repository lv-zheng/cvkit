#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 4) {
		kit::usage(argv[0], "<r|g|b|avg|lum>");
		return 1;
	}

	kit::handle img(argv);

	char methodc = argv[3][0];
	img.to_gray(methodc);

	img.save();

	return 0;
}
