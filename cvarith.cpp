#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 5) {
		kit::usage(argv[0], "<add|sub|mul> <file2>");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);
	kit::handle jmg(argv[4], cv::IMREAD_GRAYSCALE);

	switch (argv[3][0]) {
	case 'a':
		img.add(jmg.img);
		break;
	case 's':
		img.sub(jmg.img);
		break;
	case 'm':
		img.mul(jmg.img);
		break;
	default:
		throw std::runtime_error("unknown arithmetic operation");
	}

	img.save(argv[2]);

	return 0;
}
