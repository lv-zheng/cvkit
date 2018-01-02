#include <cmath>
#include <deque>
#include <vector>

#include "kit.hpp"

namespace kit {

static inline double limit01(double v)
{
	return v < 0 ? 0 : v > 1 ? 1 : v;
}

static inline uchar limit0255(long v)
{
	return v < 0 ? 0 : v > 255 ? 255 : v;
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

static cv::Mat matsize2(cv::Mat a, cv::Mat b)
{
	assert(a.type() == b.type());
	auto cols = std::min(a.cols, b.cols);
	auto rows = std::min(a.rows, b.rows);
	return cv::Mat(cv::Size(cols, rows), a.type());
}

void handle::add(cv::Mat rhs)
{
	assert(img.type() == CV_8UC1);

	cv::Mat dst = matsize2(img, rhs);
	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			int value = (int) img.at<uchar>(i, j) +
					(int) rhs.at<uchar>(i, j);
			dst.at<uchar>(i, j) = limit0255(value);
		}
	}

	img = dst;
}

void handle::sub(cv::Mat rhs)
{
	assert(img.type() == CV_8UC1);

	cv::Mat dst = matsize2(img, rhs);
	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			int value = (int) img.at<uchar>(i, j) -
					(int) rhs.at<uchar>(i, j);
			dst.at<uchar>(i, j) = limit0255(value);
		}
	}

	img = dst;
}

void handle::mul(cv::Mat rhs)
{
	assert(img.type() == CV_8UC1);

	cv::Mat dst = matsize2(img, rhs);
	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			double value = (double) img.at<uchar>(i, j) *
					(double) rhs.at<uchar>(i, j) /
					255.0;
			dst.at<uchar>(i, j) = limit0255(std::round(value));
		}
	}

	img = dst;
}

void handle::triangle(cv::Mat rhs)
{
	assert(img.type() == CV_8UC1);

	cv::Mat dst = matsize2(img, rhs);
	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			double x = (double) img.at<uchar>(i, j) / 255;
			double y = (double) rhs.at<uchar>(i, j) / 255;
			dst.at<uchar>(i, j) = limit0255(std::round(std::sqrt(x * x + y * y) * 255));
		}
	}

	img = dst;
}

cv::Mat handle::histogram2img(unsigned height)
{
	assert(img.type() == CV_8UC1);

	cv::Mat res(cv::Size(256, height), CV_8UC1);
	auto his = histogram(img);
	auto mx = *std::max_element(his.begin(), his.end());
	for (int i = 0; i < 256; ++i) {
		unsigned h = (unsigned long)
				his[i] * height / mx;
		for (unsigned j = 0; j < height; ++j)
			res.at<uchar>(height - 1 - j, i) = (j < h ? 0 : 255);
	}

	return res;
}

static uchar at_ranged_scale(const cv::Mat &img, int x, int y)
{
	if (x < 0)
		x = 0;
	if (x >= img.rows)
		x = img.rows - 1;
	if (y < 0)
		y = 0;
	if (y >= img.cols)
		y = img.cols - 1;
	return img.at<uchar>(x, y);
}

static uchar at_ranged(const cv::Mat &img, int x, int y)
{
	if (x < 0 || x >= img.rows ||
			y < 0 || y >= img.cols)
		return 0;
	return img.at<uchar>(x, y);
}

static uchar pick_nearest(const cv::Mat& img, double x, double y)
{
	assert(img.type() == CV_8UC1);
	x *= img.rows;
	y *= img.cols;
	//std::cout << x << ' ' << y << std::endl;

	return at_ranged_scale(img, std::floor(x), std::floor(y));
}

static uchar pick_bilinear(const cv::Mat& img, double x, double y)
{
	assert(img.type() == CV_8UC1);
	x = x * img.rows - 0.5;
	y = y * img.cols - 0.5;
	int i = std::floor(x);
	int j = std::floor(y);
	double di = x - i;
	double dj = y - j;
	//std::cout << i << j << ' ' << di << ' ' << dj << std::endl;

	double v1 = at_ranged_scale(img, i, j) * (1 - di) +
		at_ranged_scale(img, i + 1, j) * di;
	double v2 = at_ranged_scale(img, i, j + 1) * (1 - di) +
		at_ranged_scale(img, i + 1, j + 1) * di;
	double v = v1 * (1 - dj) + v2 * dj;

	return limit0255(std::round(v));
}

