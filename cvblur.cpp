#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 5) {
		kit::usage(argv[0], "<avg|med|gauss> <radius/sigma>");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);

	int radius = atoi(argv[4]);
	double sigma = atof(argv[4]);
	char method = argv[3][0];

	if (radius < 0 || radius > 16 || (method == 'g' && (sigma < 0.5 || sigma > 5)))
		throw std::runtime_error("radius/sigma out of range");

	switch (method) {
	case 'a':
		img.blur_avg(radius);
		break;
	case 'm':
		img.blur_med(radius);
		break;
	case 'g':
		img.blur_gauss(sigma);
		break;
	default:
		throw std::runtime_error("unknown blur method");
	}

	img.save(argv[2]);

	return 0;
}
