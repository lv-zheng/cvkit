#include <cmath>

#include "kit.hpp"

namespace kit {

void handle::to_gray(char method)
{
	assert(img.type() == CV_8UC3);

	double weight[3] = {0, 0, 0};
	switch (method) {
	case 'r':
		weight[2] = 1;
		break;
	case 'g':
		weight[1] = 1;
		break;
	case 'b':
		weight[0] = 1;
		break;
	case 'a':
		weight[0] = weight[1] = weight[2] = 1.0 / 3;
		break;
	case 'l':
		weight[0] = 0.0722;
		weight[1] = 0.7152;
		weight[2] = 0.2126;
		break;
	default:
		throw std::runtime_error("unknown grayscale method");
	}

	cv::Mat dst(cv::Size(img.cols, img.rows), CV_8UC1);

	for (int i = 0; i < img.rows; ++i) {
		for (int j = 0; j < img.cols; ++j) {
			auto pixel = img.at<cv::Vec3b>(i, j);
			double value = pixel[0] * weight[0] / 255 +
					pixel[1] * weight[1] / 255 +
					pixel[2] * weight[2] / 255;
			value = limit01(value);
			dst.at<uchar>(i, j) = (uchar) std::round(value * 255);
		}
	}

	img = dst;
}

} // namespace kit
