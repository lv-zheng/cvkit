#ifndef KIT_HPP
#define KIT_HPP

#include <iostream>

#include <opencv2/opencv.hpp>

#define whassert(x) do { if (!(x)) \
	{ throw std::runtime_error("invalid size"); } } while (0)

namespace kit {

struct handle {
	handle(std::string filein, cv::ImreadModes flags = cv::IMREAD_COLOR) {
		img = cv::imread(filein, flags);
		if (!img.data)
			throw std::runtime_error("fail to read image");
	}

	void save(std::string fileout) {
		std::vector<int> params;
		if (fileout.size() > 4 && fileout.substr(fileout.size() - 4) == ".jpg")
			params.push_back(cv::IMWRITE_JPEG_QUALITY);
		else
			params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		bool ok = cv::imwrite(fileout, img, params);
		if (!ok)
			throw std::runtime_error("fail to write image");
	}

	void to_gray(char method);
	uchar otsu();
	void threshold(int lo, int hi);
	void add(cv::Mat rhs);
	void sub(cv::Mat rhs);
	void mul(cv::Mat rhs);
	void triangle(cv::Mat rhs);

	cv::Mat histogram2img(unsigned height = 256);

	void scale(double cols, double rows, bool bilinear);

	void equalize(int lo, int hi);

	void blur_avg(int radius);
	void blur_med(int radius);
	void blur_gauss(double sigma);

	void edge_sobel();
	void edge_laplacian();
	void edge_neglap();
	void edge_canny(double thres);

	void bin_erode(const cv::Mat& rhs);
	void bin_dilate(const cv::Mat& rhs);

	void gmorph_erode(int radius);
	void gmorph_dilate(int radius);

	cv::Mat img;
};

static inline void usage(const char *prog, const char *extra)
{
	std::cerr << "usage: " << prog << " <filein> <fileout> " << extra << std::endl;
}

} // namespace kit

#endif
