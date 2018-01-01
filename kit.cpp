#include <cmath>
#include <vector>

#include "kit.hpp"

namespace kit {

static inline double limit01(double v)
{
	return v < 0 ? 0 : v > 1 ? 1 : v;
}

static std::vector<unsigned> histogram(const cv::Mat img)
{
	assert(img.type() == CV_8UC1);
	std::vector<unsigned> his(256, 0);
	uchar *p = img.data;
	for (int i = 0; i < img.cols * img.rows; ++i)
		++his[p[i]];
	return his;
}

static cv::Mat threshold(const cv::Mat img, uchar thres)
{
	assert(img.type() == CV_8UC1);
	cv::Mat dst = img.clone();
	uchar *p = dst.data;
	for (int i = 0; i < img.cols * img.rows; ++i)
		p[i] = p[i] > thres ? 255 : 0;
	return dst;
}

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

uchar handle::otsu()
{
	assert(img.type() == CV_8UC1);

	auto his = histogram(img);

	unsigned long sum = 0;
	for (int i = 1; i < 256; ++i)
		sum += i * his[i];
	unsigned long sumB = 0;
	double wB = 0;
	double wF = 0;
	double mB;
	double mF;
	double max = 0.0;
	double between = 0.0;
	int threshold1 = 0;
	int threshold2 = 0;
	unsigned long total = img.cols * img.rows;
	for (int i = 0; i < 256; ++i) {
		wB += his[i];
		if (wB == 0)
			continue;
		wF = total - wB;
		if (wF == 0)
			break;
		sumB += i * his[i];
		mB = sumB / wB;
		mF = (sum - sumB) / wF;
		between = wB * wF * (mB - mF) * (mB - mF);
		if (between >= max) {
			threshold1 = i;
			if (between > max)
				threshold2 = i;
			max = between;
		}
	}
	int thres = (threshold1 + threshold2) / 2;

	//cv::Mat dst = img.clone();
	//cv::threshold(img, dst, thres, 255, cv::THRESH_BINARY);
	img = threshold(img, thres);

	return thres;
}

} // namespace kit
