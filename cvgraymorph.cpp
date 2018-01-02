#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 5) {
		kit::usage(argv[0], "<ero|dil|ope|clo> <radius>");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);

	char method = argv[3][0];
	int radius = atoi(argv[4]);

	if (radius < 0 || radius > 16)
		throw std::runtime_error("radius out of range");

	switch (method) {
	case 'e':
		img.gmorph_erode(radius);
		break;
	case 'd':
		img.gmorph_dilate(radius);
		break;
	case 'o':
		img.gmorph_erode(radius);
		img.gmorph_dilate(radius);
		break;
	case 'c':
		img.gmorph_dilate(radius);
		img.gmorph_erode(radius);
		break;
	default:
		throw std::runtime_error("unknown morphology operation");
	}

	img.save(argv[2]);

	return 0;
}
