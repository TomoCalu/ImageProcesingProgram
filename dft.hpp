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

void get_high_or_low_pass_filter(Mat, bool);

void get_band_pass_filter(Mat);

Mat create_high_or_low_pass_filter(Size, double, bool);

double gaussian_coeff(double, double, double);

void get_convolution(Mat);