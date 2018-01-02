#include "kit.hpp"

int main(int argc, char **argv)
{
	if (argc < 2) {
		kit::usage(argv[0], "");
		return 1;
	}

	kit::handle img(argv[1], cv::IMREAD_COLOR);

	std::string fn(argv[1]);
	cv::namedWindow(fn, cv::WINDOW_NORMAL);
	cv::imshow(fn, img.img);

	cv::waitKey();

	return 0;
}