void handle::scale(double cols, double rows, bool bilinear)
{
	assert(img.type() == CV_8UC1);
	assert(cols >= 1);
	assert(rows >= 1);

	cv::Mat dst(cv::Size(std::ceil(cols), std::ceil(rows)), CV_8UC1);

	std::function<uchar(const cv::Mat&, double, double)> pick;
	if (bilinear)
		pick = pick_bilinear;
	else
		pick = pick_nearest;

	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			double x = i + 0.5;
			double y = j + 0.5;
			dst.at<uchar>(i, j) = pick(img,
					x / dst.rows,
					y / dst.cols);
		}
	}

	img = dst;
}

void handle::equalize(int lo, int hi)
{
	assert(img.type() == CV_8UC1);
	assert(lo != hi);

	uchar *p = img.data;
	for (int i = 0; i < img.cols * img.rows; ++i)
		p[i] = limit0255(std::round((double) (p[i] - lo) / (hi - lo) * 255));
}

void handle::blur_avg(int radius)
{
	assert(img.type() == CV_8UC1);
	assert(radius >= 0);

	cv::Mat dst = img.clone();
	long area = (long) (radius * 2 + 1) * (radius * 2 + 1);

	for (int i = 0; i < img.rows; ++i) {
		long sum = 0;
		for (int x = -radius; x <= radius; ++x)
			for (int y = -radius - 1; y < radius; ++y)
				sum += at_ranged_scale(img, i + x, y);

		for (int j = 0; j < img.cols; ++j) {
			for (int x = -radius; x <= radius; ++x) {
				sum += at_ranged_scale(img, i + x, j + radius);
				sum -= at_ranged_scale(img, i + x, j - radius - 1);
			}
			dst.at<uchar>(i, j) = limit0255(std::round((double) sum / area));
		}
	}

	img = dst;
}

void handle::blur_med(int radius)
{
	assert(img.type() == CV_8UC1);
	assert(radius >= 0);

	cv::Mat dst = img.clone();
	long area = (long) (radius * 2 + 1) * (radius * 2 + 1);
	int med = area / 2;

	std::vector<uchar> buf(area);
	for (int i = 0; i < img.rows; ++i) {
		std::deque<uchar> q;
		for (int x = -radius; x <= radius; ++x)
			for (int y = -radius - 1; y < radius; ++y)
				q.push_back(at_ranged_scale(img, i + x, y));

		for (int j = 0; j < img.cols; ++j) {
			for (int x = -radius; x <= radius; ++x) {
				q.push_back(at_ranged_scale(img, i + x, j + radius));
				q.pop_front();
			}
			assert((long) q.size() == area);
			std::copy(q.begin(), q.end(), buf.begin());
			std::nth_element(buf.begin(), buf.begin() + med, buf.end());
			dst.at<uchar>(i, j) = buf[med];
		}
	}

	img = dst;
}

static std::vector<std::vector<double>> get_gauss_matrix(double sigma)
{
	// 3-sigma rule
	int radius = std::ceil(sigma * 3);
	int diam = 2 * radius + 1;
	std::vector<std::vector<double>> res(diam, std::vector<double>(diam));

	double pi = 2 * std::acos(0);
	double sigma2 = sigma * sigma;

	for (int i = -radius; i <= radius; ++i) {
		for (int j = -radius; j <= radius; ++j) {
			res[i + radius][j + radius] =
				std::exp(-(i * i + j * j) / (2 * sigma2)) /
				(2 * pi * sigma2);
		}
	}

	return res;
}

static cv::Mat apply_mat(const cv::Mat img, const std::vector<std::vector<double>> mat)
{
	assert(img.type() == CV_8UC1);

	cv::Mat dst = img.clone();
	int radius = mat.size() / 2;

	for (int i = 0; i < img.rows; ++i) {
		for (int j = 0; j < img.cols; ++j) {
			double value = 0;
			for (int x = -radius; x <= radius; ++x)
				for (int y = -radius; y <= radius; ++y)
					value += mat[x + radius][y + radius] * at_ranged_scale(img, i + x, j + y);
			dst.at<uchar>(i, j) = limit0255(std::round(value));
		}
	}

	return dst;
}

void handle::blur_gauss(double sigma)
{
	assert(img.type() == CV_8UC1);
	assert(3 * sigma >= 1);

	auto gmat = get_gauss_matrix(sigma);

	img = apply_mat(img, gmat);
}

void handle::edge_sobel()
{
	assert(img.type() == CV_8UC1);

	std::vector<std::vector<double>> gx = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1},
	};

	std::vector<std::vector<double>> gy = {
		{1, 2, 1},
		{0, 0, 0},
		{-1, -2, -1},
	};

	auto imgx = apply_mat(img, gx);
	auto imgy = apply_mat(img, gy);
	img = imgx;
	triangle(imgy);
}

void handle::edge_laplacian()
{
	// TODO
}

void handle::edge_neglap()
{
	// TODO
}

} // namespace kit
