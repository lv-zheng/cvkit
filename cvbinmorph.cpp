#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 5) {
		kit::usage(argv[0], "<ero|dil|ope|clo> <struct-file>");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);
	kit::handle jmg(argv[4], cv::IMREAD_GRAYSCALE);

	switch (argv[3][0]) {
	case 'e':
		img.bin_erode(jmg.img);
		break;
	case 'd':
		img.bin_dilate(jmg.img);
		break;
	case 'o':
		img.bin_erode(jmg.img);
		img.bin_dilate(jmg.img);
		break;
	case 'c':
		img.bin_dilate(jmg.img);
		img.bin_erode(jmg.img);
		break;
	default:
		throw std::runtime_error("unknown morphology operation");
	}

	img.save(argv[2]);

	return 0;
}
