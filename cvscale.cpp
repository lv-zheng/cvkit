#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 4) {
		kit::usage(argv[0], "<x|w(idth)|h(eight)>value [b(ilinear)]");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);

	double x = 0;
	double h = 0;
	double w = 0;
	char *v = argv[3] + 1;

	switch (argv[3][0]) {
	case 'x':
		x = atof(v);
		whassert(x > 0);
		w = img.img.cols * x;
		h = img.img.rows * x;
		whassert(w >= 1);
		whassert(h >= 1);
		break;
	case 'w':
		w = atof(v);
		whassert(w > 0);
		x = w / img.img.cols;
		whassert(x > 0);
		h = img.img.rows * x;
		whassert(h >= 1);
		break;
	case 'h':
		w = atof(v);
		whassert(w > 0);
		x = w / img.img.cols;
		whassert(x > 0);
		h = img.img.rows * x;
		whassert(h >= 1);
		break;
	default:
		throw std::runtime_error("unknown scaling method");
	}

	bool bilinear = false;

	if (argc > 4)
		bilinear = true;

	img.scale(w, h, bilinear);

	std::cout << img.img.cols << ' ' << img.img.rows << std::endl;

	img.save(argv[2]);

	return 0;
}
