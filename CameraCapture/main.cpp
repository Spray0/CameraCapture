#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>
#include "cal_fps.h"
#include <Windows.h>

using namespace cv;
using namespace std;

#define WIN_NAME "Capturing Demo"

using namespace std::literals;
ostream& operator<<(ostream& os, const chrono::time_point<chrono::system_clock>& t)
{
	const auto tt(chrono::system_clock::to_time_t(t)); const auto loct(std::localtime(&tt)); return os << put_time(loct, "%c");
}

long time_stamp_start;
time_t GetCurrentTimeMsec() {
	auto time = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
	time_t timestamp = time.time_since_epoch().count();
	return timestamp;
}

struct config_T
{
	short width;
	short height;
	short fps;
}config_w,config_r;


int main(int argc, char** argv)
{
	FpsCounter fps_counter;

	cv::Mat cvFrame;
	cv::Mat cvFrame2;
	cv::VideoCapture cv_cap;

	time_stamp_start = GetCurrentTimeMsec();

	//设置opencv日志等级
	cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_WARNING);
	//打开指定的UVC摄像头设备
	cv_cap.open(0, cv::CAP_ANY);
	if (!cv_cap.isOpened())
	{
		std::cout << "Camera open failed!" << std::endl;
		exit(-1);
	}
	//设置参数
	cv_cap.set(cv::CAP_PROP_FPS, 60.0);
	cv_cap.set(cv::CAP_PROP_FRAME_WIDTH, 1600);
	cv_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1200);
	cv_cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	config_r.fps = cv_cap.get(cv::CAP_PROP_FPS);
	config_r.width = cv_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	config_r.height = cv_cap.get(cv::CAP_PROP_FRAME_HEIGHT);

	cout << "[ " << chrono::system_clock::now() << " ] ";
	std::cout << "---------------------------------" << std::endl;
	cout << "[ " << chrono::system_clock::now() << " ] ";
	std::cout << "\033[32m" << "	Camera open ok." << "\033[0m" << std::endl;
	cout << "[ " << chrono::system_clock::now() << " ] ";
	std::cout << "Fps: " << config_r.fps << "  Resolution: " << config_r.width << "x" << config_r.height << std::endl;
	cout << "[ " << chrono::system_clock::now() << " ] ";
	std::cout << "---------------------------------" << std::endl;
	cout << "[ " << chrono::system_clock::now() << " ] ";
	std::cout << "Press P to capture..." << std::endl;
	

	//主循环
	cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);
	int key = -1;
	while	(
				(cv_cap.isOpened()) &&
				((key ==-1) || (key == 80) ||(key == 112)) &&
				(getWindowProperty(WIN_NAME, cv::WND_PROP_AUTOSIZE) > 0)
			)
	{

		// Fps计算
		fps_counter.aio();

		//获取帧
		cv_cap >> cvFrame;
		
		// 按键处理
		key = cv::waitKey(1);
		if ((key == 112) || (key == 80)) {
			std::cout << std::endl;
			cout << "[ " << chrono::system_clock::now() << " ] ";
			std::cout << "Key P pressed." << std::endl;
			cv::cvtColor(cvFrame, cvFrame2, cv::COLOR_RGB2GRAY);
			cv::imwrite("capture_8bit.bmp", cvFrame2);
			cout << "[ " << chrono::system_clock::now() << " ] ";
			std::cout << "\033[32m" << "Image saved successfully.  capture_8bit.bmp" << "\033[0m" << std::endl;
		}

		//显示数据
		std::stringstream fmt;
		fmt << "FPS_AVG: " << setprecision(4) << fps_counter.fps_avg;
		cv::putText( cvFrame, fmt.str(),
			cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, 20, false );
		cv::putText( cvFrame, "Press P to capture...",
			cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0), 2, 20, false );

		//刷新至窗口
		cv::imshow(WIN_NAME, cvFrame);
	}
	cv::destroyAllWindows();

	cv_cap.release();

	return 0;
}