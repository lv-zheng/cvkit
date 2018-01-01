#ifndef KIT_HPP
#define KIT_HPP

#include <iostream>

#include <opencv2/opencv.hpp>

namespace kit {

struct handle {
	handle(char **argv) {
		filein = argv[1];
		fileout = argv[2];
		img = cv::imread(filein);
		if (!img.data)
			throw std::runtime_error("fail to read image");
	}

	void save() {
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

	std::string filein, fileout;
	cv::Mat img;
};

static inline void usage(const char *prog, const char *extra)
{
	std::cerr << "usage: " << prog << " <filein> <fileout> " << extra << std::endl;
}

static inline double limit01(double v)
{
	return v < 0 ? 0 : v > 1 ? 1 : v;
}

} // namespace kit

#endif
