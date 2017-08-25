#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>

int MAX_KERNEL_LENGTH = 31;

using namespace cv;
using namespace std;

void action_switch(int, Mat);

Mat get_dft(Mat);

void get_idft(Mat);

void get_magnitude(Mat);

Mat crop_and_rearrange(Mat);

void get_gaussian(Mat, bool);

Mat createGaussianHighPassFilter(Size, double, bool);

double gaussianCoeff(double, double, double);

void get_convolution(Mat);