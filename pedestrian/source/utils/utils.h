#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <fstream>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <dlib/image_loader/load_image.h>
#include <dlib/image_transforms.h>
#include <dlib/opencv/cv_image.h>
#include <opencv2/opencv.hpp>
#include <type_traits>
#include "../train/trainfhog.h"
class Utils
{

//	void fillMat(std::string &path, std::vector<cv::Mat> &dstList, std::vector<int> &labels, cv::Size pedSize, bool isNeg = false)
//	{
//		assert(!path.empty());
//		cv::Mat frame;
//		std::fstream sampleFile(path);
//		std::string oSample;
//		while (sampleFile >> oSample) {
//
//			frame = cv::imread(oSample, CV_32FC3);
//			if (frame.empty())		std::cout << "fail" << std::endl;
//			cv::resize(frame, frame, pedSize);
//			dstList.push_back(frame.clone());
//		}
//		if (!isNeg) {
//			labels.push_back(1);
//		}
//		else {
//			labels.push_back(0);
//		}
//	}
//
//	void fillArray(std::string &path, std::vector<dlib::array2d < dlib::bgr_pixel >> &dstList, std::vector<int> &labels, cv::Size pedSize, bool isNeg = false)
//	{
//		assert(!path.empty());
//		dlib::array2d < dlib::bgr_pixel > frame;
//		std::fstream sampleFile(path);
//		std::string oSample;
//		while (sampleFile >> oSample) {
//			dlib::array2d < dlib::bgr_pixel > img;
//			//dlib::cv_image<dlib::bgr_pixel> temp(frame);
//			dlib::assign_image(img, frame);
//			dstList.push_back(img);
//		}
//		if (!isNeg) {
//			labels.push_back(1);
//		}
//		else {
//			labels.push_back(0);
//		}
//	}

public:

	static std::vector < cv::Rect > vecDlibRectangle2VecOpenCV(std::vector< dlib::rectangle > r)
	{
		std::vector < cv::Rect > rects;
		for(int i = 0; i < r.size(); i++)
		 rects.push_back(cv::Rect(cv::Point2i(r[i].left(), r[i].top()), cv::Point2i(r[i].right() + 1, r[i].bottom() + 1)));

		return rects;
	}

	static void fillSamples2List(std::string &path, std::vector<cv::Mat> &dstList, std::vector< int > &labels, cv::Size pedSize, bool isNeg = false)
	{
		assert(!path.empty());
		cv::Mat frame;
		std::fstream sampleFile(path);
		std::string oSample;
		while (sampleFile >> oSample) {

			frame = cv::imread(oSample, CV_32FC3);
			if (frame.empty())		std::cout << "fail" << std::endl;
			cv::resize(frame, frame, pedSize);
			dstList.push_back(frame.clone());
			if (!isNeg) {
				labels.push_back(1);
			}
			else {
				labels.push_back(0);
			}
		}
	}
	static void fillSamples2List(std::string &path, std::vector< dlib::matrix < TrainFHog::pixel_type> > &dstList, std::vector<float> &labels, cv::Size pedSize, bool isNeg = false)
	{
	//	int i = 0;
		assert(!path.empty());
		cv::Mat frame;
		//dlib::array2d < dlib::bgr_pixel > frame;
		std::fstream sampleFile(path);
		std::string oSample;
		while (sampleFile >> oSample) {
			frame = cv::imread(oSample, CV_32FC3);
			if (frame.empty())		std::cout << "fail" << std::endl;
			cv::resize(frame, frame, pedSize);
			dlib::cv_image<TrainFHog::pixel_type> cvTmp(frame);
			dlib::matrix<TrainFHog::pixel_type> test = dlib::mat(cvTmp);
			dstList.push_back(test);
			//dlib::pyramid_up(dstList);
			//i++;
		//	if (i == 6)
			//	break;
		}
		if (!isNeg) {
			labels.push_back(1);
		}
		else {
			labels.push_back(0);
		}
	}

//	static dlib::array2d<dlib::rgb_pixel> toDLib(cv::Mat px)
//	{
//		dlib::array2d<dlib::rgb_pixel> out;
//		int width = px.cols;
//		int height = px.rows;
//		int ch = px.channels();
//
//		out.set_size(height, width);
//		const unsigned char* data = px.data;
//		for (int n = 0; n < height; n++)
//		{
//			const unsigned char* v = &data[n * width *  ch];
//			for (int m = 0; m < width; m++)
//			{
//				if (ch == 1)
//				{
//					unsigned char p = v[m];
//					dlib::assign_pixel(out[n][m], p);
//				}
//				else {
//					dlib::rgb_pixel p;
//					p.red = v[m * 3];
//					p.green = v[m * 3 + 1];
//					p.blue = v[m * 3 + 2];
//					dlib::assign_pixel(out[n][m], p);
//				}
//			}
//		}
//		return out;
//	}
};

#endif // UTILS_H