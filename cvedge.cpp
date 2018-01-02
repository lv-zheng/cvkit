#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 4) {
		kit::usage(argv[0], "<sobel|laplacian|neglap|canny> [threshold]");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_GRAYSCALE);

	char method = argv[3][0];
	double thres;
	if (method == 'c') {
		if (argc < 5 || (thres = atof(argv[4]), thres < 0 || thres > 1))
			throw std::runtime_error("bad threshold");
	}

	switch (method) {
	case 's':
		img.edge_sobel();
		break;
	case 'l':
		img.edge_laplacian();
		break;
	case 'n':
		img.edge_neglap();
		break;
	case 'c':
		img.edge_canny(thres);
		break;
	default:
		throw std::runtime_error("unknown edge detection method");
	}

	img.save(argv[2]);

	return 0;
}
