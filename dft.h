#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

int MAX_KERNEL_LENGTH = 31;

using namespace cv;
using namespace std;

Mat crop_and_rearrange(Mat);

Mat action_switch(int, Mat);

Mat get_dft(Mat);

void get_magnitude(Mat);

Mat get_idft(Mat);

cv::Mat createGaussianHighPassFilter(cv::Size, double, bool);

double pixelDistance(double, double);

double gaussianCoeff(double, double, double);

Mat get_gaussian(Mat, bool);

