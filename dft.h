#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

int MAX_KERNEL_LENGTH = 31;

using namespace cv;
using namespace std;

Mat complexI;

Mat expand_to_optimal_size(Mat);

Mat crop_and_rearrange(Mat);

void write_image_to_file(Mat, char*);

Mat action_switch(int, Mat);

void show_images(Mat, Mat);

Mat get_dft(Mat);

Mat get_idft(Mat);